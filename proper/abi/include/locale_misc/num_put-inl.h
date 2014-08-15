#ifndef _LOCALE_MISC_NUM_PUT_INL_H_
#define _LOCALE_MISC_NUM_PUT_INL_H_

#include <cdecl.h>
#include <locale_misc/locale.h>
#include <locale_misc/numpunct.h>
#include <stdimpl/locale_numeric.h>
#include <ios>

_namespace_begin(std)


template<typename Char, typename Iter>
num_put<Char, Iter>::num_put(size_t refs)
: locale::facet(refs)
{}

template<typename Char, typename Iter>
auto num_put<Char, Iter>::put(iter_type out, ios_base& str, char_type fill,
                              bool v) const -> iter_type {
  return do_put(out, str, fill, v);
}

template<typename Char, typename Iter>
auto num_put<Char, Iter>::put(iter_type out, ios_base& str, char_type fill,
                              long v) const -> iter_type {
  return do_put(out, str, fill, v);
}

template<typename Char, typename Iter>
auto num_put<Char, Iter>::put(iter_type out, ios_base& str, char_type fill,
                              long long v) const -> iter_type {
  return do_put(out, str, fill, v);
}

template<typename Char, typename Iter>
auto num_put<Char, Iter>::put(iter_type out, ios_base& str, char_type fill,
                              unsigned long v) const -> iter_type {
  return do_put(out, str, fill, v);
}

template<typename Char, typename Iter>
auto num_put<Char, Iter>::put(iter_type out, ios_base& str, char_type fill,
                              unsigned long long v) const -> iter_type {
  return do_put(out, str, fill, v);
}

#if _USE_INT128
template<typename Char, typename Iter>
auto num_put<Char, Iter>::put(iter_type out, ios_base& str, char_type fill,
                              int128_t v) const -> iter_type {
  return do_put(out, str, fill, v);
}

template<typename Char, typename Iter>
auto num_put<Char, Iter>::put(iter_type out, ios_base& str, char_type fill,
                              uint128_t v) const -> iter_type {
  return do_put(out, str, fill, v);
}
#endif

template<typename Char, typename Iter>
auto num_put<Char, Iter>::put(iter_type out, ios_base& str, char_type fill,
                              double v) const -> iter_type {
  return do_put(out, str, fill, v);
}

template<typename Char, typename Iter>
auto num_put<Char, Iter>::put(iter_type out, ios_base& str, char_type fill,
                              long double v) const -> iter_type {
  return do_put(out, str, fill, v);
}

template<typename Char, typename Iter>
auto num_put<Char, Iter>::put(iter_type out, ios_base& str, char_type fill,
                              const void* v) const -> iter_type {
  return do_put(out, str, fill, v);
}

template<typename Char, typename Iter>
num_put<Char, Iter>::~num_put() noexcept {}

template<typename Char, typename Iter>
auto num_put<Char, Iter>::do_put(iter_type out, ios_base& str, char_type fill,
                                 bool v) const -> iter_type {
  using impl::render_num_encoder;

  if ((str.flags() & ios_base::boolalpha) != ios_base::boolalpha)
    return do_put(out, str, fill, static_cast<long>(v));

  const numpunct<char_type>& punct =
      use_facet<numpunct<char_type>>(str.getloc());
  const auto vname = (v ? punct.truename() : punct.falsename());
  return render_num_encoder(out, str,
                            basic_string_ref<char_type>(),
                            basic_string_ref<char_type>(vname),
                            fill, false);
}

template<typename Char, typename Iter>
auto num_put<Char, Iter>::do_put(iter_type out, ios_base& str, char_type fill,
                                 long v) const -> iter_type {
  using impl::render_num_encoder;
  using impl::num_encoder;

  return render_num_encoder(out, str,
                            num_encoder<char_type, long>(v, str),
                            fill);
}

template<typename Char, typename Iter>
auto num_put<Char, Iter>::do_put(iter_type out, ios_base& str, char_type fill,
                                 long long v) const -> iter_type {
  using impl::render_num_encoder;
  using impl::num_encoder;

  return render_num_encoder(out, str,
                            num_encoder<char_type, long long>(v, str),
                            fill);
}

template<typename Char, typename Iter>
auto num_put<Char, Iter>::do_put(iter_type out, ios_base& str, char_type fill,
                                 unsigned long v) const -> iter_type {
  using impl::render_num_encoder;
  using impl::num_encoder;

  return render_num_encoder(out, str,
                            num_encoder<char_type, unsigned long>(v, str),
                            fill);
}

template<typename Char, typename Iter>
auto num_put<Char, Iter>::do_put(iter_type out, ios_base& str, char_type fill,
                                 unsigned long long v) const -> iter_type {
  using impl::render_num_encoder;
  using impl::num_encoder;

  return render_num_encoder(out, str,
                            num_encoder<char_type, unsigned long long>(v, str),
                            fill);
}

#if _USE_INT128
template<typename Char, typename Iter>
auto num_put<Char, Iter>::do_put(iter_type out, ios_base& str, char_type fill,
                                 int128_t v) const -> iter_type {
  using impl::render_num_encoder;
  using impl::num_encoder;

  return render_num_encoder(out, str,
                            num_encoder<char_type, int128_t>(v, str),
                            fill);
}

template<typename Char, typename Iter>
auto num_put<Char, Iter>::do_put(iter_type out, ios_base& str, char_type fill,
                                 uint128_t v) const -> iter_type {
  using impl::render_num_encoder;
  using impl::num_encoder;

  return render_num_encoder(out, str,
                            num_encoder<char_type, uint128_t>(v, str),
                            fill);
}
#endif

template<typename Char, typename Iter>
auto num_put<Char, Iter>::do_put(iter_type out, ios_base& /*str*/, char_type /*fill*/,
                                 double /*v*/) const -> iter_type {
  assert_msg(false, "XXX implement");  // XXX implement
  return out;
}

template<typename Char, typename Iter>
auto num_put<Char, Iter>::do_put(iter_type out, ios_base& /*str*/, char_type /*fill*/,
                                 long double /*v*/) const -> iter_type {
  assert_msg(false, "XXX implement");  // XXX implement
  return out;
}

template<typename Char, typename Iter>
auto num_put<Char, Iter>::do_put(iter_type out, ios_base& str, char_type fill,
                                 const void* v) const -> iter_type {
  constexpr auto nhexdigits = numeric_limits<uintptr_t>::digits / 4U;
  constexpr Char xdigits[16] = { '0', '1', '2', '3', '4', '5', '6', '7',
                                 '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
  using data_t = char_type[nhexdigits + 2];

  uintptr_t pv = reinterpret_cast<uintptr_t>(v);
  data_t data;
  decltype(begin(data)) i;
  for (i = begin(data);
       (pv != 0 || i == begin(data)) && i != begin(data) + nhexdigits;
       ++i, pv /= 16)
    *i = xdigits[pv % 16];

  *i++ = 'x';
  *i++ = '0';
  reverse(begin(data), i);

  const auto data_str = basic_string_ref<char_type>(begin(data),
                                                    i - begin(data));
  const auto prefix = data_str.substr(0, 2);
  const auto digits = data_str.substr(2);
  return impl::render_num_encoder(out, str,
                                  prefix, digits, fill, false);
}


_namespace_end(std)

#endif /* _LOCALE_MISC_NUM_PUT_INL_H_ */
