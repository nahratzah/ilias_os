#include <cwchar>
#include <cstring>

namespace std {


wchar_t* wmemmove(wchar_t* dst, const wchar_t* src, size_t len) noexcept {
  return static_cast<wchar_t*>(memmove(dst, src, len * sizeof(*src)));
}

wchar_t* wmemcpy(wchar_t* dst, const wchar_t* src, size_t len) noexcept {
  return static_cast<wchar_t*>(memcpy(dst, src, len * sizeof(*src)));
}

int wmemcmp(const wchar_t* a, const wchar_t* b, size_t len) noexcept {
  for (; len > 0; ++a, ++b, --len)
    if (*a != *b) return *a - *b;
  return 0;
}

wchar_t* wmemchr(const wchar_t* s, wchar_t c, size_t len) noexcept {
  for (; len > 0; ++s, --len)
    if (*s == c) return const_cast<wchar_t*>(s);
  return nullptr;
}

size_t wcslen(const wchar_t* s) noexcept {
  size_t len = 0;
  while (*s++) ++len;
  return len;
}

wchar_t* wmemset(wchar_t* s, wchar_t c, size_t len) noexcept {
  while (--len) *s++ = c;
  return s;
}


}
