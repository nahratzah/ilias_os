#ifndef _ILIAS_CYPTR_IMPL_BASIC_OBJ_H_
#define _ILIAS_CYPTR_IMPL_BASIC_OBJ_H_

#include <mutex>
#include <new>
#include <ilias/refcnt.h>
#include <ilias/linked_list.h>
#include <ilias/cyptr/impl/fwd.h>
#include <ilias/cyptr/impl/tags.h>

namespace ilias {
namespace cyptr {
namespace impl {


class basic_obj
: public linked_list_element<basic_obj_gen_linktag>
{
  friend edge;

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

  mutable refpointer<generation> gen_;
  mutable std::mutex mtx_;
  mutable edge_list edge_list_;
};


}}} /* namespace ilias::cyptr::impl */

#include <ilias/cyptr/impl/basic_obj-inl.h>

#endif /* _ILIAS_CYPTR_IMPL_BASIC_OBJ_H_ */
