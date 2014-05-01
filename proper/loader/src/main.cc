#include <loader/main.h>
#include <loader/x86_video.h>
#include <loader/ldexport_init.h>
#include <loader/page.h>
#include <ilias/pmap/page.h>
#include <ilias/pmap/consts.h>
#include <ilias/cpuid.h>
#include <ilias/pmap/pmap_i386.h>

namespace loader {

/* Provided by linker script. */
extern "C" char kernel_start;
extern "C" char kernel_end;


/* Set up page allocator so it tracks all pages below 4GB. */
void setup_page_allocator(page_allocator<ilias::native_arch>& pga,
                          const ldexport& lde) {
  /* Track all pages. */
  for (const auto& range : lde.physmem) {
    auto addr = ilias::pmap::round_page_up(range.addr, ilias::native_arch);
    auto end = ilias::pmap::round_page_down(range.addr + range.len,
                                            ilias::native_arch);
    if (end > 0x100000000ULL) end = 0x100000000ULL;  // Truncate to 4GB.
    if (end > addr) pga.add_range(addr, end - addr);
  }
  pga.shrink_to_fit();

  /* Punch out the loader, so it won't get allocated in. */
  {
    uintptr_t start = reinterpret_cast<uintptr_t>(&kernel_start);
    uintptr_t end = reinterpret_cast<uintptr_t>(&kernel_end);
    start = ilias::pmap::round_page_down(start, ilias::native_arch);
    end = ilias::pmap::round_page_up(end, ilias::native_arch);
    pga.mark_in_use(ilias::pmap::phys_addr<ilias::native_arch>(start),
                    ilias::pmap::phys_addr<ilias::native_arch>(end));
  }
}

/* Map all pages from the loader into the page map. */
void setup_loader_pmap(ilias::pmap::pmap<ilias::native_arch>& loader_pmap) {
  using ilias::pmap::vaddr;
  using ilias::pmap::vpage_no;
  using ilias::pmap::page_no;
  using ilias::pmap::round_page_down;
  using ilias::pmap::round_page_up;
  using ilias::native_arch;

  const auto end = vaddr<native_arch>(
      round_page_up(reinterpret_cast<uintptr_t>(&kernel_end),
                    native_arch));
  for (vpage_no<native_arch> start = vaddr<native_arch>(
           round_page_down(reinterpret_cast<uintptr_t>(&kernel_start),
                           native_arch));
       start != end;
       ++start) {
    auto pg = page_no<native_arch>(start.get());  // One-to-one mapping.
    loader_pmap.map(start, pg);
  }
}


void main() {
  bios_printf("CPU vendor: %s\n", (ilias::has_cpuid() ?
                                   ilias::cpu_vendor().c_str() :
                                   "unknown (cpuid not supported)"));

  /* Initialize export data. */
  ldexport& lde = ldexport_get();
  bios_put_str(lde.to_string());

  /* Initialize our page map. */
  page_allocator<ilias::native_arch> pga;
  setup_page_allocator(pga, lde);

  /* Create pmap for loader. */
  ilias::pmap::pmap<ilias::native_arch> loader_pmap{ pga };
  setup_loader_pmap(loader_pmap);
}

} /* namespace loader */
