#include <loader/ldexport_init.h>
#include <loader/multiboot.h>

#if defined(__i386__) || defined(__amd64__) || defined(__x86_64__)
# include <loader/amd64.h>
#endif

namespace loader {
namespace {


ldexport create() {
  ldexport impl;

  if (multiboot::valid()) {
    for (const auto& line : multiboot::memory_map())
      if (line.is_free()) impl.physmem.push_back({ line.base, line.len });

    impl.cmd_line = multiboot::command_line();
  }

  /* Use fallback code is memory map is not available. */
  if (impl.physmem.empty()) {
#if defined(__i386__) || defined(__amd64__) || defined(__x86_64__)
    impl.physmem = amd64_memorymap();
#else
# error Unknown platform (implement me).
#endif
  }

  return impl;
}


} /* namespace loader::<unnamed> */


ldexport& ldexport_get() {
  static ldexport impl = create();
  return impl;
}


} /* namesapce loader */
