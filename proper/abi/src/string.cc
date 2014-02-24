#include <string>
#include <abi/ext/reader.h>


_namespace_begin(std)


template<>
auto char_traits<char>::strfind(const char_type* haystack,
                                size_t n_haystack,
                                const char_type* needle,
                                size_type n_needle)
    noexcept -> const char_type* {
  return abi::ext::memfind(haystack, n_haystack, needle, n_needle);
}

template<>
auto char_traits<wchar_t>::strfind(const char_type* haystack,
                                   size_t n_haystack,
                                   const char_type* needle,
                                   size_type n_needle)
    noexcept -> const char_type* {
  return abi::ext::memfind(haystack, n_haystack, needle, n_needle);
}

template<>
auto char_traits<char16_t>::strfind(const char_type* haystack,
                                    size_t n_haystack,
                                    const char_type* needle,
                                    size_type n_needle)
    noexcept -> const char_type* {
  return abi::ext::memfind(haystack, n_haystack, needle, n_needle);
}

template<>
auto char_traits<char32_t>::strfind(const char_type* haystack,
                                    size_t n_haystack,
                                    const char_type* needle,
                                    size_type n_needle)
    noexcept -> const char_type* {
  return abi::ext::memfind(haystack, n_haystack, needle, n_needle);
}


template<>
auto char_traits<char>::strrfind(const char_type* haystack,
                                 size_t n_haystack,
                                 const char_type* needle,
                                 size_type n_needle)
    noexcept -> const char_type* {
  return abi::ext::memrfind(haystack, n_haystack, needle, n_needle);
}

template<>
auto char_traits<wchar_t>::strrfind(const char_type* haystack,
                                    size_t n_haystack,
                                    const char_type* needle,
                                    size_type n_needle)
    noexcept -> const char_type* {
  return abi::ext::memrfind(haystack, n_haystack, needle, n_needle);
}

template<>
auto char_traits<char16_t>::strrfind(const char_type* haystack,
                                     size_t n_haystack,
                                     const char_type* needle,
                                     size_type n_needle)
    noexcept -> const char_type* {
  return abi::ext::memrfind(haystack, n_haystack, needle, n_needle);
}

template<>
auto char_traits<char32_t>::strrfind(const char_type* haystack,
                                     size_t n_haystack,
                                     const char_type* needle,
                                     size_type n_needle)
    noexcept -> const char_type* {
  return abi::ext::memrfind(haystack, n_haystack, needle, n_needle);
}


template<>
auto char_traits<char>::spn(const char_type* s, size_t len, char_type c)
    noexcept -> const char_type* {
  using abi::ext::reader;
  using abi::ext::DIR_FORWARD;

  auto r = reader<DIR_FORWARD, char_type>(s, len);
  while (len-- > 0)
    if (!eq(r.read8(len + 1U), c)) return r.addr<const char_type>() - 1;
  return nullptr;
}

template<>
auto char_traits<wchar_t>::spn(const char_type* s, size_t len, char_type c)
    noexcept -> const char_type* {
  using abi::ext::reader;
  using abi::ext::DIR_FORWARD;

  auto r = reader<DIR_FORWARD, char_type>(s, len);
  while (len-- > 0)
    if (!eq(r.read8(len + 1U), c)) return r.addr<const char_type>() - 1;
  return nullptr;
}

template<>
auto char_traits<char16_t>::spn(const char_type* s, size_t len, char_type c)
    noexcept -> const char_type* {
  using abi::ext::reader;
  using abi::ext::DIR_FORWARD;

  auto r = reader<DIR_FORWARD, char_type>(s, len);
  while (len-- > 0)
    if (!eq(r.read8(len + 1U), c)) return r.addr<const char_type>() - 1;
  return nullptr;
}

template<>
auto char_traits<char32_t>::spn(const char_type* s, size_t len, char_type c)
    noexcept -> const char_type* {
  using abi::ext::reader;
  using abi::ext::DIR_FORWARD;

  auto r = reader<DIR_FORWARD, char_type>(s, len);
  while (len-- > 0)
    if (!eq(r.read8(len + 1U), c)) return r.addr<const char_type>() - 1;
  return nullptr;
}


template<>
auto char_traits<char>::rspn(const char_type* s, size_t len, char_type c)
    noexcept -> const char_type* {
  using abi::ext::reader;
  using abi::ext::DIR_BACKWARD;

  auto r = reader<DIR_BACKWARD, char_type>(s + len, len);
  while (len-- > 0)
    if (!eq(r.read8(len + 1U), c)) return r.addr<const char_type>();
  return nullptr;
}

template<>
auto char_traits<wchar_t>::rspn(const char_type* s, size_t len, char_type c)
    noexcept -> const char_type* {
  using abi::ext::reader;
  using abi::ext::DIR_BACKWARD;

  auto r = reader<DIR_BACKWARD, char_type>(s + len, len);
  while (len-- > 0)
    if (!eq(r.read8(len + 1U), c)) return r.addr<const char_type>();
  return nullptr;
}

template<>
auto char_traits<char16_t>::rspn(const char_type* s, size_t len, char_type c)
    noexcept -> const char_type* {
  using abi::ext::reader;
  using abi::ext::DIR_BACKWARD;

  auto r = reader<DIR_BACKWARD, char_type>(s + len, len);
  while (len-- > 0)
    if (!eq(r.read8(len + 1U), c)) return r.addr<const char_type>();
  return nullptr;
}

template<>
auto char_traits<char32_t>::rspn(const char_type* s, size_t len, char_type c)
    noexcept -> const char_type* {
  using abi::ext::reader;
  using abi::ext::DIR_BACKWARD;

  auto r = reader<DIR_BACKWARD, char_type>(s + len, len);
  while (len-- > 0)
    if (!eq(r.read8(len + 1U), c)) return r.addr<const char_type>();
  return nullptr;
}


template<>
auto char_traits<char>::strspn(const char_type* h, size_t hlen,
                               const char_type* n, size_t nlen)
    noexcept -> const char_type* {
  using abi::ext::reader;
  using abi::ext::DIR_FORWARD;

  auto r = reader<DIR_FORWARD, char_type>(s, len);
  while (len-- > 0) {
    if (find(n, nlen, r.read8(len + 1U)) == nullptr)
      return r.addr<const char_type*>() - 1;
  }
  return nullptr;
}

template<>
auto char_traits<wchar_t>::strspn(const char_type* h, size_t hlen,
                                  const char_type* n, size_t nlen)
    noexcept -> const char_type* {
  using abi::ext::reader;
  using abi::ext::DIR_FORWARD;

  auto r = reader<DIR_FORWARD, char_type>(s, len);
  while (len-- > 0) {
    if (find(n, nlen, r.read8(len + 1U)) == nullptr)
      return r.addr<const char_type*>() - 1;
  }
  return nullptr;
}

template<>
auto char_traits<char16_t>::strspn(const char_type* h, size_t hlen,
                                   const char_type* n, size_t nlen)
    noexcept -> const char_type* {
  using abi::ext::reader;
  using abi::ext::DIR_FORWARD;

  auto r = reader<DIR_FORWARD, char_type>(s, len);
  while (len-- > 0) {
    if (find(n, nlen, r.read8(len + 1U)) == nullptr)
      return r.addr<const char_type*>() - 1;
  }
  return nullptr;
}

template<>
auto char_traits<char32_t>::strspn(const char_type* h, size_t hlen,
                                   const char_type* n, size_t nlen)
    noexcept -> const char_type* {
  using abi::ext::reader;
  using abi::ext::DIR_FORWARD;

  auto r = reader<DIR_FORWARD, char_type>(s, len);
  while (len-- > 0) {
    if (find(n, nlen, r.read8(len + 1U)) == nullptr)
      return r.addr<const char_type*>() - 1;
  }
  return nullptr;
}


template<>
auto char_traits<char>::strrspn(const char_type* h, size_t hlen,
                                const char_type* n, size_t nlen)
    noexcept -> const char_type* {
  using abi::ext::reader;
  using abi::ext::DIR_BACKWARD;

  auto r = reader<DIR_BACKWARD, char_type>(s + len, len);
  while (len-- > 0) {
    if (find(n, nlen, r.read8(len + 1U)) == nullptr)
      return r.addr<const char_type*>();
  }
  return nullptr;
}

template<>
auto char_traits<wchar_t>::strrspn(const char_type* h, size_t hlen,
                                   const char_type* n, size_t nlen)
    noexcept -> const char_type* {
  using abi::ext::reader;
  using abi::ext::DIR_BACKWARD;

  auto r = reader<DIR_BACKWARD, char_type>(s + len, len);
  while (len-- > 0) {
    if (find(n, nlen, r.read8(len + 1U)) == nullptr)
      return r.addr<const char_type*>();
  }
  return nullptr;
}

template<>
auto char_traits<char16_t>::strrspn(const char_type* h, size_t hlen,
                                    const char_type* n, size_t nlen)
    noexcept -> const char_type* {
  using abi::ext::reader;
  using abi::ext::DIR_BACKWARD;

  auto r = reader<DIR_BACKWARD, char_type>(s + len, len);
  while (len-- > 0) {
    if (find(n, nlen, r.read8(len + 1U)) == nullptr)
      return r.addr<const char_type*>();
  }
  return nullptr;
}

template<>
auto char_traits<char32_t>::strrspn(const char_type* h, size_t hlen,
                                    const char_type* n, size_t nlen)
    noexcept -> const char_type* {
  using abi::ext::reader;
  using abi::ext::DIR_BACKWARD;

  auto r = reader<DIR_BACKWARD, char_type>(s + len, len);
  while (len-- > 0) {
    if (find(n, nlen, r.read8(len + 1U)) == nullptr)
      return r.addr<const char_type*>();
  }
  return nullptr;
}


template<>
auto char_traits<char>::strcspn(const char_type* h, size_t hlen,
                                const char_type* n, size_t nlen)
    noexcept -> const char_type* {
  using abi::ext::reader;
  using abi::ext::DIR_FORWARD;

  auto r = reader<DIR_FORWARD, char_type>(s, len);
  while (len-- > 0) {
    if (find(n, nlen, r.read8(len + 1U)) != nullptr)
      return r.addr<const char_type*>() - 1;
  }
  return nullptr;
}

template<>
auto char_traits<wchar_t>::strcspn(const char_type* h, size_t hlen,
                                   const char_type* n, size_t nlen)
    noexcept -> const char_type* {
  using abi::ext::reader;
  using abi::ext::DIR_FORWARD;

  auto r = reader<DIR_FORWARD, char_type>(s, len);
  while (len-- > 0) {
    if (find(n, nlen, r.read8(len + 1U)) != nullptr)
      return r.addr<const char_type*>() - 1;
  }
  return nullptr;
}

template<>
auto char_traits<char16_t>::strcspn(const char_type* h, size_t hlen,
                                    const char_type* n, size_t nlen)
    noexcept -> const char_type* {
  using abi::ext::reader;
  using abi::ext::DIR_FORWARD;

  auto r = reader<DIR_FORWARD, char_type>(s, len);
  while (len-- > 0) {
    if (find(n, nlen, r.read8(len + 1U)) != nullptr)
      return r.addr<const char_type*>() - 1;
  }
  return nullptr;
}

template<>
auto char_traits<char32_t>::strcspn(const char_type* h, size_t hlen,
                                    const char_type* n, size_t nlen)
    noexcept -> const char_type* {
  using abi::ext::reader;
  using abi::ext::DIR_FORWARD;

  auto r = reader<DIR_FORWARD, char_type>(s, len);
  while (len-- > 0) {
    if (find(n, nlen, r.read8(len + 1U)) != nullptr)
      return r.addr<const char_type*>() - 1;
  }
  return nullptr;
}


template<>
auto char_traits<char>::strrcspn(const char_type* h, size_t hlen,
                                 const char_type* n, size_t nlen)
    noexcept -> const char_type* {
  using abi::ext::reader;
  using abi::ext::DIR_BACKWARD;

  auto r = reader<DIR_BACKWARD, char_type>(s + len, len);
  while (len-- > 0) {
    if (find(n, nlen, r.read8(len + 1U)) != nullptr)
      return r.addr<const char_type*>();
  }
  return nullptr;
}

template<>
auto char_traits<wchar_t>::strrcspn(const char_type* h, size_t hlen,
                                    const char_type* n, size_t nlen)
    noexcept -> const char_type* {
  using abi::ext::reader;
  using abi::ext::DIR_BACKWARD;

  auto r = reader<DIR_BACKWARD, char_type>(s + len, len);
  while (len-- > 0) {
    if (find(n, nlen, r.read8(len + 1U)) != nullptr)
      return r.addr<const char_type*>();
  }
  return nullptr;
}

template<>
auto char_traits<char16_t>::strrcspn(const char_type* h, size_t hlen,
                                     const char_type* n, size_t nlen)
    noexcept -> const char_type* {
  using abi::ext::reader;
  using abi::ext::DIR_BACKWARD;

  auto r = reader<DIR_BACKWARD, char_type>(s + len, len);
  while (len-- > 0) {
    if (find(n, nlen, r.read8(len + 1U)) != nullptr)
      return r.addr<const char_type*>();
  }
  return nullptr;
}

template<>
auto char_traits<char32_t>::strrcspn(const char_type* h, size_t hlen,
                                     const char_type* n, size_t nlen)
    noexcept -> const char_type* {
  using abi::ext::reader;
  using abi::ext::DIR_BACKWARD;

  auto r = reader<DIR_BACKWARD, char_type>(s + len, len);
  while (len-- > 0) {
    if (find(n, nlen, r.read8(len + 1U)) != nullptr)
      return r.addr<const char_type*>();
  }
  return nullptr;
}


_namespace_end(std)
