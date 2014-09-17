#ifndef _ILIAS_VM_PAGE_OWNER_H_
#define _ILIAS_VM_PAGE_OWNER_H_

#include <ilias/vm/vm-fwd.h>
#include <cstdint>

namespace ilias {
namespace vm {


class page_owner {
 public:
  using offset_type = std::uintmax_t;

 protected:
  page_owner() noexcept = default;
  page_owner(const page_owner&) noexcept = default;
  page_owner(page_owner&&) noexcept = default;
  page_owner& operator=(const page_owner&) noexcept = default;
  page_owner& operator=(page_owner&&) noexcept = default;
  ~page_owner() noexcept = default;

 public:
  virtual page_refptr release_urgent(offset_type, page&);
  virtual void release_async(offset_type, page&);
  virtual void undirty_async(offset_type, page&);
};


}} /* namespace ilias::vm */

#endif /* _ILIAS_VM_PAGE_OWNER_H_ */
