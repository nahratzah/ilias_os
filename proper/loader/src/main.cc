#include <loader/main.h>
#include <loader/x86_video.h>
#include <loader/ldexport_init.h>
#include <loader/page.h>
#include <ilias/pmap/page.h>
#include <ilias/pmap/consts.h>
#include <ilias/cpuid.h>
#include <ilias/pmap/pmap_i386.h>
#include <ilias/i386/paging.h>
#include <mutex>

namespace loader {

/* Provided by linker script. */
extern "C" char kernel_start;
extern "C" char kernel_end;

namespace {

/* Set up page allocator so it tracks all pages below 4GB. */
void setup_page_allocator(page_allocator<ilias::native_arch>& pga,
                          const ldexport& lde,
                          uint64_t base, uint64_t limit) {
  base = ilias::pmap::round_page_up(base, ilias::native_arch);
  limit = ilias::pmap::round_page_down(limit, ilias::native_arch);
  if (_predict_false(base >= limit)) return;

  /* Track all pages. */
  for (const auto& range : lde.physmem) {
    auto addr = ilias::pmap::round_page_up(range.addr, ilias::native_arch);
    auto end = ilias::pmap::round_page_down(range.addr + range.len,
                                            ilias::native_arch);
    if (addr < base) addr = base;
    if (end > limit) end = limit;
    if (end > addr) pga.add_range(addr, end - addr);
  }
  pga.shrink_to_fit();
}

/* Punch out the loader, so it won't get allocated in. */
void punch_loader(page_allocator<ilias::native_arch>& pga) {
  uintptr_t start = reinterpret_cast<uintptr_t>(&kernel_start);
  uintptr_t end = reinterpret_cast<uintptr_t>(&kernel_end);
  start = ilias::pmap::round_page_down(start, ilias::native_arch);
  end = ilias::pmap::round_page_up(end, ilias::native_arch);
  pga.mark_in_use(ilias::pmap::phys_addr<ilias::native_arch>(start),
                  ilias::pmap::phys_addr<ilias::native_arch>(end));
}

/* Map all pages from the loader into the page map. */
void setup_loader_pmap(ilias::pmap::pmap<ilias::native_arch>& loader_pmap) {
  using ilias::pmap::vaddr;
  using ilias::pmap::vpage_no;
  using ilias::pmap::page_no;
  using ilias::pmap::round_page_down;
  using ilias::pmap::round_page_up;
  using ilias::pmap::permission;
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
    loader_pmap.map(start, pg, permission::RWX());
  }
}

/*
 * Create and initialize page allocator.
 *
 * This is created using a manual call-once construct, since it must
 * outlive the lifetime of its associated pmaps.
 */
page_allocator<ilias::native_arch>& get_pga() {
  using store_t =
      std::aligned_storage_t<sizeof(page_allocator<ilias::native_arch>),
                             alignof(page_allocator<ilias::native_arch>)>;
  using std::once_flag;
  using std::call_once;

  static once_flag guard;
  static store_t store;

  void* impl = &store;
  call_once(guard,
            [](void* p) {
              new (p) page_allocator<ilias::native_arch>();
            },
            impl);
  return *static_cast<page_allocator<ilias::native_arch>*>(impl);
}

/*
 * Create loader pmap.
 *
 * It is created using a manual call-once construct, since destroying this
 * is going to have disastrous consequences (potentially).  I.e. being unable
 * to load addresses in the failure path of the loader.
 */
ilias::pmap::pmap<ilias::native_arch>& get_pmap(
    page_allocator<ilias::native_arch>& pga) {
  using store_t =
      std::aligned_storage_t<sizeof(ilias::pmap::pmap<ilias::native_arch>),
                             alignof(ilias::pmap::pmap<ilias::native_arch>)>;
  using std::once_flag;
  using std::call_once;

  static once_flag guard;
  static store_t store;

  void* impl = &store;
  call_once(guard,
            [](void* p, page_allocator<ilias::native_arch>& pga) {
              new (p) ilias::pmap::pmap<ilias::native_arch>(pga);
            },
            impl, pga);
  return *static_cast<ilias::pmap::pmap<ilias::native_arch>*>(impl);
}

} /* namespace loader::<unnamed> */


void main() {
  bios_printf("CPU vendor: %s\n", (ilias::has_cpuid() ?
                                   ilias::cpu_vendor().c_str() :
                                   "unknown (cpuid not supported)"));

  /* Initialize export data. */
  ldexport& lde = ldexport_get();
  bios_put_str(lde.to_string());

  /* Initialize our page allocator. */
  page_allocator<ilias::native_arch>& pga = get_pga();
  uintptr_t lim = 2 * reinterpret_cast<uintptr_t>(&kernel_end);
  if (lim < 32 * 1024 * 1024) lim = 32 * 1024 * 1024;
  setup_page_allocator(pga, lde, 1024 * 1024, lim);
  punch_loader(pga);

  /* Create pmap for loader. */
  ilias::pmap::pmap<ilias::native_arch>& loader_pmap = get_pmap(pga);
  setup_loader_pmap(loader_pmap);

  /* Enable paging. */
  bios_put_str("Trying to enable paging... ");
  ilias::i386::enable_paging(loader_pmap);
  bios_put_str("Succes!\n");
}

} /* namespace loader */
