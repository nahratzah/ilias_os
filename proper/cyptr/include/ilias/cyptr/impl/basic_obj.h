#ifndef _ILIAS_CYPTR_IMPL_BASIC_OBJ_H_
#define _ILIAS_CYPTR_IMPL_BASIC_OBJ_H_

#include <atomic>
#include <cstdint>
#include <mutex>
#include <new>
#include <ilias/refcnt.h>
#include <ilias/llptr.h>
#include <ilias/linked_list.h>
#include <ilias/cyptr/impl/fwd.h>
#include <ilias/cyptr/impl/tags.h>
#include <ilias/cyptr/impl/tstamp.h>

namespace ilias {
namespace cyptr {
namespace impl {


tstamp get_generation_seq(const basic_obj&) noexcept;

void refcnt_acquire(const basic_obj&, std::uintptr_t = 1U) noexcept;
void refcnt_release(const basic_obj&, std::uintptr_t = 1U) noexcept;


class basic_obj
: public linked_list_element<basic_obj_gen_linktag>,
  public linked_list_element<wavefront_tag>
{
  friend basic_obj_lock;
  friend edge;
  friend generation;

  friend tstamp get_generation_seq(const basic_obj&) noexcept;
  friend void refcnt_acquire(const basic_obj&, std::uintptr_t) noexcept;
  friend void refcnt_release(const basic_obj&, std::uintptr_t) noexcept;

 private:
  using edge_list = linked_list<edge, edge_objtag>;

 public:
  basic_obj() throw (std::bad_alloc);

 protected:
  basic_obj(const basic_obj&) noexcept;
  basic_obj(basic_obj&&) noexcept;
  basic_obj& operator=(const basic_obj&) noexcept;
  basic_obj& operator=(basic_obj&&) noexcept;

 public:
  ~basic_obj() noexcept;

 private:
  basic_obj(refpointer<generation>) noexcept;

  void add_edge_(edge&) const noexcept;
  void remove_edge_(edge&) const noexcept;

  mutable llptr<generation> gen_;
  mutable std::mutex mtx_;
  mutable edge_list edge_list_;
  mutable std::atomic<obj_color> color_;
  mutable std::atomic<std::uintptr_t> refcnt_;
};


class basic_obj_lock {
 public:
  basic_obj_lock() noexcept = default;
  basic_obj_lock(const basic_obj_lock&) = delete;
  basic_obj_lock(basic_obj_lock&&) noexcept;
  basic_obj_lock& operator=(const basic_obj_lock&) = delete;
  basic_obj_lock& operator=(basic_obj_lock&&) noexcept;
  ~basic_obj_lock() noexcept;

  explicit basic_obj_lock(basic_obj&);
  basic_obj_lock(basic_obj&, std::defer_lock_t) noexcept;
  basic_obj_lock(basic_obj&, std::try_to_lock_t);
  basic_obj_lock(basic_obj&, std::adopt_lock_t);

  void lock();
  bool try_lock();
  generation_ptr unlock();

  explicit operator bool() const noexcept;
  basic_obj* get_obj_ptr() const noexcept;
  basic_obj& get_obj() const noexcept;
  generation& get_generation() const noexcept;
  generation_ptr get_generation_ptr() const noexcept;
  generation_ptr release() noexcept;

 private:
  basic_obj* obj_ = nullptr;
  generation_ptr gen_;
};


}}} /* namespace ilias::cyptr::impl */

#include <ilias/cyptr/impl/basic_obj-inl.h>

#endif /* _ILIAS_CYPTR_IMPL_BASIC_OBJ_H_ */
