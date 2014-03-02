#include <cdecl.h>
#include <loader/abi.h>
#include <atomic>

/* Provided by linker. */
_cdecl_begin

extern char heap_start;
extern char heap_end;

_cdecl_end

namespace loader {

void* heap_malloc(_TYPES(size_t)* sz, _TYPES(size_t) min_sz) noexcept {
  static std::atomic<bool> allocated;
  auto heap_size = &heap_end - &heap_start;
  if (heap_size < min_sz) return nullptr;
  if (!sz) return nullptr;

  if (allocated.exchange(true, std::memory_order_relaxed)) return nullptr;
  *sz = &heap_end - &heap_start;
  return &heap_start;
}

} /* namespace loader */
