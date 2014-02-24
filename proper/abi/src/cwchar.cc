#include <cwchar>
#include <cstring>
#include <abi/ext/reader.h>

namespace std {


int wmemcmp(const wchar_t* a, const wchar_t* b, size_t len) noexcept {
  return abi::ext::memcmp(a, b, len);
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
  if (len <= 0) return nullptr;
  auto r = abi::ext::reader<abi::ext::DIR_FORWARD, wchar_t>(p, len);
  while (len-- > 0) {
    if (c == r.read8(len + 1U))
      return const_cast<wchar_t*>(r.addr<wchar_t>() - 1U);
  }
  return nullptr;
}

wchar_t* wmemrchr(const wchar_t* p, wchar_t c, size_t len) noexcept {
  if (len <= 0) return nullptr;
  auto r = abi::ext::reader<abi::ext::DIR_BACKWARD, wchar_t>(p + len, len);
  while (len-- > 0) {
    if (c == r.read8(len + 1U))
      return const_cast<wchar_t*>(r.addr<wchar_t>());
  }
  return nullptr;
}

size_t wcslen(const wchar_t* s) noexcept {
  if (_predict_false(!s)) return 0;
  auto r = abi::ext::reader<abi::ext::DIR_FORWARD, wchar_t>(s);
  size_t len = 0;
  while (r.read8() != wchar_t()) ++len;
  return len;
}


} /* namespace std */
