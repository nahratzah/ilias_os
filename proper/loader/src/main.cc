#include <loader/main.h>
#include <loader/x86_video.h>
#include <loader/ldexport_init.h>
#include <loader/page.h>
#include <ilias/pmap/page.h>
#include <ilias/pmap/consts.h>
#include <ilias/cpuid.h>

namespace loader {

/* Provided by linker script. */
extern "C" char kernel_start;
extern "C" char kernel_end;

void main() {
  bios_printf("CPU vendor: %s\n", (ilias::has_cpuid() ?
                                   ilias::cpu_vendor().c_str() :
                                   "unknown (cpuid not supported)"));

  /* Initialize export data. */
  ldexport& lde = ldexport_get();
  bios_put_str(lde.to_string());

  /* Initialize our page map. */
  page_allocator<ilias::arch::i386> pga;
  for (const auto& range : lde.physmem) {
    auto addr = ilias::pmap::round_page_up(range.addr, ilias::arch::i386);
    auto end = ilias::pmap::round_page_down(range.addr + range.len,
                                            ilias::arch::i386);
    if (end > addr)
    pga.add_range(addr, end - addr);
  }
  pga.shrink_to_fit();

  /* Punch out the loader, so it won't get allocated in. */
  {
    uintptr_t start = reinterpret_cast<uintptr_t>(&kernel_start);
    uintptr_t end = reinterpret_cast<uintptr_t>(&kernel_end);
    start &= ~uintptr_t(ilias::pmap::PAGE_MASK);
    end += ilias::pmap::PAGE_MASK;
    end &= ~uintptr_t(ilias::pmap::PAGE_MASK);
    pga.mark_in_use(ilias::pmap::phys_addr<ilias::arch::i386>(start),
                    ilias::pmap::phys_addr<ilias::arch::i386>(end));
  }
}

} /* namespace loader */
