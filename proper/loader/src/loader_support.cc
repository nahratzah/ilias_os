#include <cdecl.h>
#include <abi/abi.h>
#include <abi/memory.h>
#include <loader/x86_video.h>
#include <iterator>

namespace loader {

_cdecl_begin

uint32_t mb_magic;
uint32_t mb_data;

void loader_setup() noexcept {
  bios_put_str("Loader setup...\n");
}

void bss_zero() noexcept {
  extern abi::uint8_t sbss, ebss;  /* Provided by linker. */

  abi::memzero(&sbss, ebss - sbss);
  bios_put_str("BSS cleared.\n");
}


namespace {

using ctor_dtor = void (*)();  /* Global constructor/destructor. */

} /* namespace loader::<unnamed> */


void loader_constructors() noexcept {
  extern ctor_dtor start_ctors, end_ctors;  /* Provided by linker. */

  ctor_dtor* i = &start_ctors;
  while (i != &end_ctors) (*i++)();
}

void loader_destructors() noexcept {
  extern ctor_dtor start_dtors, end_dtors;  /* Provided by linker. */

  ctor_dtor* i = &end_dtors;
  while (i-- != &start_dtors) (*i)();
}

_cdecl_end

} /* namespace loader */
