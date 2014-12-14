#ifndef _ILIAS_CYPTR_IMPL_EDGE_INL_H_
#define _ILIAS_CYPTR_IMPL_EDGE_INL_H_

#include <ilias/cyptr/impl/edge.h>
#include <cassert>
#include <ilias/cyptr/impl/basic_obj.h>

namespace ilias {
namespace cyptr {
namespace impl {


inline edge::edge(basic_obj& src) noexcept
: src_(src)
{
  src_.add_edge_(*this);
}

inline edge::~edge() noexcept {
  reset();
  src_.remove_edge_(*this);
}

inline void edge::lock() noexcept {
  dst_pointer::flags_type fl;

  do {
    fl = dst_.fetch_or(LOCKED, std::memory_order_acquire);
  } while (fl == LOCKED);
}

inline bool edge::try_lock() noexcept {
  dst_pointer::flags_type fl;

  fl = dst_.fetch_or(LOCKED, std::memory_order_acquire);
  return fl == UNLOCKED;
}

inline void edge::unlock() noexcept {
  dst_.fetch_and(UNLOCKED, std::memory_order_release);
}

inline basic_obj* edge::get() const noexcept {
  return std::get<0>(dst_.load(std::memory_order_relaxed));
}


}}} /* namespace ilias::cyptr::impl */

#endif /* _ILIAS_CYPTR_IMPL_EDGE_INL_H_ */
