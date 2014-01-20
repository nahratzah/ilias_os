#include <abi/demangle.h>

/*
 * These are simply stubs.
 * A real implementation should be provided by someone, somewhere.
 */
namespace __cxxabiv1 {

char* __attribute__((weak)) __cxa_demangle(const char* mangled_name, char* buf,
                                           _TYPES(size_t)* buflen,
                                           int* status) noexcept {
  if (status) *status = -1;  // No malloc avaible. :)
  return nullptr;
}

char* __attribute__((weak)) __cxa_demangle_gnu3(const char* mangled_name)
    noexcept {
  return nullptr;
}

} /* namespace __cxxabiv1 */
