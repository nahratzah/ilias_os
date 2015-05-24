#include <loader/main.h>
#include <loader/x86_video.h>
#include <loader/ldexport_init.h>
#include <loader/page.h>
#include <ilias/pmap/page.h>
#include <ilias/pmap/consts.h>
#include <ilias/cpuid.h>
#include <ilias/pmap/pmap_i386.h>
#include <ilias/i386/gdt.h>
#include <ilias/i386/paging.h>
#include <mutex>
#include <array>
#include <string>

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
  bios_printf("Mapping %#llx - %#llx\n",
              static_cast<unsigned long long>(base),
              static_cast<unsigned long long>(limit));

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
  bios_printf("Punch out loader: %#llx - %#llx\n",
              static_cast<unsigned long long>(start),
              static_cast<unsigned long long>(end));

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

  bios_put_str("Setting up loader pmap ");
  static constexpr std::array<std::string_ref, 4> windmill{{
    std::string_ref("\b-", 2),
    std::string_ref("\b\\", 2),
    std::string_ref("\b|", 2),
    std::string_ref("\b/", 2),
  }};

  const auto end = vaddr<native_arch>(
      round_page_up(reinterpret_cast<uintptr_t>(&kernel_end) - 1U,
                    native_arch));
  for (vpage_no<native_arch> start = vaddr<native_arch>(
           round_page_down(reinterpret_cast<uintptr_t>(&kernel_start),
                           native_arch));
       start != end;
       ++start) {
    bios_put_str(windmill[start.get() % windmill.size()]);
    auto pg = page_no<native_arch>(start.get());  // One-to-one mapping.
    loader_pmap.map(start, pg, permission::RWX());
  }
  bios_put_str("\b \b\n");
}

/* Map vram into pmap. */
void setup_loader_vram(ilias::pmap::pmap<ilias::native_arch>& loader_pmap) {
  using ilias::pmap::vaddr;
  using ilias::pmap::vpage_no;
  using ilias::pmap::page_no;
  using ilias::pmap::round_page_down;
  using ilias::pmap::round_page_up;
  using ilias::pmap::permission;
  using ilias::native_arch;

  uintptr_t vram_begin, vram_end;
  std::tie(vram_begin, vram_end) = vram_ram();
  bios_put_str("Mapping vram:");

  const auto end = vaddr<native_arch>(round_page_up(vram_end - 1U,
                                                    native_arch));
  for (vpage_no<native_arch> start =
           vaddr<native_arch>(round_page_down(vram_begin, native_arch));
       start != end;
       ++start) {
    auto pg = page_no<native_arch>(start.get());  // One-to-one mapping.
    bios_printf(
        " %#llx->%#llx",
        static_cast<unsigned long long>(vaddr<native_arch>(start).get()),
        static_cast<unsigned long long>(ilias::pmap::phys_addr<native_arch>(pg).get()));
    /* XXX: we map the video memory as executable,
     * since we get page faults without;
     * doesn't look like a bug in the pmap code, maybe it's a qemu thing?  */
    loader_pmap.map(start, pg, permission::RW() | permission::UNCACHED());
  }
  bios_put_str(" DONE\n");
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
  bios_printf("CPU flags: %s\n",
              to_string(ilias::cpuid_features()).c_str());
  bios_printf("    extended flags: %s\n",
              to_string(ilias::cpuid_extfeatures()).c_str());

  /* Initialize export data. */
  ldexport& lde = ldexport_get();
  bios_put_str(lde.to_string());

  /* Initialize our page allocator. */
  page_allocator<ilias::native_arch>& pga = get_pga();
  uintptr_t lim = 2 * reinterpret_cast<uintptr_t>(&kernel_end);
  if (lim < 32 * 1024 * 1024) lim = 32 * 1024 * 1024;
  setup_page_allocator(pga, lde, 1024 * 1024, lim);
  punch_loader(pga);

  /* Print out page allocator state. */
  bios_printf("Loader page allocator: %lld (%lld used, %lld free)\n",
              static_cast<unsigned long long>(pga.size()),
              static_cast<unsigned long long>(pga.used_size()),
              static_cast<unsigned long long>(pga.free_size()));

  /* Create pmap for loader. */
  ilias::pmap::pmap<ilias::native_arch>& loader_pmap = get_pmap(pga);
  setup_loader_pmap(loader_pmap);
  setup_loader_vram(loader_pmap);

  /* Enable paging. */
  bios_put_str("Trying to enable paging... ");
  ilias::i386::enable_paging(ilias::i386::gdt, loader_pmap);
  bios_put_str("Succes!\n");
}

} /* namespace loader */
