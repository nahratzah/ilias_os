#ifndef _ILIAS_VM_PAGE_H_
#define _ILIAS_VM_PAGE_H_

#include <cstddef>
#include <ilias/pmap/page.h>
#include <ilias/linked_list.h>

namespace ilias {
namespace vm {
namespace tags {

struct free_list {};

} /* namespace ilias::vm::tags */


using namespace std;
using namespace ilias::pmap;


class page {
 public:
  static constexpr size_t PAGE_SIZE = page_size(native_arch);
  static constexpr size_t PAGE_MASK = page_mask(native_arch);
  static constexpr size_t PAGE_SHIFT = page_shift(native_arch);

  page() = delete;
  page(const page&) = delete;
  page& operator=(const page&) = delete;
  page(page_no<native_arch>) noexcept;

  page_no<native_arch> address() const noexcept { return pgno_; }

 private:
  page_no<native_arch> pgno_;  // Address of this page.
  page_count<native_arch> nfree_;  // Number of free pages starting at this
                                   // page (only has meaning if the page is
                                   // actually free).
};

using page_ptr = page*;


}} /* namespace ilias::vm */

namespace std {

template<>
struct hash<ilias::vm::page> {
  using result_type = size_t;
  using argument_type = ilias::vm::page;
  size_t operator()(const ilias::vm::page&) const noexcept;
};

} /* namespace std */

#include <ilias/vm/page-inl.h>

#endif /* _ILIAS_VM_PAGE_H_ */
