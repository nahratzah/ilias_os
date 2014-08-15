#ifndef _LOCALE_MISC_NUM_GET_INL_H_
#define _LOCALE_MISC_NUM_GET_INL_H_

#include <locale_misc/num_get.h>
#include <locale_misc/numpunct.h>
#include <stdimpl/locale_numeric.h>
#include <ios>
#include <string>

_namespace_begin(std)


template<typename Char, typename Iter>
num_get<Char, Iter>::num_get(size_t refs)
: locale::facet(refs)
{}

template<typename Char, typename Iter>
auto num_get<Char, Iter>::get(iter_type in, iter_type end,
                              ios_base& str, ios_base::iostate& err,
                              bool& v) const -> iter_type {
  return do_get(in, end, str, err, v);
}

template<typename Char, typename Iter>
auto num_get<Char, Iter>::get(iter_type in, iter_type end,
                              ios_base& str, ios_base::iostate& err,
                              long& v) const -> iter_type {
  return do_get(in, end, str, err, v);
}

template<typename Char, typename Iter>
auto num_get<Char, Iter>::get(iter_type in, iter_type end,
                              ios_base& str, ios_base::iostate& err,
                              long long& v) const -> iter_type {
  return do_get(in, end, str, err, v);
}

template<typename Char, typename Iter>
auto num_get<Char, Iter>::get(iter_type in, iter_type end,
                              ios_base& str, ios_base::iostate& err,
                              unsigned short& v) const -> iter_type {
  return do_get(in, end, str, err, v);
}

template<typename Char, typename Iter>
auto num_get<Char, Iter>::get(iter_type in, iter_type end,
                              ios_base& str, ios_base::iostate& err,
                              unsigned int& v) const -> iter_type {
  return do_get(in, end, str, err, v);
}

template<typename Char, typename Iter>
auto num_get<Char, Iter>::get(iter_type in, iter_type end,
                              ios_base& str, ios_base::iostate& err,
                              unsigned long& v) const -> iter_type {
  return do_get(in, end, str, err, v);
}

template<typename Char, typename Iter>
auto num_get<Char, Iter>::get(iter_type in, iter_type end,
                              ios_base& str, ios_base::iostate& err,
                              unsigned long long& v) const -> iter_type {
  return do_get(in, end, str, err, v);
}

#if _USE_INT128
template<typename Char, typename Iter>
auto num_get<Char, Iter>::get(iter_type in, iter_type end,
                              ios_base& str, ios_base::iostate& err,
                              int128_t& v) const -> iter_type {
  return do_get(in, end, str, err, v);
}

template<typename Char, typename Iter>
auto num_get<Char, Iter>::get(iter_type in, iter_type end,
                              ios_base& str, ios_base::iostate& err,
                              uint128_t& v) const -> iter_type {
  return do_get(in, end, str, err, v);
}
#endif

template<typename Char, typename Iter>
auto num_get<Char, Iter>::get(iter_type in, iter_type end,
                              ios_base& str, ios_base::iostate& err,
                              float& v) const -> iter_type {
  return do_get(in, end, str, err, v);
}

template<typename Char, typename Iter>
auto num_get<Char, Iter>::get(iter_type in, iter_type end,
                              ios_base& str, ios_base::iostate& err,
                              double& v) const -> iter_type {
  return do_get(in, end, str, err, v);
}

template<typename Char, typename Iter>
auto num_get<Char, Iter>::get(iter_type in, iter_type end,
                              ios_base& str, ios_base::iostate& err,
                              long double& v) const -> iter_type {
  return do_get(in, end, str, err, v);
}

template<typename Char, typename Iter>
auto num_get<Char, Iter>::get(iter_type in, iter_type end,
                              ios_base& str, ios_base::iostate& err,
                              void*& v) const -> iter_type {
  return do_get(in, end, str, err, v);
}

template<typename Char, typename Iter>
num_get<Char, Iter>::~num_get() noexcept {}

template<typename Char, typename Iter>
auto num_get<Char, Iter>::do_get(iter_type in, iter_type end,
                                 ios_base& str, ios_base::iostate& err,
                                 bool& v) const -> iter_type {
  using string_ref = basic_string_ref<char_type>;

  /* For numeric bool, use long conversion. */
  if ((str.flags() & ios_base::boolalpha) != ios_base::boolalpha) {
    long tmp = 0;
    in = get(in, end, str, err, tmp);
    v = (tmp != 0);
    if (tmp != 0 && tmp != 1) err = ios_base::failbit;
    return in;
  }

  const numpunct<char_type>& punct =
      use_facet<numpunct<char_type>>(str.getloc());

  /* Figure out the shortest true/false names. */
  const auto truename_store = punct.truename();
  const auto falsename_store = punct.falsename();
  string_ref truename = truename_store;
  string_ref falsename = falsename_store;
  using traits = typename string_ref::traits_type;
  string_ref choice;

  for (bool choice_made = false;
       !choice_made;
       truename = truename.substr(1), falsename = falsename.substr(1), ++in) {
    /* Handle truename == falsename, which is an illegal state (but may happen
     * if you provide a bad locale?)  */
    if (_predict_false(truename.empty() && falsename.empty())) {
      v = false;
      err = ios_base::failbit;
      return in;
    }

    /* Handle empty input. */
    if (in == end) {
      if (truename.empty() && !falsename.empty()) {
        v = true;
        err = ios_base::goodbit | ios_base::eofbit;
        return in;
      }
      if (falsename.empty() && !truename.empty()) {
        v = false;
        err = ios_base::goodbit | ios_base::eofbit;
        return in;
      }

      v = false;
      err = ios_base::failbit | ios_base::eofbit;
      return in;
    }

    /* Compare *in to truename.front(), falsename.front(). */
    const auto c = *in;
    if (traits::eq(truename.front(), falsename.front())) {
      if (!traits::eq(c, truename.front())) {
        v = false;
        err = ios_base::failbit;
        return in;
      }
    } else if (traits::eq(c, truename.front())) {
      v = true;
      choice = truename.substr(1);  // truename, after loop increment.
      choice_made = true;
    } else if (traits::eq(c, falsename.front())) {
      v = false;
      choice = falsename.substr(1);  // falsename, after loop increment.
      choice_made = true;
    } else {
      v = false;
      err = ios_base::failbit;
      return in;
    }
  }

  /* Consume remaining chars in choice. */
  err = ios_base::goodbit;
  while (!choice.empty()) {
    if (in == end) {
      err |= ios_base::eofbit;
      break;  // GUARD
    }
    const auto c = *in;
    if (!traits::eq(c, choice.front()))
      break;  // GUARD
  }

  return in;
}

template<typename Char, typename Iter>
auto num_get<Char, Iter>::do_get(iter_type in, iter_type end,
                                 ios_base& str, ios_base::iostate& err,
                                 long& v) const -> iter_type {
  using decoder_t = impl::num_decoder<Char, long>;

  auto decoder = decoder_t(in, end, in, str);
  v = decoder.get();
  err = (decoder.valid() ? ios_base::goodbit : ios_base::failbit);
  if (decoder.eof()) err |= ios_base::eofbit;
  return in;
}

template<typename Char, typename Iter>
auto num_get<Char, Iter>::do_get(iter_type in, iter_type end,
                                 ios_base& str, ios_base::iostate& err,
                                 long long& v) const -> iter_type {
  using decoder_t = impl::num_decoder<Char, long long>;

  auto decoder = decoder_t(in, end, in, str);
  v = decoder.get();
  err = (decoder.valid() ? ios_base::goodbit : ios_base::failbit);
  if (decoder.eof()) err |= ios_base::eofbit;
  return in;
}

template<typename Char, typename Iter>
auto num_get<Char, Iter>::do_get(iter_type in, iter_type end,
                                 ios_base& str, ios_base::iostate& err,
                                 unsigned short& v) const -> iter_type {
  using decoder_t = impl::num_decoder<Char, unsigned short>;

  auto decoder = decoder_t(in, end, in, str);
  v = decoder.get();
  err = (decoder.valid() ? ios_base::goodbit : ios_base::failbit);
  if (decoder.eof()) err |= ios_base::eofbit;
  return in;
}

template<typename Char, typename Iter>
auto num_get<Char, Iter>::do_get(iter_type in, iter_type end,
                                 ios_base& str, ios_base::iostate& err,
                                 unsigned int& v) const -> iter_type {
  using decoder_t = impl::num_decoder<Char, unsigned int>;

  auto decoder = decoder_t(in, end, in, str);
  v = decoder.get();
  err = (decoder.valid() ? ios_base::goodbit : ios_base::failbit);
  if (decoder.eof()) err |= ios_base::eofbit;
  return in;
}

template<typename Char, typename Iter>
auto num_get<Char, Iter>::do_get(iter_type in, iter_type end,
                                 ios_base& str, ios_base::iostate& err,
                                 unsigned long& v) const -> iter_type {
  using decoder_t = impl::num_decoder<Char, unsigned long>;

  auto decoder = decoder_t(in, end, in, str);
  v = decoder.get();
  err = (decoder.valid() ? ios_base::goodbit : ios_base::failbit);
  if (decoder.eof()) err |= ios_base::eofbit;
  return in;
}

template<typename Char, typename Iter>
auto num_get<Char, Iter>::do_get(iter_type in, iter_type end,
                                 ios_base& str, ios_base::iostate& err,
                                 unsigned long long& v) const -> iter_type {
  using decoder_t = impl::num_decoder<Char, unsigned long long>;

  auto decoder = decoder_t(in, end, in, str);
  v = decoder.get();
  err = (decoder.valid() ? ios_base::goodbit : ios_base::failbit);
  if (decoder.eof()) err |= ios_base::eofbit;
  return in;
}

#if _USE_INT128
template<typename Char, typename Iter>
auto num_get<Char, Iter>::do_get(iter_type in, iter_type end,
                                 ios_base& str, ios_base::iostate& err,
                                 int128_t& v) const -> iter_type {
  using decoder_t = impl::num_decoder<Char, int128_t>;

  auto decoder = decoder_t(in, end, in, str);
  v = decoder.get();
  err = (decoder.valid() ? ios_base::goodbit : ios_base::failbit);
  if (decoder.eof()) err |= ios_base::eofbit;
  return in;
}

template<typename Char, typename Iter>
auto num_get<Char, Iter>::do_get(iter_type in, iter_type end,
                                 ios_base& str, ios_base::iostate& err,
                                 uint128_t& v) const -> iter_type {
  using decoder_t = impl::num_decoder<Char, uint128_t>;

  auto decoder = decoder_t(in, end, in, str);
  v = decoder.get();
  err = (decoder.valid() ? ios_base::goodbit : ios_base::failbit);
  if (decoder.eof()) err |= ios_base::eofbit;
  return in;
}
#endif

template<typename Char, typename Iter>
auto num_get<Char, Iter>::do_get(iter_type in, iter_type /*end*/,
                                 ios_base& /*str*/, ios_base::iostate& /*err*/,
                                 float& /*v*/) const -> iter_type {
  assert_msg(false, "XXX implement");  // XXX implement
  return in;
}

template<typename Char, typename Iter>
auto num_get<Char, Iter>::do_get(iter_type in, iter_type /*end*/,
                                 ios_base& /*str*/, ios_base::iostate& /*err*/,
                                 double& /*v*/) const -> iter_type {
  assert_msg(false, "XXX implement");  // XXX implement
  return in;
}

template<typename Char, typename Iter>
auto num_get<Char, Iter>::do_get(iter_type in, iter_type /*end*/,
                                 ios_base& /*str*/, ios_base::iostate& /*err*/,
                                 long double& /*v*/) const -> iter_type {
  assert_msg(false, "XXX implement");  // XXX implement
  return in;
}

template<typename Char, typename Iter>
auto num_get<Char, Iter>::do_get(iter_type in, iter_type end,
                                 ios_base&, ios_base::iostate& err,
                                 void*& v) const -> iter_type {
  if (in == end || *in != '0') {
    v = nullptr;
    err = ios_base::failbit | ios_base::eofbit;
    return in;
  }
  ++in;

  if (in == end || *in != 'x') {
    v = nullptr;
    err = ios_base::failbit | ios_base::eofbit;
    return in;
  }
  ++in;

  uintptr_t ptr;
  bool saw_digits = false;
  err = ios_base::goodbit;
  for (ptr = 0; in != end; ++in, saw_digits = true) {
    const uintptr_t old_ptr = ptr;
    const auto c = *in;
    switch (c) {
    default:
      v = reinterpret_cast<void*>(ptr);
      if (!saw_digits) err = ios_base::badbit;
      return in;
    case '0':
      ptr <<= 4;
      ptr |= 0x0;
      break;
    case '1':
      ptr <<= 4;
      ptr |= 0x1;
      break;
    case '2':
      ptr <<= 4;
      ptr |= 0x2;
      break;
    case '3':
      ptr <<= 4;
      ptr |= 0x3;
      break;
    case '4':
      ptr <<= 4;
      ptr |= 0x4;
      break;
    case '5':
      ptr <<= 4;
      ptr |= 0x5;
      break;
    case '6':
      ptr <<= 4;
      ptr |= 0x6;
      break;
    case '7':
      ptr <<= 4;
      ptr |= 0x7;
      break;
    case '8':
      ptr <<= 4;
      ptr |= 0x8;
      break;
    case '9':
      ptr <<= 4;
      ptr |= 0x9;
      break;
    case 'a':
    case 'A':
      ptr <<= 4;
      ptr |= 0xa;
      break;
    case 'b':
    case 'B':
      ptr <<= 4;
      ptr |= 0xb;
      break;
    case 'c':
    case 'C':
      ptr <<= 4;
      ptr |= 0xc;
      break;
    case 'd':
    case 'D':
      ptr <<= 4;
      ptr |= 0xd;
      break;
    case 'e':
    case 'E':
      ptr <<= 4;
      ptr |= 0xe;
      break;
    case 'f':
    case 'F':
      ptr <<= 4;
      ptr |= 0xf;
      break;
    }

    if (old_ptr >> 4 != ptr)
      err = ios_base::badbit;  // Overflow.
  }

  err |= ios_base::eofbit;
  v = reinterpret_cast<void*>(ptr);
  return in;
}


_namespace_end(std)

#endif /* _LOCALE_MISC_NUM_GET_INL_H_ */
