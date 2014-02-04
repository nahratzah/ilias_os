#include <cwchar>
#include <cstring>

namespace std {


int wmemcmp(const wchar_t* a, const wchar_t* b, size_t len) noexcept {
  while (--len) {
    int cmp = (int(*a++) - int(*b++));
    if (cmp != 0) return cmp;
  }
  return 0;
}

wchar_t* wmemset(wchar_t* p, wchar_t v, size_t len) noexcept {
  for (wchar_t* i = p; i != p + len; ++i) *i = v;
  return p;
}

wchar_t* wmemcpy(wchar_t*__restrict dst, const wchar_t*__restrict src,
    size_t len) noexcept {
  return static_cast<wchar_t*>(memcpy(dst, src, sizeof(wchar_t) * len));
}

wchar_t* wmemmove(wchar_t* dst, const wchar_t* src, size_t len) noexcept {
  return static_cast<wchar_t*>(memmove(dst, src, sizeof(wchar_t) * len));
}

wchar_t* wmemchr(const wchar_t* p, wchar_t c, size_t len) noexcept {
  for (const wchar_t* i = p; len > 0; ++i, --len)
    if (*i == c) return const_cast<wchar_t*>(i);
  return nullptr;
}

wchar_t* wmemrchr(const wchar_t* p, wchar_t c, size_t len) noexcept {
  wchar_t* rv = nullptr;
  for (const wchar_t* i = p; len > 0; ++i, --len)
    if (*i == c) rv = const_cast<wchar_t*>(i);
  return rv;
}

size_t wcslen(const wchar_t* s) noexcept {
  if (!s) return 0;
  size_t len = 0;
  while (*s++) len++;
  return len;
}


} /* namespace std */