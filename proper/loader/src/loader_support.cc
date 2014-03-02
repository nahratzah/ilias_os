#include <cdecl.h>
#include <abi/abi.h>
#include <abi/memory.h>
#include <loader/x86_video.h>

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

_cdecl_end

} /* namespace loader */
