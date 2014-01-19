#ifndef _ABI_MEMORY_H_
#define _ABI_MEMORY_H_

#include <abi/_config.h>
#include <abi/abi.h>

namespace __cxxabiv1 {


using _config::heap;


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

/*
 * Zero memory at given address, for sz bytes.
 * This function is external, to prevent the compiler from
 * optimizing it out.
 */
void safe_memzero(void*, size_t) noexcept;


} /* namespace __cxxabiv1 */

#endif /* _ABI_MEMORY_H_ */
