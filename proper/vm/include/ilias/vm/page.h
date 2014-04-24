#ifndef _ILIAS_VM_PAGE_H_
#define _ILIAS_VM_PAGE_H_

#include <cstddef>
#include <ilias/pmap/page.h>
#include <ilias/linked_list.h>

namespace ilias {
namespace vm {

template<arch Arch>
class page
: public linked_list_element<> {
 public:
  static constexpr size_t PAGE_SIZE = page_size(Arch);
  static constexpr size_t PAGE_MASK = page_mask(Arch);
  static constexpr size_t PAGE_SHIFT = page_shift(Arch);

  page() noexcept = default;
  page(const page&) = delete;
  page& operator=(const page&) = delete;
  page(page_no<Arch>) noexcept;

  page_no<Arch> address() const noexcept { return pgno_; }

 private:
  page_no<Arch> pgno_;  // Address of this page.
  page_no<Arch> free_;  // Number of free pages after this page.
};

}} /* namespace ilias::vm */

namespace std {

template<ilias::arch A>
struct hash<ilias::vm::page<A> {
  using result_type = size_t;
  using argument_type = ilias::vm::page<A>;
  size_t operator()(const ilias::vm::page<A>&) const noexcept;
};

} /* namespace std */

#endif /* _ILIAS_VM_PAGE_H_ */
