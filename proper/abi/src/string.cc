#include <string>
#include <abi/ext/reader.h>


_namespace_begin(std)


auto char_traits<char16_t>::length(const char_type* s) noexcept -> size_t {
  return abi::ext::strlen(s);
}

auto char_traits<char16_t>::compare(const char_type* a, const char_type* b,
                                    size_t n) noexcept -> int {
  return abi::ext::memcmp(a, b, n);
}

auto char_traits<char16_t>::find(const char_type* s, size_t n,
                                 const char_type& c)
    noexcept -> const char_type* {
  return abi::ext::memchr(s, n, c);
}

auto char_traits<char16_t>::rfind(const char_type* s, size_t n,
                                  const char_type& c)
    noexcept -> const char_type* {
  return abi::ext::memrchr(s, n, c);
}


auto char_traits<char32_t>::length(const char_type* s) noexcept -> size_t {
  return abi::ext::strlen(s);
}

auto char_traits<char32_t>::compare(const char_type* a,
                                    const char_type* b,
                                    size_t n) noexcept -> int {
  return abi::ext::memcmp(a, b, n);
}

auto char_traits<char32_t>::find(const char_type* s, size_t n,
                                 const char_type& c)
    noexcept -> const char_type* {
  return abi::ext::memchr(s, n, c);
}

auto char_traits<char32_t>::rfind(const char_type* s, size_t n,
                                  const char_type& c)
    noexcept -> const char_type* {
  return abi::ext::memrchr(s, n, c);
}


auto char_traits<char>::strfind(const char_type* haystack,
                                size_t n_haystack,
                                const char_type* needle,
                                size_t n_needle)
    noexcept -> const char_type* {
  return abi::ext::memfind(haystack, n_haystack, needle, n_needle);
}

auto char_traits<wchar_t>::strfind(const char_type* haystack,
                                   size_t n_haystack,
                                   const char_type* needle,
                                   size_t n_needle)
    noexcept -> const char_type* {
  return abi::ext::memfind(haystack, n_haystack, needle, n_needle);
}

auto char_traits<char16_t>::strfind(const char_type* haystack,
                                    size_t n_haystack,
                                    const char_type* needle,
                                    size_t n_needle)
    noexcept -> const char_type* {
  return abi::ext::memfind(haystack, n_haystack, needle, n_needle);
}

auto char_traits<char32_t>::strfind(const char_type* haystack,
                                    size_t n_haystack,
                                    const char_type* needle,
                                    size_t n_needle)
    noexcept -> const char_type* {
  return abi::ext::memfind(haystack, n_haystack, needle, n_needle);
}


auto char_traits<char>::strrfind(const char_type* haystack,
                                 size_t n_haystack,
                                 const char_type* needle,
                                 size_t n_needle)
    noexcept -> const char_type* {
  return abi::ext::memrfind(haystack, n_haystack, needle, n_needle);
}

auto char_traits<wchar_t>::strrfind(const char_type* haystack,
                                    size_t n_haystack,
                                    const char_type* needle,
                                    size_t n_needle)
    noexcept -> const char_type* {
  return abi::ext::memrfind(haystack, n_haystack, needle, n_needle);
}

auto char_traits<char16_t>::strrfind(const char_type* haystack,
                                     size_t n_haystack,
                                     const char_type* needle,
                                     size_t n_needle)
    noexcept -> const char_type* {
  return abi::ext::memrfind(haystack, n_haystack, needle, n_needle);
}

auto char_traits<char32_t>::strrfind(const char_type* haystack,
                                     size_t n_haystack,
                                     const char_type* needle,
                                     size_t n_needle)
    noexcept -> const char_type* {
  return abi::ext::memrfind(haystack, n_haystack, needle, n_needle);
}


auto char_traits<char>::spn(const char_type* s, size_t len, char_type c)
    noexcept -> const char_type* {
  using abi::ext::reader;
  using abi::ext::DIR_FORWARD;

  auto r = reader<DIR_FORWARD, char_type>(s, len);
  while (len-- > 0)
    if (!eq(r.read8(len + 1U), c)) return r.addr<const char_type>() - 1;
  return nullptr;
}

auto char_traits<wchar_t>::spn(const char_type* s, size_t len, char_type c)
    noexcept -> const char_type* {
  using abi::ext::reader;
  using abi::ext::DIR_FORWARD;

  auto r = reader<DIR_FORWARD, char_type>(s, len);
  while (len-- > 0)
    if (!eq(r.read8(len + 1U), c)) return r.addr<const char_type>() - 1;
  return nullptr;
}

auto char_traits<char16_t>::spn(const char_type* s, size_t len, char_type c)
    noexcept -> const char_type* {
  using abi::ext::reader;
  using abi::ext::DIR_FORWARD;

  auto r = reader<DIR_FORWARD, char_type>(s, len);
  while (len-- > 0)
    if (!eq(r.read8(len + 1U), c)) return r.addr<const char_type>() - 1;
  return nullptr;
}

auto char_traits<char32_t>::spn(const char_type* s, size_t len, char_type c)
    noexcept -> const char_type* {
  using abi::ext::reader;
  using abi::ext::DIR_FORWARD;

  auto r = reader<DIR_FORWARD, char_type>(s, len);
  while (len-- > 0)
    if (!eq(r.read8(len + 1U), c)) return r.addr<const char_type>() - 1;
  return nullptr;
}


auto char_traits<char>::rspn(const char_type* s, size_t len, char_type c)
    noexcept -> const char_type* {
  using abi::ext::reader;
  using abi::ext::DIR_BACKWARD;

  auto r = reader<DIR_BACKWARD, char_type>(s + len, len);
  while (len-- > 0)
    if (!eq(r.read8(len + 1U), c)) return r.addr<const char_type>();
  return nullptr;
}

auto char_traits<wchar_t>::rspn(const char_type* s, size_t len, char_type c)
    noexcept -> const char_type* {
  using abi::ext::reader;
  using abi::ext::DIR_BACKWARD;

  auto r = reader<DIR_BACKWARD, char_type>(s + len, len);
  while (len-- > 0)
    if (!eq(r.read8(len + 1U), c)) return r.addr<const char_type>();
  return nullptr;
}

auto char_traits<char16_t>::rspn(const char_type* s, size_t len, char_type c)
    noexcept -> const char_type* {
  using abi::ext::reader;
  using abi::ext::DIR_BACKWARD;

  auto r = reader<DIR_BACKWARD, char_type>(s + len, len);
  while (len-- > 0)
    if (!eq(r.read8(len + 1U), c)) return r.addr<const char_type>();
  return nullptr;
}

auto char_traits<char32_t>::rspn(const char_type* s, size_t len, char_type c)
    noexcept -> const char_type* {
  using abi::ext::reader;
  using abi::ext::DIR_BACKWARD;

  auto r = reader<DIR_BACKWARD, char_type>(s + len, len);
  while (len-- > 0)
    if (!eq(r.read8(len + 1U), c)) return r.addr<const char_type>();
  return nullptr;
}


auto char_traits<char>::strspn(const char_type* h, size_t hlen,
                               const char_type* n, size_t nlen)
    noexcept -> const char_type* {
  using abi::ext::reader;
  using abi::ext::DIR_FORWARD;

  auto r = reader<DIR_FORWARD, char_type>(h, hlen);
  while (hlen-- > 0) {
    if (find(n, nlen, r.read8(hlen + 1U)) == nullptr)
      return r.addr<const char_type>() - 1;
  }
  return nullptr;
}

auto char_traits<wchar_t>::strspn(const char_type* h, size_t hlen,
                                  const char_type* n, size_t nlen)
    noexcept -> const char_type* {
  using abi::ext::reader;
  using abi::ext::DIR_FORWARD;

  auto r = reader<DIR_FORWARD, char_type>(h, hlen);
  while (hlen-- > 0) {
    if (find(n, nlen, r.read8(hlen + 1U)) == nullptr)
      return r.addr<const char_type>() - 1;
  }
  return nullptr;
}

auto char_traits<char16_t>::strspn(const char_type* h, size_t hlen,
                                   const char_type* n, size_t nlen)
    noexcept -> const char_type* {
  using abi::ext::reader;
  using abi::ext::DIR_FORWARD;

  auto r = reader<DIR_FORWARD, char_type>(h, hlen);
  while (hlen-- > 0) {
    if (find(n, nlen, r.read8(hlen + 1U)) == nullptr)
      return r.addr<const char_type>() - 1;
  }
  return nullptr;
}

auto char_traits<char32_t>::strspn(const char_type* h, size_t hlen,
                                   const char_type* n, size_t nlen)
    noexcept -> const char_type* {
  using abi::ext::reader;
  using abi::ext::DIR_FORWARD;

  auto r = reader<DIR_FORWARD, char_type>(h, hlen);
  while (hlen-- > 0) {
    if (find(n, nlen, r.read8(hlen + 1U)) == nullptr)
      return r.addr<const char_type>() - 1;
  }
  return nullptr;
}


auto char_traits<char>::strrspn(const char_type* h, size_t hlen,
                                const char_type* n, size_t nlen)
    noexcept -> const char_type* {
  using abi::ext::reader;
  using abi::ext::DIR_BACKWARD;

  auto r = reader<DIR_BACKWARD, char_type>(h + hlen, hlen);
  while (hlen-- > 0) {
    if (find(n, nlen, r.read8(hlen + 1U)) == nullptr)
      return r.addr<const char_type>();
  }
  return nullptr;
}

auto char_traits<wchar_t>::strrspn(const char_type* h, size_t hlen,
                                   const char_type* n, size_t nlen)
    noexcept -> const char_type* {
  using abi::ext::reader;
  using abi::ext::DIR_BACKWARD;

  auto r = reader<DIR_BACKWARD, char_type>(h + hlen, hlen);
  while (hlen-- > 0) {
    if (find(n, nlen, r.read8(hlen + 1U)) == nullptr)
      return r.addr<const char_type>();
  }
  return nullptr;
}

auto char_traits<char16_t>::strrspn(const char_type* h, size_t hlen,
                                    const char_type* n, size_t nlen)
    noexcept -> const char_type* {
  using abi::ext::reader;
  using abi::ext::DIR_BACKWARD;

  auto r = reader<DIR_BACKWARD, char_type>(h + hlen, hlen);
  while (hlen-- > 0) {
    if (find(n, nlen, r.read8(hlen + 1U)) == nullptr)
      return r.addr<const char_type>();
  }
  return nullptr;
}

auto char_traits<char32_t>::strrspn(const char_type* h, size_t hlen,
                                    const char_type* n, size_t nlen)
    noexcept -> const char_type* {
  using abi::ext::reader;
  using abi::ext::DIR_BACKWARD;

  auto r = reader<DIR_BACKWARD, char_type>(h + hlen, hlen);
  while (hlen-- > 0) {
    if (find(n, nlen, r.read8(hlen + 1U)) == nullptr)
      return r.addr<const char_type>();
  }
  return nullptr;
}


auto char_traits<char>::strcspn(const char_type* h, size_t hlen,
                                const char_type* n, size_t nlen)
    noexcept -> const char_type* {
  using abi::ext::reader;
  using abi::ext::DIR_FORWARD;

  auto r = reader<DIR_FORWARD, char_type>(h, hlen);
  while (hlen-- > 0) {
    if (find(n, nlen, r.read8(hlen + 1U)) != nullptr)
      return r.addr<const char_type>() - 1;
  }
  return nullptr;
}

auto char_traits<wchar_t>::strcspn(const char_type* h, size_t hlen,
                                   const char_type* n, size_t nlen)
    noexcept -> const char_type* {
  using abi::ext::reader;
  using abi::ext::DIR_FORWARD;

  auto r = reader<DIR_FORWARD, char_type>(h, hlen);
  while (hlen-- > 0) {
    if (find(n, nlen, r.read8(hlen + 1U)) != nullptr)
      return r.addr<const char_type>() - 1;
  }
  return nullptr;
}

auto char_traits<char16_t>::strcspn(const char_type* h, size_t hlen,
                                    const char_type* n, size_t nlen)
    noexcept -> const char_type* {
  using abi::ext::reader;
  using abi::ext::DIR_FORWARD;

  auto r = reader<DIR_FORWARD, char_type>(h, hlen);
  while (hlen-- > 0) {
    if (find(n, nlen, r.read8(hlen + 1U)) != nullptr)
      return r.addr<const char_type>() - 1;
  }
  return nullptr;
}

auto char_traits<char32_t>::strcspn(const char_type* h, size_t hlen,
                                    const char_type* n, size_t nlen)
    noexcept -> const char_type* {
  using abi::ext::reader;
  using abi::ext::DIR_FORWARD;

  auto r = reader<DIR_FORWARD, char_type>(h, hlen);
  while (hlen-- > 0) {
    if (find(n, nlen, r.read8(hlen + 1U)) != nullptr)
      return r.addr<const char_type>() - 1;
  }
  return nullptr;
}


auto char_traits<char>::strrcspn(const char_type* h, size_t hlen,
                                 const char_type* n, size_t nlen)
    noexcept -> const char_type* {
  using abi::ext::reader;
  using abi::ext::DIR_BACKWARD;

  auto r = reader<DIR_BACKWARD, char_type>(h + hlen, hlen);
  while (hlen-- > 0) {
    if (find(n, nlen, r.read8(hlen + 1U)) != nullptr)
      return r.addr<const char_type>();
  }
  return nullptr;
}

auto char_traits<wchar_t>::strrcspn(const char_type* h, size_t hlen,
                                    const char_type* n, size_t nlen)
    noexcept -> const char_type* {
  using abi::ext::reader;
  using abi::ext::DIR_BACKWARD;

  auto r = reader<DIR_BACKWARD, char_type>(h + hlen, hlen);
  while (hlen-- > 0) {
    if (find(n, nlen, r.read8(hlen + 1U)) != nullptr)
      return r.addr<const char_type>();
  }
  return nullptr;
}

auto char_traits<char16_t>::strrcspn(const char_type* h, size_t hlen,
                                     const char_type* n, size_t nlen)
    noexcept -> const char_type* {
  using abi::ext::reader;
  using abi::ext::DIR_BACKWARD;

  auto r = reader<DIR_BACKWARD, char_type>(h + hlen, hlen);
  while (hlen-- > 0) {
    if (find(n, nlen, r.read8(hlen + 1U)) != nullptr)
      return r.addr<const char_type>();
  }
  return nullptr;
}

auto char_traits<char32_t>::strrcspn(const char_type* h, size_t hlen,
                                     const char_type* n, size_t nlen)
    noexcept -> const char_type* {
  using abi::ext::reader;
  using abi::ext::DIR_BACKWARD;

  auto r = reader<DIR_BACKWARD, char_type>(h + hlen, hlen);
  while (hlen-- > 0) {
    if (find(n, nlen, r.read8(hlen + 1U)) != nullptr)
      return r.addr<const char_type>();
  }
  return nullptr;
}


template class basic_string<char>;
template class basic_string<wchar_t>;
template class basic_string<char16_t>;
template class basic_string<char32_t>;


namespace {

template<typename InputIterator>
auto read_size_t(InputIterator b, InputIterator e) noexcept ->
    enable_if_t<sizeof(typename iterator_traits<InputIterator>::value_type) >=
                sizeof(size_t),
                tuple<typename iterator_traits<InputIterator>::value_type,
                      InputIterator>> {
  assert(b != e);
  typename iterator_traits<InputIterator>::value_type v = *b;
  ++b;
  return make_tuple(move(v), move(b));
}

template<typename InputIterator>
auto read_size_t(InputIterator b, InputIterator e) noexcept ->
    enable_if_t<sizeof(typename iterator_traits<InputIterator>::value_type) <
                sizeof(size_t),
                tuple<size_t,
                      InputIterator>> {
  assert(b != e);
  size_t v = 0;
  for (unsigned int i = 0;
       b != e &&
       i < sizeof(size_t) /
           sizeof(typename iterator_traits<InputIterator>::value_type);
       ++i) {
    v <<= CHAR_BIT *
          sizeof(typename iterator_traits<InputIterator>::value_type);
    v |= *b;
    ++b;
  }
  return make_tuple(move(v), move(b));
}

/* Murmur hash. */
template<typename InputIterator, typename N = size_t>
auto hash_impl(InputIterator b, InputIterator e) noexcept ->
    enable_if_t<sizeof(N) == 4, N> {
  uint32_t c1 = 0xcc9e2d51;
  uint32_t c2 = 0x1b873593;
  uint32_t r1 = 15;
  uint32_t r2 = 13;
  uint32_t m = 5;
  uint32_t n = 0xe6546b64;

  /* Seed. */
  uint32_t hash =
      17 * sizeof(typename iterator_traits<InputIterator>::value_type);
  if (b == e) return hash;

  auto n_b = read_size_t(b, e);
  auto n_ = get<0>(n_b);
  b = get<1>(n_b);
  unsigned int shift = (sizeof(n_) + sizeof(N) - 1) / sizeof(N);

  uint32_t k = get<0>(n_b);
  k *= c1;
  k = (k << r1) | (k >> (32 - r1));
  k *= c2;
  hash ^= k;
  hash = (hash << r2) | (hash >> (32 - r2));
  hash = hash * m + n;

  for (unsigned int i = 1; i < shift; ++i) {
    get<0>(n_b) >>= (sizeof(n_) <= 4 ? 0 : 32);
    uint32_t rb = n_;
    rb *= c1;
    rb = (rb << r1) | (rb >> (32 - r1));
    rb *= c2;
    hash ^= rb;
  }

  uint32_t count = 1;
  while (b != e) {
    tie(n_, b) = read_size_t(b, e);
    for (unsigned int i = 1; i < shift; ++i) {
      n_ >>= (sizeof(n_) <= 4 ? 0 : 32);  // Shift, silence compiler warning.
      uint32_t rb = n_;
      rb *= c1;
      rb = (rb << r1) | (rb >> (32 - r1));
      rb *= c2;
      hash ^= rb;
    }
    ++count;
  }

  /* hash ^= len. */
  hash ^=
      count * sizeof(typename iterator_traits<InputIterator>::value_type);

  hash ^= (hash >> 16);
  hash *= 0x85ebca6b;
  hash ^= (hash >> 13);
  hash *= 0xc2b2ae35;
  hash ^= (hash >> 16);

  return hash;
}

template<typename InputIterator, typename N = size_t>
auto hash_impl(InputIterator b, InputIterator e) noexcept ->
    enable_if_t<sizeof(N) == 8, N> {
  uint64_t c1 = 0xc6a4a7935bd1e995ULL;
  uint64_t c2 = 0xcc9e2d511b873593ULL;
  uint64_t r1 = 47;
  uint64_t r2 = 13;
  uint64_t m = 5;
  uint64_t n = 0xe6546b64;

  /* Fixed seed. */
  uint64_t hash =
      17 * sizeof(typename iterator_traits<InputIterator>::value_type);
  if (b == e) return hash;

  auto n_b = read_size_t(b, e);
  auto n_ = get<0>(n_b);
  b = get<1>(n_b);
  unsigned int shift = (sizeof(n_) + sizeof(N) - 1) / sizeof(N);

  uint64_t k = n_;
  k *= c1;
  k = (k << r1) | (k >> (64 - r1));
  k *= c2;
  hash ^= k;
  hash = (hash << r2) | (hash >> (64 - r2));
  hash = hash * m + n;

  for (unsigned int i = 1; i < shift; ++i) {
    n_ >>= (sizeof(n_) <= 8 ? 0 : 64);  // Shift, but silence compiler warning.
    uint64_t rb = n_;
    rb *= c1;
    rb = (rb << r1) | (rb >> (64 - r1));
    rb *= c2;
    hash ^= rb;
  }

  uint64_t count = 1;
  while (++b != e) {
    tie(n_, b) = read_size_t(b, e);
    for (unsigned int i = 1; i < shift; ++i) {
      n_ >>= (sizeof(n_) <= 4 ? 0 : 32);  // Shift, silence compiler warning.
      uint32_t rb = n_;
      rb *= c1;
      rb = (rb << r1) | (rb >> (32 - r1));
      rb *= c2;
      hash ^= rb;
    }
    ++count;
  }

  /* hash ^= len. */
  hash ^=
      count * sizeof(typename iterator_traits<InputIterator>::value_type);

  hash ^= (hash >> 33);
  hash *= 0xff51afd7ed558ccdULL;
  hash ^= (hash >> 33);
  hash *= 0xc4ceb9fe1a85ec53ULL;
  hash ^= (hash >> 33);

  return hash;
}

} /* namespace std::<unnamed> */

size_t hash<string_ref>::operator()(string_ref s) const noexcept {
  return hash_impl(s.begin(), s.end());
}

size_t hash<u16string_ref>::operator()(u16string_ref s) const noexcept {
  return hash_impl(s.begin(), s.end());
}

size_t hash<u32string_ref>::operator()(u32string_ref s) const noexcept {
  return hash_impl(s.begin(), s.end());
}

size_t hash<wstring_ref>::operator()(wstring_ref s) const noexcept {
  return hash_impl(s.begin(), s.end());
}


_namespace_end(std)
