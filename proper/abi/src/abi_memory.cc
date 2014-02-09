#include <abi/memory.h>

namespace __cxxabiv1 {


void safe_memzero(void* p, size_t sz) noexcept {
  memzero(p, sz);
}


} /* namespace __cxxabiv1 */
