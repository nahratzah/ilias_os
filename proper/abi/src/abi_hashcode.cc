#include <abi/hashcode.h>
#include <cdecl.h>

namespace __cxxabiv1 {


size_t hash_code(intmax_t v) noexcept {
  intmax_t rv = 0;
  for (unsigned int i = 0; i < sizeof(v); i *= 2) rv ^= v;
  return rv;
}

size_t hash_code(uintmax_t v) noexcept {
  uintmax_t rv = 0;
  for (unsigned int i = 0; i < sizeof(v); i *= 2) rv ^= v;
  return rv;
}

size_t hash_code(const void* p) noexcept {
  return reinterpret_cast<uintmax_t>(p);
}

size_t hash_code(const char* s) noexcept {
  size_t rv = 0;
  if (_predict_false(s == nullptr)) return rv;
  for (unsigned int i = 0; *s && i < sizeof(size_t) - 8; ++i, ++s) {
    rv ^= size_t(*s) << i;
  }
  return rv;
}


} /* namespace __cxxabiv1 */
