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

  bios_put_str("Running constructors...\n");
  ctor_dtor* i = &start_ctors;
  while (i != &end_ctors) (*i++)();
  bios_put_str("Done running constructors...\n");
}

void loader_destructors() noexcept {
  extern ctor_dtor start_dtors, end_dtors;  /* Provided by linker. */

  bios_put_str("Running destructors...\n");
  ctor_dtor* i = &end_dtors;
  while (i-- != &start_dtors) (*i)();
  bios_put_str("Done running destructors\n");
}

void loader_print_endmsg() noexcept {
  bios_put_str("Loader has finished.  Will attempt to halt the system,\n"
               "                      or spin indefinately");
}

_cdecl_end

} /* namespace loader */
