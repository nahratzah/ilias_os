#ifndef _STDIMPL_LOCALE_NUMERIC_INL_H_
#define _STDIMPL_LOCALE_NUMERIC_INL_H_

#include <stdimpl/locale_numeric.h>
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

template<typename CType>
auto num_encoder_base::get_digits_(bool upper, const CType& ctype) ->
    array<typename CType::char_type, 16> {
  array<typename CType::char_type, 16> rv;
  const char (&source)[16] = (upper ? digits_char_upper : digits_char_lower);

  ctype.widen(begin(source), end(source), begin(rv));
  return rv;
}


template<typename Char>
basic_num_encoder<Char>::basic_num_encoder(ios_base& str)
: ctype(use_facet<_namespace(std)::ctype<char_type>>(str.getloc())),
  digits(num_encoder_base::get_digits_(
             (str.flags() & ios_base::uppercase) == ios_base::uppercase,
             this->ctype))
{}


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


template<typename Char>
basic_num_decoder<Char>::basic_num_decoder(ios_base& str)
: ctype(use_facet<_namespace(std)::ctype<char_type>>(str.getloc())),
  punct(use_facet<_namespace(std)::numpunct<char_type>>(str.getloc())),
  digits_lower(num_encoder_base::get_digits_(false, this->ctype)),
  digits_upper(num_encoder_base::get_digits_(true, this->ctype)),
  decimal_point(this->punct.decimal_point()),
  thousands_sep(this->punct.thousands_sep()),
  oct_leadin(this->ctype.widen('0')),
  hex_leadin_lower(this->ctype.widen('x')),
  hex_leadin_upper(this->ctype.widen('X')),
  sign_neg(this->ctype.widen('-')),
  sign_pos(this->ctype.widen('+')),
  grouping(this->punct.grouping())
{}

template<typename Char>
auto basic_num_decoder<Char>::validate_grouping() const noexcept -> bool {
  return !tsep_seen_ ||
         compare_grouping_measure_spec(tsep_, before_tsep_, grouping);
}


template<typename Char, typename T>
template<typename Iter>
num_decoder<Char, T>::num_decoder(Iter b, Iter e, Iter& next, ios_base& str)
: basic_num_decoder<Char>(str)
{
  using traits = char_traits<char_type>;

  /*
   * Enforce numeric base, if given.
   */
  unsigned_type base;
  switch (str.flags() & ios_base::basefield) {
  default:
    base = 0;  // Unknown base.
    break;
  case ios_base::dec:
    base = 10;
    break;
  case ios_base::oct:
    base = 8;
    break;
  case ios_base::hex:
    base = 16;
    break;
  }
  assert(this->digits_lower.size() >= base);
  assert(this->digits_upper.size() >= base);

  /* Initialize digits lists, using current base.
   * Note that base == 0 will be fixed up in the loop below. */
  auto digits_lower = basic_string_ref<char_type>(this->digits_lower.data(),
                                                  base);
  auto digits_upper = basic_string_ref<char_type>(this->digits_upper.data(),
                                                  base);

  /*
   * Parse, using a loop to consume characters.
   */
  bool sign_seen = false;
  bool maybe_oct = false;
  streamsize ndigits_since_tsep = 0;
  for (next = b; !(eof_ = (next == e)); ++next) {
    const auto c = *next;

    /* Handle sign detection. */
    if (!sign_seen) {
      if (traits::eq(c, this->sign_neg)) {
        sign_seen = true;
        negative_ = true;
        continue;  // Character consumed.
      } else if (traits::eq(c, this->sign_pos)) {
        sign_seen = true;
        continue;  // Character consumed.
      } else {
        sign_seen = true;  // Absense -> positive.
        // Fall through: character was not consumed.
      }
    }

    /* Handle base detection. */
    if (base == 0) {
      bool do_continue;

      /* Try to detect the base. */
      if (traits::eq(c, this->oct_leadin)) {
        maybe_oct = true;
        saw_at_least_one_digit_ = true;  // Saw a zero.
        do_continue = true;  // Character consumed.
      } else if (maybe_oct &&
                 (traits::eq(c, this->hex_leadin_upper) ||
                  traits::eq(c, this->hex_leadin_lower))) {
        base = 16;
        saw_at_least_one_digit_ = false;  // Undo above.
        do_continue = true;  // Character consumed.
      } else {
        base = (maybe_oct ? 8 : 10);
        do_continue = false;  // Fall through: character was not consumed.
      }

      /* Initialize detected base. */
      if (base != 0) {
        assert(this->digits_lower.size() >= base);
        assert(this->digits_upper.size() >= base);
        digits_lower = basic_string_ref<char_type>(this->digits_lower.data(),
                                                   base);
        digits_upper = basic_string_ref<char_type>(this->digits_upper.data(),
                                                   base);
      }

      /* If this is not a digit, skip detection. */
      if (do_continue) continue;
    }

    /* Try to treat c as a digit. */
    const auto dl_off = digits_lower.find(c);
    const auto du_off = digits_upper.find(c);
    if (dl_off != basic_string_ref<char_type>::npos ||
        du_off != basic_string_ref<char_type>::npos) {
      unsigned_type digit;
      if (dl_off != basic_string_ref<char_type>::npos)
        digit = dl_off;
      else
        digit = du_off;

      /* Multiply and add, taking care to detect overflow. */
      unsigned_type tmp = unsigned_result_ * base;
      overflow_ |= (tmp < unsigned_result_);  // XXX: use <abi/misc_int.h>
      tmp += digit;
      overflow_ |= (tmp < unsigned_result_);  // XXX: use <abi/misc_int.h>

      /* Record that we consumed a digit. */
      ++ndigits_since_tsep;
      saw_at_least_one_digit_ = true;

      continue;  // Character consumed.
    }

    /* Try to treat c as a thousands separator. */
    if (!this->grouping.empty() &&
        traits::eq(c, this->thousands_sep) &&
        ndigits_since_tsep > 0) {
      if (this->tsep_seen_)
        this->tsep_.push_back(static_cast<char>(ndigits_since_tsep));
      else
        this->before_tsep_ = ndigits_since_tsep;
      this->tsep_seen_ = true;
      ndigits_since_tsep = 0;
      continue;  // Character consumed.
    }

    /*
     * Not a character we expect.
     */
    break;  // GUARD
  }

  /* Add last set of thousand separators. */
  if (this->tsep_seen_)
    this->tsep_.push_back(static_cast<char>(ndigits_since_tsep));
  else
    this->before_tsep_ = ndigits_since_tsep;

  /* Set overflow if the unsigned value exceeds the possible result. */
  if (overflow_) {
    /* SKIP */
  } else if (!negative_) {
    if (unsigned_result_ > numeric_limits<T>::max())
      overflow_ = true;
  } else if (numeric_limits<T>::is_signed) {
    if (unsigned_result_ > this->abs(numeric_limits<T>::min()))
      overflow_ = true;
  } else {
    /* Negative number for non-negative value. */
    overflow_ = true;
  }
}

template<typename Char, typename T>
auto num_decoder<Char, T>::get() const -> value_type {
  /* In the case of overflow, return the closest number. */
  if (overflow_) return (negative_ ?
                         numeric_limits<value_type>::min() :
                         numeric_limits<value_type>::max());

  /* If negative, negate the unsigned result value.
   * Note that abs(INT_MIN) is not representable in signed numbers,
   * so we do a small trick:
   *   -x  ==  1 - x - 1  ==  -(x - 1) - 1
   * This allows us to correctly convert INT_MIN (and other *_MIN values).
   */
  value_type rv;
  if (negative_)
    rv = -static_cast<value_type>(unsigned_result_ - 1U) - 1;
  else
    rv = static_cast<value_type>(unsigned_result_);

  assert(this->abs(rv) == unsigned_result_);  // Paranoid about math/overflow.
  return rv;
}

template<typename Char, typename T>
auto num_decoder<Char, T>::saw_digits() const noexcept -> bool {
  return saw_at_least_one_digit_;
}

template<typename Char, typename T>
auto num_decoder<Char, T>::overflow() const noexcept -> bool {
  return overflow_;
}

template<typename Char, typename T>
auto num_decoder<Char, T>::eof() const noexcept -> bool {
  return eof_;
}

template<typename Char, typename T>
auto num_decoder<Char, T>::valid() const noexcept -> bool {
  return !overflow() && saw_digits() && this->validate_grouping();
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


extern template class basic_num_encoder<char>;
extern template class basic_num_encoder<char16_t>;
extern template class basic_num_encoder<char32_t>;
extern template class basic_num_encoder<wchar_t>;

extern template class basic_num_decoder<char>;
extern template class basic_num_decoder<char16_t>;
extern template class basic_num_decoder<char32_t>;
extern template class basic_num_decoder<wchar_t>;


} /* namespace std::impl */
_namespace_end(std)

#endif /* _STDIMPL_LOCALE_NUMERIC_INL_H_ */
