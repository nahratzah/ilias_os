#ifndef _ILIAS_CYPTR_IMPL_BASIC_OBJ_INL_H_
#define _ILIAS_CYPTR_IMPL_BASIC_OBJ_INL_H_

#include <cassert>
#include <ilias/cyptr/impl/basic_obj.h>
#include <ilias/cyptr/impl/generation.h>
#include <ilias/cyptr/impl/edge.h>

namespace ilias {
namespace cyptr {
namespace impl {


inline basic_obj::basic_obj() throw (std::bad_alloc)
: basic_obj(generation::new_generation())
{}

inline basic_obj::basic_obj(const basic_obj& o) noexcept
: basic_obj(o.gen_)
{}

inline basic_obj::basic_obj(basic_obj&& o) noexcept
: basic_obj(o.gen_)
{}

inline basic_obj& basic_obj::operator=(const basic_obj&) noexcept {
  return *this;
}

inline basic_obj& basic_obj::operator=(basic_obj&&) noexcept {
  return *this;
}

inline basic_obj::basic_obj(refpointer<generation> gen) noexcept
: gen_(std::move(gen))
{
  assert(this->gen_ != nullptr);
}

inline void basic_obj::add_edge_(edge& e) const noexcept {
  std::unique_lock<std::mutex> guard{ mtx_ };
  edge_list_.link_back(&e);
}

inline void basic_obj::remove_edge_(edge& e) const noexcept {
  std::unique_lock<std::mutex> guard{ mtx_ };
  edge_list_.unlink(&e);
}


}}} /* namespace ilias::cyptr::impl */

#endif /* _ILIAS_CYPTR_IMPL_BASIC_OBJ_INL_H_ */
