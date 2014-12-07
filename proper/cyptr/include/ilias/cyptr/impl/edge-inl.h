#ifndef _ILIAS_CYPTR_IMPL_EDGE_INL_H_
#define _ILIAS_CYPTR_IMPL_EDGE_INL_H_

#include <ilias/cyptr/impl/edge.h>
#include <cassert>

namespace ilias {
namespace cyptr {
namespace impl {


inline edge::edge(const basic_obj& src) noexcept
: src_(src)
{
  src_.add_edge_(*this);
}

inline edge::~edge() noexcept {
  assert(dst_ == nullptr);
  src_.remove_edge_(*this);
}

inline void edge::swap_same_src_(edge& x, edge& y) noexcept {
  assert(&x.src_ == &y.src_);
  std::swap(x.dst_, y.dst_);
}


}}} /* namespace ilias::cyptr::impl */

#endif /* _ILIAS_CYPTR_IMPL_EDGE_INL_H_ */
