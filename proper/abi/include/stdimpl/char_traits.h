#ifndef _IMPL_CHAR_TRAITS_H_
#define _IMPL_CHAR_TRAITS_H_

_namespace_begin(std)
namespace impl {

struct traits_tester {
 public:
  struct yes {};
  struct no {};

 private:
  /* Test if rfind exists. */
  template<typename CharT, typename RV>
  static auto has_rfind_test(
      const CharT&, RV = decltype(
        CharT::rfind(declval<const typename CharT::char_type*>(),
                     declval<size_t>(),
                     declval<const typename CharT::char_type&>()),
        yes())()) -> RV;
  template<typename CharT>
  static auto has_rfind_test(const CharT&, ...) -> no;

  /* Test if strfind exists. */
  template<typename CharT, typename RV>
  static auto has_strfind_test(
      const CharT&, RV = decltype(
        CharT::strfind(declval<const typename CharT::char_type*>(),
                       declval<size_t>(),
                       declval<const typename CharT::char_type*>(),
                       declval<size_t>()),
        yes())()) -> RV;
  template<typename CharT>
  static auto has_strfind_test(const CharT&, ...) -> no;

  /* Test if strrfind exists. */
  template<typename CharT, typename RV>
  static auto has_strrfind_test(
      const CharT&, RV = decltype(
        CharT::strrfind(declval<const typename CharT::char_type*>(),
                        declval<size_t>(),
                        declval<const typename CharT::char_type*>(),
                        declval<size_t>()),
        yes())()) -> RV;
  template<typename CharT>
  static auto has_strrfind_test(const CharT&, ...) -> no;

  /* Test if strspn exists. */
  template<typename CharT, typename RV>
  static auto has_strspn_test(
      const CharT&, RV = decltype(
        CharT::strspn(declval<const typename CharT::char_type*>(),
                      declval<size_t>(),
                      declval<const typename CharT::char_type*>(),
                      declval<size_t>()),
        yes())()) -> RV;
  template<typename CharT>
  static auto has_strspn_test(const CharT&, ...) -> no;

  /* Test if strrspn exists. */
  template<typename CharT, typename RV>
  static auto has_strrspn_test(
      const CharT&, RV = decltype(
        CharT::strrspn(declval<const typename CharT::char_type*>(),
                       declval<size_t>(),
                       declval<const typename CharT::char_type*>(),
                       declval<size_t>()),
        yes())()) -> RV;
  template<typename CharT>
  static auto has_strrspn_test(const CharT&, ...) -> no;

  /* Test if strcspn exists. */
  template<typename CharT, typename RV>
  static auto has_strcspn_test(
      const CharT&, RV = decltype(
        CharT::strcspn(declval<const typename CharT::char_type*>(),
                       declval<size_t>(),
                       declval<const typename CharT::char_type*>(),
                       declval<size_t>()),
        yes())()) -> RV;
  template<typename CharT>
  static auto has_strcspn_test(const CharT&, ...) -> no;

  /* Test if strrcspn exists. */
  template<typename CharT, typename RV>
  static auto has_strrcspn_test(
      const CharT&, RV = decltype(
        CharT::strrcspn(declval<const typename CharT::char_type*>(),
                        declval<size_t>(),
                        declval<const typename CharT::char_type*>(),
                        declval<size_t>()),
        yes())()) -> RV;
  template<typename CharT>
  static auto has_strrcspn_test(const CharT&, ...) -> no;

  /* Test if spn exists. */
  template<typename CharT, typename RV>
  static auto has_spn_test(
      const CharT&, RV = decltype(
        CharT::spn(declval<const typename CharT::char_type*>(),
                   declval<size_t>(),
                   declval<const typename CharT::char_type&>()),
        yes())()) -> RV;
  template<typename CharT>
  static auto has_spn_test(const CharT&, ...) -> no;

  /* Test if rspn exists. */
  template<typename CharT, typename RV>
  static auto has_rspn_test(
      const CharT&, RV = decltype(
        CharT::rspn(declval<const typename CharT::char_type*>(),
                    declval<size_t>(),
                    declval<const typename CharT::char_type&>()),
        yes())()) -> RV;
  template<typename CharT>
  static auto has_rspn_test(const CharT&, ...) -> no;

 public:
  template<typename CharT> using has_rfind =
      decltype(has_rfind_test(declval<const CharT&>()));
  template<typename CharT> using has_strfind =
      decltype(has_strfind_test(declval<const CharT&>()));
  template<typename CharT> using has_strrfind =
      decltype(has_strrfind_test(declval<const CharT&>()));
  template<typename CharT> using has_strspn =
      decltype(has_strspn_test(declval<const CharT&>()));
  template<typename CharT> using has_strrspn =
      decltype(has_strrspn_test(declval<const CharT&>()));
  template<typename CharT> using has_strcspn =
      decltype(has_strcspn_test(declval<const CharT&>()));
  template<typename CharT> using has_strrcspn =
      decltype(has_strrcspn_test(declval<const CharT&>()));
  template<typename CharT> using has_spn =
      decltype(has_spn_test(declval<const CharT&>()));
  template<typename CharT> using has_rspn =
      decltype(has_rspn_test(declval<const CharT&>()));

 private:
  /* rfind selector implementation. */
  template<typename CharT>
  static auto rfind(const typename CharT::char_type* s, size_t len,
                    const typename CharT::char_type& c, yes)
      noexcept(noexcept(CharT::rfind(s, len, c))) ->
      const typename CharT::char_type* {
    return CharT::rfind(s, len, c);
  }

  template<typename CharT>
  static auto rfind(const typename CharT::char_type* s, size_t len,
                    const typename CharT::char_type& c, no)
      noexcept ->
      const typename CharT::char_type* {
    s += len;
    while (len-- > 0) {
      --s;
      if (CharT::eq(*s, c)) return s;
    }
    return nullptr;
  }

  /* strfind selector implementation. */
  template<typename CharT>
  static auto strfind(const typename CharT::char_type* h,
                      size_t hlen,
                      const typename CharT::char_type* n,
                      size_t nlen,
                      yes)
      noexcept(noexcept(CharT::strfind(h, hlen, n, nlen))) ->
      const typename CharT::char_type* {
    return CharT::strfind(h, hlen, n, nlen);
  }

  template<typename CharT>
  static auto strfind(const typename CharT::char_type* h,
                      size_t hlen,
                      const typename CharT::char_type* n,
                      size_t nlen,
                      no)
      noexcept ->
      const typename CharT::char_type* {
    using char_type = const typename CharT::char_type;

    if (nlen > hlen) return nullptr;
    for (char_type* i = h; i + nlen != h + hlen; ++i)
      if (CharT::compare(i, n, nlen) == 0) return i;
    return nullptr;
  }

  /* strrfind selector implementation. */
  template<typename CharT>
  static auto strrfind(const typename CharT::char_type* h,
                       size_t hlen,
                       const typename CharT::char_type* n,
                       size_t nlen,
                       yes)
      noexcept(noexcept(CharT::strrfind(h, hlen, n, nlen))) ->
      const typename CharT::char_type* {
    return CharT::strrfind(h, hlen, n, nlen);
  }

  template<typename CharT>
  static auto strrfind(const typename CharT::char_type* h,
                       size_t hlen,
                       const typename CharT::char_type* n,
                       size_t nlen,
                       no)
      noexcept ->
      const typename CharT::char_type* {
    using char_type = const typename CharT::char_type;

    if (nlen > hlen) return nullptr;
    char_type* i = h + (hlen - nlen);
    do {
      if (CharT::compare(i, n, nlen) == 0) return i;
    } while (i-- != h);
    return nullptr;
  }

  /* strspn selector implementation. */
  template<typename CharT>
  static auto strspn(const typename CharT::char_type* h,
                     size_t hlen,
                     const typename CharT::char_type* n,
                     size_t nlen,
                     yes)
      noexcept(noexcept(CharT::strspn(h, hlen, n, nlen))) ->
      const typename CharT::char_type* {
    return CharT::strspn(h, hlen, n, nlen);
  }

  template<typename CharT>
  static auto strspn(const typename CharT::char_type* h,
                     size_t hlen,
                     const typename CharT::char_type* n,
                     size_t nlen,
                     no)
      noexcept ->
      const typename CharT::char_type* {
    while (hlen-- > 0) {
      if (CharT::find(n, nlen, *h) != nullptr) return h;
      ++h;
    }
    return nullptr;
  }

  /* strrspn selector implementation. */
  template<typename CharT>
  static auto strrspn(const typename CharT::char_type* h,
                      size_t hlen,
                      const typename CharT::char_type* n,
                      size_t nlen,
                      yes)
      noexcept(noexcept(CharT::strrspn(h, hlen, n, nlen))) ->
      const typename CharT::char_type* {
    return CharT::strrspn(h, hlen, n, nlen);
  }

  template<typename CharT>
  static auto strrspn(const typename CharT::char_type* h,
                      size_t hlen,
                      const typename CharT::char_type* n,
                      size_t nlen,
                      no)
      noexcept ->
      const typename CharT::char_type* {
    const typename CharT::char_type* hi = h + hlen;
    while (hi-- != h)
      if (CharT::find(n, nlen, *hi) != nullptr) return hi;
    return nullptr;
  }

  /* strspn selector implementation. */
  template<typename CharT>
  static auto strcspn(const typename CharT::char_type* h,
                      size_t hlen,
                      const typename CharT::char_type* n,
                      size_t nlen,
                      yes)
      noexcept(noexcept(CharT::strcspn(h, hlen, n, nlen))) ->
      const typename CharT::char_type* {
    return CharT::strcspn(h, hlen, n, nlen);
  }

  template<typename CharT>
  static auto strcspn(const typename CharT::char_type* h,
                      size_t hlen,
                      const typename CharT::char_type* n,
                      size_t nlen,
                      no)
      noexcept ->
      const typename CharT::char_type* {
    while (hlen-- > 0) {
      if (CharT::find(n, nlen, *h) == nullptr) return h;
      ++h;
    }
    return nullptr;
  }

  /* strrspn selector implementation. */
  template<typename CharT>
  static auto strrcspn(const typename CharT::char_type* h,
                       size_t hlen,
                       const typename CharT::char_type* n,
                       size_t nlen,
                       yes)
      noexcept(noexcept(CharT::strrcspn(h, hlen, n, nlen))) ->
      const typename CharT::char_type* {
    return CharT::strrcspn(h, hlen, n, nlen);
  }

  template<typename CharT>
  static auto strrcspn(const typename CharT::char_type* h,
                       size_t hlen,
                       const typename CharT::char_type* n,
                       size_t nlen,
                       no)
      noexcept ->
      const typename CharT::char_type* {
    const typename CharT::char_type* hi = h + hlen;
    while (hi-- != h)
      if (CharT::find(n, nlen, *hi) == nullptr) return hi;
    return nullptr;
  }

  /* spn selector implementation. */
  template<typename CharT>
  static auto spn(const typename CharT::char_type* s, size_t len,
                  const typename CharT::char_type& c, yes)
      noexcept(noexcept(CharT::spn(s, len, c))) ->
      const typename CharT::char_type* {
    return CharT::spn(s, len, c);
  }

  template<typename CharT>
  static auto spn(const typename CharT::char_type* s, size_t len,
                  const typename CharT::char_type& c, no)
      noexcept ->
      const typename CharT::char_type* {
    while (len-- > 0) {
      if (!CharT::eq(*s, c)) return s;
      ++s;
    }
    return nullptr;
  }

  /* spn selector implementation. */
  template<typename CharT>
  static auto rspn(const typename CharT::char_type* s, size_t len,
                   const typename CharT::char_type& c, yes)
      noexcept(noexcept(CharT::rspn(s, len, c))) ->
      const typename CharT::char_type* {
    return CharT::rspn(s, len, c);
  }

  template<typename CharT>
  static auto rspn(const typename CharT::char_type* s, size_t len,
                   const typename CharT::char_type& c, no)
      noexcept ->
      const typename CharT::char_type* {
    s += len;
    while (len-- > 0) {
      --s;
      if (!CharT::eq(*s, c)) return s;
    }
    return nullptr;
  }

 public:
  /* rfind implementation. */
  template<typename CharT>
  static auto rfind(const typename CharT::char_type* s, size_t len,
                    const typename CharT::char_type& c)
      noexcept(noexcept(rfind<CharT>(s, len, c, has_rfind<CharT>()))) ->
      const typename CharT::char_type* {
    return rfind<CharT>(s, len, c, has_rfind<CharT>());
  }

  /* strfind implementation. */
  template<typename CharT>
  static auto strfind(const typename CharT::char_type* h,
                      size_t hlen,
                      const typename CharT::char_type* n,
                      size_t nlen)
      noexcept(noexcept(strfind<CharT>(h, hlen,
                                       n, nlen, has_strfind<CharT>()))) ->
      const typename CharT::char_type* {
    return strfind<CharT>(h, hlen, n, nlen, has_strfind<CharT>());
  }

  /* strrfind implementation. */
  template<typename CharT>
  static auto strrfind(const typename CharT::char_type* h,
                       size_t hlen,
                       const typename CharT::char_type* n,
                       size_t nlen)
      noexcept(noexcept(strrfind<CharT>(h, hlen,
                                        n, nlen, has_strfind<CharT>()))) ->
      const typename CharT::char_type* {
    return strrfind<CharT>(h, hlen, n, nlen, has_strfind<CharT>());
  }

  /* strspn implementation. */
  template<typename CharT>
  static auto strspn(const typename CharT::char_type* h,
                     size_t hlen,
                     const typename CharT::char_type* n,
                     size_t nlen)
      noexcept(noexcept(strspn<CharT>(h, hlen,
                                      n, nlen, has_strspn<CharT>()))) ->
      const typename CharT::char_type* {
    return strspn<CharT>(h, hlen, n, nlen, has_strspn<CharT>());
  }

  /* strrspn implementation. */
  template<typename CharT>
  static auto strrspn(const typename CharT::char_type* h,
                      size_t hlen,
                      const typename CharT::char_type* n,
                      size_t nlen)
      noexcept(noexcept(strrspn<CharT>(h, hlen,
                                       n, nlen, has_strrspn<CharT>()))) ->
      const typename CharT::char_type* {
    return strrspn<CharT>(h, hlen, n, nlen, has_strrspn<CharT>());
  }

  /* strcspn implementation. */
  template<typename CharT>
  static auto strcspn(const typename CharT::char_type* h,
                      size_t hlen,
                      const typename CharT::char_type* n,
                      size_t nlen)
      noexcept(noexcept(strcspn<CharT>(h, hlen,
                                       n, nlen, has_strcspn<CharT>()))) ->
      const typename CharT::char_type* {
    return strcspn<CharT>(h, hlen, n, nlen, has_strcspn<CharT>());
  }

  /* strrcspn implementation. */
  template<typename CharT>
  static auto strrcspn(const typename CharT::char_type* h,
                       size_t hlen,
                       const typename CharT::char_type* n,
                       size_t nlen)
      noexcept(noexcept(strrcspn<CharT>(h, hlen,
                                        n, nlen, has_strrcspn<CharT>()))) ->
      const typename CharT::char_type* {
    return strrcspn<CharT>(h, hlen, n, nlen, has_strrcspn<CharT>());
  }

  /* spn implementation. */
  template<typename CharT>
  static auto spn(const typename CharT::char_type* s, size_t len,
                  const typename CharT::char_type& c)
      noexcept(noexcept(spn<CharT>(s, len, c, has_spn<CharT>()))) ->
      const typename CharT::char_type* {
    return spn<CharT>(s, len, c, has_rfind<CharT>());
  }

  /* rspn implementation. */
  template<typename CharT>
  static auto rspn(const typename CharT::char_type* s, size_t len,
                   const typename CharT::char_type& c)
      noexcept(noexcept(rspn<CharT>(s, len, c, has_spn<CharT>()))) ->
      const typename CharT::char_type* {
    return rspn<CharT>(s, len, c, has_rfind<CharT>());
  }
};


template<typename CharT>
auto rfind(const typename CharT::char_type* s, size_t len,
           const typename CharT::char_type& c)
    noexcept(noexcept(traits_tester::rfind<CharT>(s, len, c))) ->
    const typename CharT::char_type* {
  return traits_tester::rfind<CharT>(s, len, c);
}

template<typename CharT>
auto strfind(const typename CharT::char_type* h, size_t hlen,
             const typename CharT::char_type* n, size_t nlen)
    noexcept(noexcept(traits_tester::strfind<CharT>(h, hlen, n, nlen))) ->
    const typename CharT::char_type* {
  return traits_tester::strfind<CharT>(h, hlen, n, nlen);
}

template<typename CharT>
auto strrfind(const typename CharT::char_type* h, size_t hlen,
              const typename CharT::char_type* n, size_t nlen)
    noexcept(noexcept(traits_tester::strrfind<CharT>(h, hlen, n, nlen))) ->
    const typename CharT::char_type* {
  return traits_tester::strrfind<CharT>(h, hlen, n, nlen);
}

template<typename CharT>
auto strspn(const typename CharT::char_type* h, size_t hlen,
            const typename CharT::char_type* n, size_t nlen)
    noexcept(noexcept(traits_tester::strspn<CharT>(h, hlen, n, nlen))) ->
    const typename CharT::char_type* {
  return traits_tester::strspn<CharT>(h, hlen, n, nlen);
}

template<typename CharT>
auto strrspn(const typename CharT::char_type* h, size_t hlen,
             const typename CharT::char_type* n, size_t nlen)
    noexcept(noexcept(traits_tester::strrspn<CharT>(h, hlen, n, nlen))) ->
    const typename CharT::char_type* {
  return traits_tester::strrspn<CharT>(h, hlen, n, nlen);
}

template<typename CharT>
auto strcspn(const typename CharT::char_type* h, size_t hlen,
             const typename CharT::char_type* n, size_t nlen)
    noexcept(noexcept(traits_tester::strcspn<CharT>(h, hlen, n, nlen))) ->
    const typename CharT::char_type* {
  return traits_tester::strcspn<CharT>(h, hlen, n, nlen);
}

template<typename CharT>
auto strrcspn(const typename CharT::char_type* h, size_t hlen,
              const typename CharT::char_type* n, size_t nlen)
    noexcept(noexcept(traits_tester::strrcspn<CharT>(h, hlen, n, nlen))) ->
    const typename CharT::char_type* {
  return traits_tester::strrcspn<CharT>(h, hlen, n, nlen);
}

template<typename CharT>
auto spn(const typename CharT::char_type* s, size_t len,
         const typename CharT::char_type& c)
    noexcept(noexcept(traits_tester::spn<CharT>(s, len, c))) ->
    const typename CharT::char_type* {
  return traits_tester::spn<CharT>(s, len, c);
}

template<typename CharT>
auto rspn(const typename CharT::char_type* s, size_t len,
          const typename CharT::char_type& c)
    noexcept(noexcept(traits_tester::rspn<CharT>(s, len, c))) ->
    const typename CharT::char_type* {
  return traits_tester::rspn<CharT>(s, len, c);
}


} /* namespace std::impl */
_namespace_end(std)

#endif /* _IMPL_CHAR_TRAITS_H_ */
