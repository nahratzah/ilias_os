#ifndef _ABI_EXT_HEAP_INL_H_
#define _ABI_EXT_HEAP_INL_H_

#include <abi/ext/heap.h>

namespace __cxxabiv1 {
namespace ext {


inline auto heap::malloc_result(void* p, size_t sz) noexcept -> void* {
  stats_.malloc_calls.add();
  if (p)
    stats_.malloc_bytes.add(sz);
  else
    stats_.malloc_fail.add();
  return p;
}

inline auto heap::resize_result(_namespace(std)::tuple<bool, size_t> rv,
                         _namespace(std)::tuple<const void*, size_t> args)
    noexcept -> _namespace(std)::tuple<bool, size_t> {
  using _namespace(std)::get;

  auto& old_sz = get<1>(rv);
  auto& new_sz = get<1>(args);

  stats_.resize_calls.add();
  if (!get<0>(rv))
    stats_.resize_fail.add();
  else if (new_sz > old_sz)
    stats_.resize_bytes_up.add(new_sz - old_sz);
  else if (new_sz < old_sz)
    stats_.resize_bytes_down.add(old_sz - new_sz);
  return rv;
}

inline auto heap::free_result(size_t sz, const void* arg) noexcept -> void {
  stats_.free_calls.add();
  if (arg) stats_.free_bytes.add(sz);
}


}} /* namespace __cxxabiv1::ext */

#endif /* _ABI_EXT_HEAP_INL_H_ */
