#include <stdimpl/allocator_cloneable.h>
#include <cdecl.h>
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <abi/ext/log2.h>

_namespace_begin(std)
namespace impl {


allocator_cloneable::~allocator_cloneable() noexcept {}


namespace {

template<typename T>
constexpr T roundup(const T& x, const T& r) noexcept {
  return (r <= T(1) ? x : (x + r - T(1)) & ~(r - T(1)));
}

} /* namespace std::impl::<unnamed> */

size_t allocator_cloneable::alloc_size_(size_t alloc_size, size_t alloc_align)
    const noexcept {
  using ::abi::ext::is_pow2;

  assert_msg(is_pow2(constraints_.align),
             "Alignment constraint must be a power-of-2.");
  assert_msg(is_pow2(alloc_align),
             "Allocator alignment constraint must be a power-of-2.");
  static_assert(is_pow2(alignof(alloc_type_)),
                "Internally used allocation type must be a power-of-2.");

  const size_t align = max({ constraints_.align, alloc_align, size_t(1) });
  const size_t space = roundup(alloc_size, align) +
                       constraints_.size +
                       align - 1U;
  return roundup(space, sizeof(alloc_type_));
}


void allocator_cloneable::deleter::operator()(allocator_cloneable* p)
    const noexcept {
  assert(p != nullptr);

  p->~allocator_cloneable();
  cleanup_fn_(apply_off(p, base_off_), apply_off(p, alloc_off_));
}

auto allocator_cloneable::deleter::calc_off(const void* dst, const void* src)
    noexcept -> offset_type {
  assert(dst != nullptr && src != nullptr);

  ptrdiff_t off = static_cast<const uint8_t*>(dst) -
                  static_cast<const uint8_t*>(src);
  assert_msg(offset_type(off) == off,
             "Offset does not fit the integer type assigned to hold it.");
  return offset_type(off);
}

auto allocator_cloneable::deleter::apply_off(const void* src, offset_type off)
    noexcept -> void* {
  if (src == nullptr) return nullptr;
  const void* rv = static_cast<const uint8_t*>(src) + off;
  return const_cast<void*>(rv);
}


} /* namespace std::impl */
_namespace_end(std)
