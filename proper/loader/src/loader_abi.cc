#include <cdecl.h>
#include <loader/abi.h>
#include <loader/x86_video.h>
#include <atomic>

/* Provided by linker. */
_cdecl_begin

extern char heap_start;
extern char heap_end;

_cdecl_end

namespace loader {
namespace {

std::atomic<bool> allocated;

} /* namespace loader::<unnamed> */


_namespace(std)::tuple<void*, _TYPES(size_t)> heap_malloc(
    _TYPES(size_t) min_sz) noexcept {
  using _namespace(std)::make_tuple;
  using _namespace(std)::get;

  const char* reason = "";  // Reason for failure.
  auto rv = _namespace(std)::make_tuple(&heap_start,
                                        size_t(&heap_end - &heap_start));
  auto& heap_size = get<1>(rv);
  if (heap_size < min_sz) {
    reason = "allocation larger than available heap";
    get<0>(rv) = nullptr;
  } else if (allocated.exchange(true, std::memory_order_relaxed)) {
    reason = "heap already handed out";
    get<0>(rv) = nullptr;
  }

  if (get<0>(rv) == nullptr) {
    bios_printf("loader::heap_malloc(%zu) -> NIL: %s\n", min_sz, reason);
  } else {
#ifndef NDEBUG
    bios_printf("loader::heap_malloc(%zu) -> (%p, %zu)\n",
                min_sz, get<0>(rv), get<1>(rv));
#endif
  }
  return rv;
}


} /* namespace loader */
