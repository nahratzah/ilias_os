#ifndef _STDIMPL_LOCALE_NUM_PUT_INL_H_
#define _STDIMPL_LOCALE_NUM_PUT_INL_H_

#include <stdimpl/locale_num_put.h>
#include <locale_misc/locale.h>
#include <locale_misc/ctype.h>
#include <locale_misc/numpunct.h>
#include <ios>

_namespace_begin(std)
namespace impl {


template<typename T>
auto num_encoder_base::abs(const T& v) ->
    enable_if_t<is_unsigned<T>::value, const T&> {
  return v;
}

template<typename T>
auto num_encoder_base::abs(const T& v) ->
    enable_if_t<is_signed<T>::value, make_unsigned_t<T>> {
  if (v < 0) return static_cast<make_unsigned_t<T>>(v + 1) + 1U;
  return static_cast<make_unsigned_t<T>>(v);
}


template<typename Char>
basic_num_encoder<Char>::basic_num_encoder(ios_base& str)
: ctype(use_facet<_namespace(std)::ctype<char_type>>(str.getloc())),
  digits(this->get_digits_(
         (str.flags() & ios_base::uppercase) == ios_base::uppercase))
{}

template<typename Char>
auto basic_num_encoder<Char>::get_digits_(bool upper) ->
    array<char_type, 16> {
  array<char_type, 16> rv;
  const char (&source)[16] = (upper ? digits_char_upper : digits_char_lower);

  ctype.widen(begin(source), end(source), begin(rv));
  return rv;
}


template<typename Char, typename T>
num_encoder<Char, T>::num_encoder(value_type v, ios_base& str)
: basic_num_encoder<Char>(str),
  data_(),
  prefix_(),
  data_begin_(data_.end()),
  prefix_begin_(prefix_.end())
{
  make_unsigned_t<value_type> base, u;

  /* Set up variables dealing with the numeric base. */
  string_ref base_str;
  if ((str.flags() & ios_base::basefield) == ios_base::oct) {
    base = 8;
    u = static_cast<make_unsigned_t<value_type>>(v);
    base_str = string_ref("0", 1);
    decimal_ = false;
  } else if ((str.flags() & ios_base::basefield) == ios_base::hex) {
    base = 16;
    u = static_cast<make_unsigned_t<value_type>>(v);
    base_str = string_ref("0x", 2);
    decimal_ = false;
  } else {
    base = 10;
    u = basic_num_encoder<Char>::abs(v);
    /* Initialize sign into prefix. */
    if (v < 0)
      *--prefix_begin_ = this->ctype.widen('-');
    else if ((str.flags() & ios_base::showpos) == ios_base::showpos)
      *--prefix_begin_ = this->ctype.widen('+');
    decimal_ = true;
  }

  /* Initialize showbase string. */
  if ((str.flags() & ios_base::showbase) == ios_base::showbase &&
      !base_str.empty()) {
    assert(prefix_.size() >= base_str.length());
    prefix_begin_ -= base_str.length();
    this->ctype.widen(base_str.begin(), base_str.end(), prefix_begin_);
  }

  /* Decode digits. */
  while (data_begin_ == data_.end() || u != 0) {
    assert(data_begin_ != data_.begin());

    *--data_begin_ = this->digits[u % base];
    u /= base;
  }
}

template<typename Char, typename T>
auto num_encoder<Char, T>::get_digits() const noexcept ->
    basic_string_ref<char_type> {
  return basic_string_ref<char_type>(data_begin_,
                                     data_.end() - data_begin_);
}

template<typename Char, typename T>
auto num_encoder<Char, T>::get_prefix() const noexcept ->
    basic_string_ref<char_type> {
  return basic_string_ref<char_type>(prefix_begin_,
                                     prefix_.end() - prefix_begin_);
}

template<typename Char, typename T>
auto num_encoder<Char, T>::is_decimal() const noexcept -> bool {
  return decimal_;
}


template<typename CountT>
CountT count_grouping_sep(string_ref grouping, CountT ndigits)
    noexcept {
  CountT rv = 0;
  char last_count = 0;  // Count in last group.

  /* Iterate groups. */
  for (auto count : grouping) {
    last_count = count;  // Save last group.
    if (count <= 0 || count == CHAR_MAX ||
        ndigits <= static_cast<unsigned char>(count)) break;  // GUARD
    ndigits -= count;
  }

  /* Last group repeats. */
  if (ndigits > 0 && last_count > 0 && last_count != CHAR_MAX)
    rv += (ndigits - 1U) / last_count;

  return rv;
}

template<typename Char, typename Iter>
auto render_num_encoder_with_sep(Iter out, basic_string_ref<Char> digits,
                                 string_ref spec, Char sep) -> Iter {
  /* Handle case for no grouping. */
  if (spec.empty() || spec[0] <= 0 || spec[0] == CHAR_MAX ||
      digits.length() < static_cast<unsigned char>(spec[0]))
    return copy(digits.begin(), digits.end(), out);

  /* Save grouping count. */
  const string_ref::size_type count = spec[0];
  /* Pop group, unless it is the last one (last group repeats). */
  if (spec.length() > 1) spec = spec.substr(1);

  /* Render remaining groups (recursion). */
  out = render_num_encoder_with_sep(out,
                                    digits.substr(0, digits.length() - count),
                                    spec, sep);
  /* Emit separator and current group. */
  digits = digits.substr(digits.length() - count);
  *out = sep;
  ++out;
  return copy(digits.begin(), digits.end(), out);
}

template<typename Char, typename Iter>
auto render_num_encoder(Iter out, ios_base& str,
                        basic_string_ref<Char> prefix,
                        basic_string_ref<Char> digits,
                        Char fill, bool isdecimal) -> Iter {
  /* Calculate number of thousand separators. */
  Char thousand_sep = 0;
  string grouping;
  size_t n_thousand_seps = 0;
  if (isdecimal) {
    const _namespace(std)::numpunct<Char>& numpunct =
        use_facet<_namespace(std)::numpunct<Char>>(str.getloc());
    thousand_sep = numpunct.thousands_sep();
    grouping = numpunct.grouping();
    n_thousand_seps = count_grouping_sep(grouping, digits.length());
  }

  /* Calculate number of emitted chars, without padding. */
  const auto emit_count = prefix.length() + digits.length() + n_thousand_seps;

  /* Calculate required padding. */
  const make_unsigned_t<streamsize> width = str.width(0);
  auto pad = (width <= emit_count ? 0 : width - emit_count);

  /* Calculate where the padding should occur. */
  bool pad_after = false, pad_before = false, pad_internal = false;
  if (pad > 0) {
    switch (str.flags() & ios_base::adjustfield) {
    case ios_base::left:
    default:
      pad_after = true;
      break;
    case ios_base::right:
      pad_before = true;
      break;
    case ios_base::internal:
      pad_internal = true;
      break;
    }
  }

  /* Emit to output iterator. */
  if (pad_before) out = fill_n(out, pad, fill);
  out = copy(prefix.begin(), prefix.end(), out);
  if (pad_internal) out = fill_n(out, pad, fill);
  out = render_num_encoder_with_sep(out, digits, grouping, thousand_sep);
  if (pad_after) out = fill_n(out, pad, fill);
  return out;
}

template<typename Char, typename Iter, typename T>
auto render_num_encoder(Iter out, ios_base& str,
                        const num_encoder<Char, T>& ne,
                        Char fill) -> Iter {
  return render_num_encoder(out, str,
                            ne.get_prefix(), ne.get_digits(), fill,
                            ne.is_decimal());
}


} /* namespace std::impl */
_namespace_end(std)

#endif /* _STDIMPL_LOCALE_NUM_PUT_INL_H_ */
