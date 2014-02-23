#include <string>
#include <abi/ext/reader.h>


_namespace_begin(std)


template<>
auto char_traits<char>::strfind(const char_type* haystack, size_t n_haystack,
                                const char_type* needle, size_type n_needle)
    noexcept {
  return abi::ext::memfind(haystack, n_haystack, needle, n_needle);
}

template<>
auto char_traits<wchar_t>::strfind(const char_type* haystack,
                                   size_t n_haystack,
                                   const char_type* needle,
                                   size_type n_needle) noexcept {
  return abi::ext::memfind(haystack, n_haystack, needle, n_needle);
}

template<>
auto char_traits<char16_t>::strfind(const char_type* haystack,
                                    size_t n_haystack,
                                    const char_type* needle,
                                    size_type n_needle) noexcept {
  return abi::ext::memfind(haystack, n_haystack, needle, n_needle);
}

template<>
auto char_traits<char32_t>::strfind(const char_type* haystack,
                                    size_t n_haystack,
                                    const char_type* needle,
                                    size_type n_needle) noexcept {
  return abi::ext::memfind(haystack, n_haystack, needle, n_needle);
}


_namespace_end(std)
