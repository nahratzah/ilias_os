#ifndef _ILIAS_VM_PAGE_H_
#define _ILIAS_VM_PAGE_H_

#include <cstddef>
#include <ilias/pmap/page.h>
#include <ilias/linked_list.h>
#include <atomic>
#include <functional>

namespace ilias {
namespace vm {
namespace tags {

struct page_cache {};
struct page_list {};

} /* namespace ilias::vm::tags */


using namespace std;
using namespace ilias::pmap;


class page;
class page_list;

using page_ptr = page*;  // XXX smart pointer?


class page
: public linked_list_element<tags::page_list>,
  public linked_list_element<tags::page_cache>
{
  friend page_list;

 public:
  using release_functor = function<bool(page_ptr, bool)>;  // XXX return promise, bool argument = delayed-ok
  using flags_type = uint32_t;

  static constexpr flags_type fl_cache_mask        = 0x03;
  static constexpr flags_type fl_cannot_free_mask  = 0xc0;

  static constexpr flags_type fl_cache_speculative = 0x01;
  static constexpr flags_type fl_cache_cold        = 0x02;
  static constexpr flags_type fl_cache_hot         = 0x03;
  static constexpr flags_type fl_cache_modify      = 0x04;
  static constexpr flags_type fl_cache_present     = 0x08;

  static constexpr flags_type fl_accessed          = 0x10;
  static constexpr flags_type fl_dirty             = 0x20;

  static constexpr flags_type fl_busy              = 0x40;
  static constexpr flags_type fl_wired             = 0x80;

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
  bool try_release_urgent() noexcept;
  void undirty() {}  // XXX implement

 private:
  page_no<native_arch> pgno_;  // Address of this page.
  page_count<native_arch> nfree_;  // Number of free pages starting at this
                                   // page (only has meaning if the page is
                                   // actually free or if the page is on a
                                   // page_list).

  atomic<flags_type> flags_;
  release_functor release_;
};


class page_list {
 private:
  using data_type = linked_list<page, tags::page_list>;

 public:
  using size_type = size_t;

  page_list() noexcept = default;
  page_list(const page_list&) = delete;
  page_list(page_list&&) noexcept;
  ~page_list() noexcept;

  page_list& operator=(const page_list&) = delete;
  page_list& operator=(page_list&&) noexcept;

  bool empty() const noexcept;
  page_count<native_arch> size() const noexcept;
  size_type n_blocks() const noexcept;

  void push(page_ptr) noexcept;
  void push_pages(page_ptr, page_count<native_arch>) noexcept;
  void push_pages_no_merge(page_ptr, page_count<native_arch>) noexcept;

  page_ptr pop() noexcept;
  pair<page_ptr, page_count<native_arch>> pop_pages() noexcept;

  void clear() noexcept;

  void sort_blocksize_ascending() noexcept;
  void sort_address_ascending(bool = true) noexcept;

 private:
  data_type data_;
  page_count<native_arch> size_ = page_count<native_arch>(0);
};


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
