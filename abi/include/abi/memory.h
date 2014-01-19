#ifndef _ABI_MEMORY_H_
#define _ABI_MEMORY_H_

#include <abi/abi.h>

namespace __cxxabiv1 {


/*
 * Memory allocation function.
 *
 * sz: size of allocation
 * subsys: a string indicating what the memory is used for;
 *         all allocations from the abi start with "abi/".
 *
 * Returns null if the allocation fails.
 */
inline void* malloc(size_t sz, const char* subsys) noexcept {
  return nullptr;  // XXX implement
}

/*
 * Free memory previously allocated using abi::malloc.
 */
inline void free(void*) noexcept {
  return;  // XXX implement
}

/*
 * Zero memory at given address, for sz bytes.
 */
inline void memzero(void* addr, size_t sz) noexcept {
  uintptr_t* lp;
  uint8_t* i = reinterpret_cast<uint8_t*>(addr);

  /* Zero bytes until we reach alignment of lp. */
  while (sz >= sizeof(*i) &&
         reinterpret_cast<uintptr_t>(i) % alignof(decltype(*lp)) != 0) {
    *i++ = 0;
    sz -= sizeof(*i);
  }

  /* Zero bytes while lp is fully within (addr, sz). */
  lp = reinterpret_cast<uintptr_t*>(i);
  while (sz >= sizeof(*lp)) {
    *lp++ = 0;
    sz -= sizeof(*lp);
  }

  /* Zero last trailing bytes of (data, sz). */
  i = reinterpret_cast<uint8_t*>(lp);
  while (sz >= sizeof(*i)) {
    *i++ = 0;
    sz -= sizeof(*i);
  }
}


} /* namespace __cxxabiv1 */

#endif /* _ABI_MEMORY_H_ */
