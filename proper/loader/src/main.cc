#include <loader/main.h>
#include <loader/x86_video.h>
#include <loader/ldexport_init.h>
#include <loader/page.h>
#include <ilias/pmap/page.h>

namespace loader {

/* Provided by linker script. */
extern "C" char kernel_start;
extern "C" char kernel_end;

void main() {
  /* Initialize export data. */
  ldexport& lde = ldexport_get();

  /* Initialize our page map. */
  page_allocator<ilias::arch::i386> pga;
  for (const auto& range : lde.physmem)
    pga.add_range(range.addr, range.len);
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

  bios_put_str(lde.to_string());
}

} /* namespace loader */
