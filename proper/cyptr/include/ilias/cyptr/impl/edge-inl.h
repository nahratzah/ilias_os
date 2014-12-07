#ifndef _ILIAS_CYPTR_IMPL_EDGE_INL_H_
#define _ILIAS_CYPTR_IMPL_EDGE_INL_H_

#include <ilias/cyptr/impl/edge.h>
#include <cassert>

namespace ilias {
namespace cyptr {
namespace impl {


inline edge::edge(basic_obj& src) noexcept
: src_(src)
{
  src_.add_edge_(*this);
}

inline edge::~edge() noexcept {
  assert(dst_.load() == std::make_tuple(nullptr, UNLOCKED));
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

inline void edge::swap_same_src_(edge& x, edge& y) noexcept {
  assert(&x.src_ == &y.src_);

  std::unique_lock<edge> x_lock;
  std::unique_lock<edge> y_lock;

  if (&x < &y) {
    x_lock = std::unique_lock<edge>(x);
    y_lock = std::unique_lock<edge>(y);
  } else if (&y < &x) {
    y_lock = std::unique_lock<edge>(y);
    x_lock = std::unique_lock<edge>(x);
  }

  const auto x_ptr = std::get<0>(x.dst_.load(std::memory_order_relaxed));
  const auto y_ptr = std::get<0>(y.dst_.load(std::memory_order_relaxed));

  x.dst_.store(std::make_tuple(y_ptr, UNLOCKED));
  y.dst_.store(std::make_tuple(x_ptr, UNLOCKED));
  x_lock.release();
  y_lock.release();
}


}}} /* namespace ilias::cyptr::impl */

#endif /* _ILIAS_CYPTR_IMPL_EDGE_INL_H_ */
