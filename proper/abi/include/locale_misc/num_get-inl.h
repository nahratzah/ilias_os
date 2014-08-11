#ifndef _LOCALE_MISC_NUM_GET_INL_H_
#define _LOCALE_MISC_NUM_GET_INL_H_

#include <locale_misc/num_get.h>
#include <locale_misc/numpunct.h>
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


_namespace_end(std)

#endif /* _LOCALE_MISC_NUM_GET_INL_H_ */
