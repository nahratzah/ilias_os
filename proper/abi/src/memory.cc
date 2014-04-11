#include <memory>
#include <thread>
#include <abi/ext/log2.h>
#include <abi/memory.h>

_namespace_begin(std)


void* align(size_t alignment, size_t size, void*& ptr, size_t& space) {
  if (ptr == nullptr) return nullptr;
  if (size == 0) size = 1;
  if (alignment == 0) alignment = 1;
  if (!abi::ext::is_pow2(alignment)) return nullptr;
  if (size > space) return nullptr;

  const uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
  const uintptr_t aligned_addr = (addr + alignment - 1U) &
                                 ~uintptr_t(alignment);
  if (aligned_addr - addr > space - size) return nullptr;

  space -= size + (aligned_addr - addr);
  ptr = reinterpret_cast<void*>(aligned_addr + size);
  return reinterpret_cast<void*>(aligned_addr);
}


namespace impl {
namespace {

abi::big_heap& get_temporary_heap() noexcept {
  using abi::big_heap;

  static once_flag guard;
  static aligned_storage_t<sizeof(big_heap), alignof(big_heap)> data;

  void* data_ptr = &data;
  call_once(guard,
            [](void* ptr) { new (ptr) big_heap("temporary_buffer"); },
            data_ptr);
  return *static_cast<big_heap*>(data_ptr);
}

} /* namespace std::impl::<unnamed> */

pair<void*, size_t> temporary_buffer_allocate(size_t size, size_t align) {
  void* addr = get_temporary_heap().malloc(size, align);
  if (addr) abi::memzero(addr, size);
  return make_pair(addr, (addr ? size : 0));
}

void temporary_buffer_deallocate(const void* p) {
  get_temporary_heap().free(p);
}


} /* namespace std::impl */


_namespace_end(std)
