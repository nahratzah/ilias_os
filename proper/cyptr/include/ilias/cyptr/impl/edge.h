#ifndef _ILIAS_CYPTR_IMPL_EDGE_H_
#define _ILIAS_CYPTR_IMPL_EDGE_H_

#include <ilias/cyptr/impl/fwd.h>
#include <ilias/cyptr/impl/tags.h>

namespace ilias {
namespace cyptr {
namespace impl {


class edge
: public linked_list_element<edge_objtag>
{
 public:
  explicit edge(const basic_obj&) noexcept;
  edge() = delete;
  edge(const edge&) = delete;
  edge(edge&&) = delete;
  edge& operator=(const edge&) = delete;
  edge& operator=(edge&&) = delete;
  ~edge() noexcept;

 protected:
  static void swap_same_src_(edge&, edge&) noexcept;

 private:
  const basic_obj& src_;
  const basic_obj* dst_ = nullptr;
};


}}} /* namespace ilias::cyptr::impl */

#include <ilias/cyptr/impl/edge-inl.h>

#endif /* _ILIAS_CYPTR_IMPL_EDGE_H_ */
