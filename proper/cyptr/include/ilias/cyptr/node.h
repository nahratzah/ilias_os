#ifndef _ILIAS_CYPTR_NODE_H_
#define _ILIAS_CYPTR_NODE_H_

#include <ilias/cyptr/impl/fwd.h>
#include <ilias/cyptr/impl/basic_obj.h>

namespace ilias {
namespace cyptr {


class cynode {
  template<typename> friend class edge;
  template<typename> friend class cyptr;

 protected:
  cynode() = default;  // May throw std::bad_alloc.
  cynode(const cynode&) noexcept;
  cynode(cynode&&) noexcept;
  cynode& operator=(const cynode&) noexcept;
  cynode& operator=(cynode&&) noexcept;
  ~cynode() noexcept;

 private:
  impl::basic_obj obj_;
};


}} /* namespace ilias::cyptr */

#include <ilias/cyptr/node-inl.h>

#endif /* _ILIAS_CYPTR_NODE_H_ */
