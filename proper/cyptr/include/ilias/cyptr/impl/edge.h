#ifndef _ILIAS_CYPTR_IMPL_EDGE_H_
#define _ILIAS_CYPTR_IMPL_EDGE_H_

#include <ilias/llptr.h>
#include <ilias/linked_list.h>
#include <ilias/cyptr/impl/fwd.h>
#include <ilias/cyptr/impl/tags.h>

namespace ilias {
namespace cyptr {
namespace impl {


class edge
: public linked_list_element<edge_objtag>
{
  friend generation;

 public:
  using dst_pointer = atomic_flag_ptr<basic_obj, 1>;

  static constexpr dst_pointer::flags_type LOCKED =
      dst_pointer::flags_type(0x1);
  static constexpr dst_pointer::flags_type UNLOCKED =
      dst_pointer::flags_type(0x0);

  explicit edge(basic_obj&) noexcept;
  edge() = delete;
  edge(const edge&) = delete;
  edge(edge&&) = delete;
  edge& operator=(const edge&) = delete;
  edge& operator=(edge&&) = delete;
  ~edge() noexcept;

  void lock() noexcept;
  bool try_lock() noexcept;
  void unlock() noexcept;

  void reset() noexcept;
  void reset(std::nullptr_t) noexcept { reset(); }
  void reset(basic_obj*) noexcept;
  basic_obj* get() const noexcept;

 private:
  basic_obj& src_;
  dst_pointer dst_;
};


}}} /* namespace ilias::cyptr::impl */

#include <ilias/cyptr/impl/edge-inl.h>

#endif /* _ILIAS_CYPTR_IMPL_EDGE_H_ */
