#ifndef _ILIAS_CYPTR_IMPL_BASIC_OBJ_INL_H_
#define _ILIAS_CYPTR_IMPL_BASIC_OBJ_INL_H_

#include <cdecl.h>
#include <bitset>
#include <cassert>
#include <functional>
#include <tuple>
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
: basic_obj(std::get<0>(o.gen_.load(std::memory_order_relaxed)))
{}

inline basic_obj::basic_obj(basic_obj&& o) noexcept
: basic_obj(std::get<0>(o.gen_.load(std::memory_order_relaxed)))
{}

inline basic_obj& basic_obj::operator=(const basic_obj&) noexcept {
  return *this;
}

inline basic_obj& basic_obj::operator=(basic_obj&&) noexcept {
  return *this;
}

inline basic_obj::basic_obj(refpointer<generation> gen) noexcept
: gen_(std::make_tuple(std::cref(gen), std::bitset<0>()))
{
  assert(gen != nullptr);
  gen->register_obj(*this);
}

inline bool basic_obj::has_generation(const generation& g) const noexcept {
  return &g == std::get<0>(gen_.load_no_acquire(std::memory_order_relaxed));
}

inline generation_ptr basic_obj::get_generation() const noexcept {
  return std::get<0>(gen_.load(std::memory_order_relaxed));
}

inline void basic_obj::add_edge_(edge& e) const noexcept {
  std::unique_lock<std::mutex> guard{ mtx_ };
  edge_list_.link_back(&e);
}

inline void basic_obj::remove_edge_(edge& e) const noexcept {
  std::unique_lock<std::mutex> guard{ mtx_ };
  edge_list_.unlink(&e);
}


inline basic_obj_lock::basic_obj_lock(basic_obj_lock&& o) noexcept
: obj_(std::exchange(o.obj_, nullptr)),
  gen_(std::exchange(o.gen_, nullptr))
{}

inline basic_obj_lock& basic_obj_lock::operator=(basic_obj_lock&& o) noexcept {
  using std::swap;

  basic_obj_lock tmp{ std::move(*this) };
  swap(obj_, o.obj_);
  swap(gen_, o.gen_);
  return *this;
}

inline basic_obj_lock::~basic_obj_lock() noexcept {
  if (gen_ != nullptr) unlock();
}

inline basic_obj_lock::basic_obj_lock(basic_obj& o)
: basic_obj_lock(o, std::defer_lock)
{
  lock();
}

inline basic_obj_lock::basic_obj_lock(basic_obj& o, std::defer_lock_t)
    noexcept
: obj_(&o)
{}

inline basic_obj_lock::basic_obj_lock(basic_obj& o, std::try_to_lock_t)
    noexcept
: basic_obj_lock(o, std::defer_lock)
{
  try_lock();
}

inline basic_obj_lock::basic_obj_lock(basic_obj& o, std::adopt_lock_t) noexcept
: obj_(&o),
  gen_(o.get_generation())
{}

inline basic_obj_lock::operator bool() const noexcept {
  return gen_ != nullptr;
}

inline auto basic_obj_lock::get_obj_ptr() const noexcept -> basic_obj* {
  return obj_;
}

inline auto basic_obj_lock::get_obj() const noexcept -> basic_obj& {
  assert(obj_ != nullptr);
  return *get_obj_ptr();
}

inline auto basic_obj_lock::get_generation() const noexcept -> generation& {
  assert(gen_ != nullptr);
  return *gen_;
}

inline auto basic_obj_lock::get_generation_ptr() const noexcept ->
    generation_ptr {
  assert(gen_ != nullptr);
  return gen_;
}

inline auto basic_obj_lock::release() noexcept -> generation_ptr {
  assert(gen_ != nullptr);
  return std::exchange(gen_, nullptr);
}


}}} /* namespace ilias::cyptr::impl */

#endif /* _ILIAS_CYPTR_IMPL_BASIC_OBJ_INL_H_ */
