#ifndef _IMPL_CHAR_TRAITS_H_
#define _IMPL_CHAR_TRAITS_H_

_namespace_begin(std)
namespace impl {

/*
 * char_traits::rfind wrapper:
 * if char_traits::rfind is defined, this function will invoke it.
 * Otherwise, SFINAE will ignore this function and fall back to the
 * implementation below.
 */
template<typename CharT, typename FN = decltype(CharT::rfind)>
auto rfind(const typename CharT::char_type* s, size_t len,
           const typename CharT::char_type& c)
    noexcept(noexcept(CharT::rfind(s, len, c))) ->
    const typename CharT::char_type* {
  return CharT::rfind(s, len, c);
}

template<typename CharT>
auto rfind(const typename CharT::char_type* s, size_t len,
           const typename CharT::char_type& c, ...)
    noexcept ->
    const typename CharT::char_type* {
  s += len;
  while (len-- > 0) {
    --s;
    if (CharT::eq(*s, c)) return s;
  }
  return nullptr;
}


/*
 * char_traits::strfind wrapper:
 * if char_traits::strfind is defined, this function will invoke it.
 * Otherwise, SFINAE will ignore this function and fall back to the
 * implementation below.
 */
template<typename CharT, typename FN = decltype(CharT::strfind)>
auto strfind(const typename CharT::char_type* h, size_t hlen,
             const typename CharT::char_type* n, size_t nlen)
    noexcept(noexcept(CharT::strfind(h, hlen, n, nlen))) ->
    const typename CharT::char_type* {
  return CharT::strfind(h, hlen, n, nlen);
}

template<typename CharT>
auto strfind(const typename CharT::char_type* h, size_t hlen,
             const typename CharT::char_type* n, size_t nlen, ...)
    noexcept ->
    const typename CharT::char_type* {
  using char_type = const typename CharT::char_type;

  if (nlen > hlen) return nullptr;
  for (char_type* i = h; i + nlen != h + hlen; ++i)
    if (CharT::compare(i, n, nlen) == 0) return i;
  return nullptr;
}


/*
 * char_traits::strrfind wrapper:
 * if char_traits::strrfind is defined, this function will invoke it.
 * Otherwise, SFINAE will ignore this function and fall back to the
 * implementation below.
 */
template<typename CharT, typename FN = decltype(CharT::strfind)>
auto strrfind(const typename CharT::char_type* h, size_t hlen,
             const typename CharT::char_type* n, size_t nlen)
    noexcept(noexcept(CharT::strfind(h, hlen, n, nlen))) ->
    const typename CharT::char_type* {
  return CharT::strrfind(h, hlen, n, nlen);
}

template<typename CharT>
auto strrfind(const typename CharT::char_type* h, size_t hlen,
             const typename CharT::char_type* n, size_t nlen, ...)
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


/*
 * char_traits::strcspn wrapper:
 * if char_traits::strcspn is defined, this function will invoke it.
 * Otherwise, SFINAE will ignore this function and fall back to the
 * implementation below.
 */
template<typename CharT, typename FN = decltype(CharT::strcspn)>
auto strcspn(const typename CharT::char_type* h, size_t hlen,
             const typename CharT::char_type* n, size_t nlen)
    noexcept(noexcept(CharT::strcspn(h, hlen, n, nlen))) ->
    const typename CharT::char_type* {
  return CharT::strcspn(h, hlen, n, nlen);
}

template<typename CharT>
auto strcspn(const typename CharT::char_type* h, size_t hlen,
             const typename CharT::char_type* n, size_t nlen, ...)
    noexcept ->
    const typename CharT::char_type* {
  const typename CharT::char_type* hi = h + hlen;
  while (hlen-- > 0) {
    if (CharT::find(n, nlen, *h) == nullptr) return h;
    ++h;
  }
  return nullptr;
}


/*
 * char_traits::strrcspn wrapper:
 * if char_traits::strrcspn is defined, this function will invoke it.
 * Otherwise, SFINAE will ignore this function and fall back to the
 * implementation below.
 */
template<typename CharT, typename FN = decltype(CharT::strrcspn)>
auto strrcspn(const typename CharT::char_type* h, size_t hlen,
              const typename CharT::char_type* n, size_t nlen)
    noexcept(noexcept(CharT::strrcspn(h, hlen, n, nlen))) ->
    const typename CharT::char_type* {
  return CharT::strrcspn(h, hlen, n, nlen);
}

template<typename CharT>
auto strrcspn(const typename CharT::char_type* h, size_t hlen,
              const typename CharT::char_type* n, size_t nlen, ...)
    noexcept ->
    const typename CharT::char_type* {
  const typename CharT::char_type* hi = h + hlen;
  while (hi-- != h)
    if (CharT::find(n, nlen, *hi) == nullptr) return hi;
  return nullptr;
}


/*
 * char_traits::strspn wrapper:
 * if char_traits::strspn is defined, this function will invoke it.
 * Otherwise, SFINAE will ignore this function and fall back to the
 * implementation below.
 */
template<typename CharT, typename FN = decltype(CharT::strspn)>
auto strspn(const typename CharT::char_type* h, size_t hlen,
             const typename CharT::char_type* n, size_t nlen)
    noexcept(noexcept(CharT::strspn(h, hlen, n, nlen))) ->
    const typename CharT::char_type* {
  return CharT::strspn(h, hlen, n, nlen);
}

template<typename CharT>
auto strspn(const typename CharT::char_type* h, size_t hlen,
             const typename CharT::char_type* n, size_t nlen, ...)
    noexcept ->
    const typename CharT::char_type* {
  const typename CharT::char_type* hi = h + hlen;
  while (hlen-- > 0) {
    if (CharT::find(n, nlen, *h) != nullptr) return h;
    ++h;
  }
  return nullptr;
}


/*
 * char_traits::strrspn wrapper:
 * if char_traits::strrspn is defined, this function will invoke it.
 * Otherwise, SFINAE will ignore this function and fall back to the
 * implementation below.
 */
template<typename CharT, typename FN = decltype(CharT::strrspn)>
auto strrspn(const typename CharT::char_type* h, size_t hlen,
              const typename CharT::char_type* n, size_t nlen)
    noexcept(noexcept(CharT::strrspn(h, hlen, n, nlen))) ->
    const typename CharT::char_type* {
  return CharT::strrspn(h, hlen, n, nlen);
}

template<typename CharT>
auto strrspn(const typename CharT::char_type* h, size_t hlen,
              const typename CharT::char_type* n, size_t nlen, ...)
    noexcept ->
    const typename CharT::char_type* {
  const typename CharT::char_type* hi = h + hlen;
  while (hi-- != h)
    if (CharT::find(n, nlen, *hi) != nullptr) return hi;
  return nullptr;
}


/*
 * char_traits::cspn wrapper:
 * if char_traits::cspn is defined, this function will invoke it.
 * Otherwise, SFINAE will ignore this function and fall back to the
 * implementation below.
 */
template<typename CharT, typename FN = decltype(CharT::cspn)>
auto cspn(const typename CharT::char_type* s, size_t len,
          const typename CharT::char_type& c)
    noexcept(noexcept(CharT::cspn(s, len, c))) ->
    const typename CharT::char_type* {
  return CharT::spn(s, len, c);
}

template<typename CharT>
auto spn(const typename CharT::char_type* s, size_t len,
         const typename CharT::char_type& c, ...)
    noexcept ->
    const typename CharT::char_type* {
  while (len-- > 0) {
    if (!CharT::eq(*s, c)) return s;
    ++s;
  }
  return nullptr;
}

/*
 * char_traits::rcspn wrapper:
 * if char_traits::rcspn is defined, this function will invoke it.
 * Otherwise, SFINAE will ignore this function and fall back to the
 * implementation below.
 */
template<typename CharT, typename FN = decltype(CharT::rcspn)>
auto rspn(const typename CharT::char_type* s, size_t len,
          const typename CharT::char_type& c)
    noexcept(noexcept(CharT::rcspn(s, len, c))) ->
    const typename CharT::char_type* {
  return CharT::rspn(s, len, c);
}

template<typename CharT>
auto rspn(const typename CharT::char_type* s, size_t len,
          const typename CharT::char_type& c, ...)
    noexcept ->
    const typename CharT::char_type* {
  s += len;
  while (len-- > 0) {
    --s;
    if (!CharT::eq(*s, c)) return s;
  }
  return nullptr;
}


} /* namespace std::impl */
_namespace_end(std)

#endif /* _IMPL_CHAR_TRAITS_H_ */
