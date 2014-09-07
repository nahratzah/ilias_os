#ifndef _ILIAS_VM_PAGE_H_
#define _ILIAS_VM_PAGE_H_

#include <cstddef>
#include <ilias/pmap/page.h>
#include <ilias/linked_list.h>

namespace ilias {
namespace vm {
namespace tags {

struct page_cache {};

} /* namespace ilias::vm::tags */


using namespace std;
using namespace ilias::pmap;


class page
: public linked_list_element<tags::page_cache>
{
 public:
  static constexpr size_t PAGE_SIZE = page_size(native_arch);
  static constexpr size_t PAGE_MASK = page_mask(native_arch);
  static constexpr size_t PAGE_SHIFT = page_shift(native_arch);

  using flags_type = uint32_t;

  static constexpr flags_type fl_cache_mask        = 0x03;

  static constexpr flags_type fl_cache_speculative = 0x01;
  static constexpr flags_type fl_cache_cold        = 0x02;
  static constexpr flags_type fl_cache_hot         = 0x03;
  static constexpr flags_type fl_cache_modify      = 0x04;
  static constexpr flags_type fl_cache_present     = 0x08;

  static constexpr flags_type fl_accessed          = 0x10;
  static constexpr flags_type fl_dirty             = 0x11;

  page() = delete;
  page(const page&) = delete;
  page& operator=(const page&) = delete;
  page(page_no<native_arch>) noexcept;

  page_no<native_arch> address() const noexcept { return pgno_; }

  flags_type get_flags() const noexcept;
  flags_type set_flag(flags_type) noexcept;
  flags_type clear_flag(flags_type) noexcept;
  pair<flags_type, bool> try_set_flag_iff_zero(flags_type) noexcept;
  flags_type set_flag_iff_zero(flags_type) noexcept;
  flags_type assign_masked_flags(flags_type, flags_type) noexcept;

  void update_accessed_dirty() {}  // XXX implement

 private:
  page_no<native_arch> pgno_;  // Address of this page.
  page_count<native_arch> nfree_;  // Number of free pages starting at this
                                   // page (only has meaning if the page is
                                   // actually free).

  atomic<flags_type> flags_;
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
