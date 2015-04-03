#include <abi/demangle.h>

/*
 * These are simply stubs.
 * A real implementation should be provided by someone, somewhere.
 */
namespace __cxxabiv1 {

char* __attribute__((weak)) __cxa_demangle(const char*, char*,
                                           _TYPES(size_t)*,
                                           int* status) noexcept {
  if (status) *status = -1;  // No malloc avaible. :)
  return nullptr;
}

char* __attribute__((weak)) __cxa_demangle_gnu3(const char* mangled) noexcept {
  return __cxa_demangle(mangled, nullptr, nullptr, nullptr);
}

} /* namespace __cxxabiv1 */
