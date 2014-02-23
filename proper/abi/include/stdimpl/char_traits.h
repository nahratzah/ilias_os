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
auto rfind(const typename CharT::char_type, CharT>* s, size_t len,
           const typename CharT::char_type& c)
    noexcept(noexcept(CharT::rfind(s, len, c))) ->
    const typename CharT::char_type* {
  return CharT::rfind(s, len, c);
}

template<typename CharT>
auto rfind(basic_string_ref<typename CharT::char_type, CharT> s,
           const typename CharT::char_type& c, ...)
    noexcept ->
    const typename CharT::char_type* {
  for (auto i = s.rbegin(); i != s.rend(); ++i)
    if (CharT::eq(*i, c)) return &*i;
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


} /* namespace std::impl */
_namespace_end(std)

#endif /* _IMPL_CHAR_TRAITS_H_ */
