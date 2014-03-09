#include <loader/ldexport_init.h>

#if defined(__i386__) || defined(__amd64__) || defined(__x86_64__)
# include <loader/amd64.h>
#endif

namespace loader {
namespace {


ldexport create() {
  ldexport impl;

#if defined(__i386__) || defined(__amd64__) || defined(__x86_64__)
  impl.physmem = amd64_memorymap();
#else
# error Unknown platform (implement me).
#endif

  return impl;
}


} /* namespace loader::<unnamed> */


ldexport& ldexport_get() {
  static ldexport impl = create();
  return impl;
}


} /* namesapce loader */
