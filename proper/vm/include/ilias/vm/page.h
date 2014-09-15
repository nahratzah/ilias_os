#ifndef _ILIAS_VM_PAGE_H_
#define _ILIAS_VM_PAGE_H_

#include <cstddef>
#include <ilias/pmap/page.h>
#include <ilias/linked_list.h>
#include <ilias/linked_set.h>
#include <atomic>
#include <functional>
#include <ilias/vm/vm-fwd.h>
#include <ilias/refcnt.h>

namespace ilias {
namespace vm {
namespace tags {

struct page_cache {};
struct page_list {};
struct page_alloc {};

} /* namespace ilias::vm::tags */


using namespace std;
using namespace ilias::pmap;


using page_refptr = refpointer<page>;

class page
: public linked_list_element<tags::page_list>,
  public linked_list_element<tags::page_cache>,
  public linked_set_element<page, tags::page_alloc>,
  public refcount_base<page>
{
  friend page_list;
  friend page_alloc;

 public:
  using release_functor = function<page_refptr(page_refptr, bool)>;  // XXX return promise, bool argument = delayed-ok
  using flags_type = uint32_t;

  static constexpr flags_type fl_cache_speculative = 0x001;
  static constexpr flags_type fl_cache_cold        = 0x002;
  static constexpr flags_type fl_cache_hot         = 0x003;
  static constexpr flags_type fl_cache_modify      = 0x004;
  static constexpr flags_type fl_cache_present     = 0x008;

  static constexpr flags_type fl_accessed          = 0x010;
  static constexpr flags_type fl_dirty             = 0x020;

  static constexpr flags_type fl_busy              = 0x040;
  static constexpr flags_type fl_wired             = 0x080;

  static constexpr flags_type fl_free              = 0x100;


  static constexpr flags_type fl_cache_mask        = fl_cache_speculative |
                                                     fl_cache_cold |
                                                     fl_cache_hot;
  static constexpr flags_type fl_cannot_free_mask  = fl_busy | fl_wired;

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
  page_refptr try_release_urgent() noexcept;
  void undirty() {}  // XXX implement

  page_count<native_arch> nfree_;  // Number of free pages starting at this
                                   // page (only has meaning if the page is
                                   // actually free).
                                   // Public, so allocators can mess with it.

 private:
  page_no<native_arch> pgno_;  // Address of this page.
  page_count<native_arch> npgl_;  // Number of pages starting at this page
                                  // (only has meaning if the page is on a
                                  // page_list).

  mutable atomic<size_t> refcnt_{ 0 };

  atomic<flags_type> flags_;
  release_functor release_;
  shared_ptr<page_alloc> pga_owner_;
};


class page_range {
  friend page_list;

 public:
  using value_type = page;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = page_refptr;
  using const_pointer = refpointer<const value_type>;
  using iterator = value_type*;
  using const_iterator = const value_type*;
  using size_type = size_t;
  using difference_type = ptrdiff_t;

  page_range(page*, size_type, bool = true) noexcept;
  page_range() noexcept = default;
  page_range(const page_range&) noexcept;
  page_range(page_range&&) noexcept;
  explicit page_range(page_refptr) noexcept;
  ~page_range() noexcept;

  page_range& operator=(page_range) noexcept;
  bool operator==(const page_range&) const noexcept;
  bool operator!=(const page_range&) const noexcept;

  reference operator[](size_type) noexcept;
  const_reference operator[](size_type) const noexcept;
  reference at(size_type);
  const_reference at(size_type) const;

  reference front() noexcept;
  const_reference front() const noexcept;
  reference back() noexcept;
  const_reference back() const noexcept;
  pointer pop_front() noexcept;
  pointer pop_back() noexcept;

  bool empty() const noexcept;
  size_type size() const noexcept;
  void clear() noexcept;
  pair<page*, size_type> release() noexcept;

  iterator begin() noexcept;
  iterator end() noexcept;
  const_iterator begin() const noexcept;
  const_iterator end() const noexcept;
  const_iterator cbegin() const noexcept;
  const_iterator cend() const noexcept;

  friend void swap(page_range&, page_range&) noexcept;

 private:
  page* pg_ = nullptr;
  size_type npg_ = 0;
};


class page_list {
 private:
  using data_type = linked_list<page, tags::page_list>;

  class proxy;

 public:
  using value_type = page_range;
  class iterator;
  using const_iterator = iterator;
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

  void push_front(page_refptr) noexcept;
  void push_back(page_refptr) noexcept;
  void push_pages_front(page_range) noexcept;
  void push_pages_back(page_range) noexcept;

  page_refptr pop_front() noexcept;
  page_refptr pop_back() noexcept;
  page_range pop_pages_front() noexcept;
  page_range pop_pages_back() noexcept;

  void clear() noexcept;

  void sort_blocksize_ascending() noexcept;
  void sort_address_ascending(bool = true) noexcept;
  void merge(page_list&&, bool = true) noexcept;
  void merge_adjecent_entries() noexcept;

  const_iterator begin() const noexcept;
  const_iterator end() const noexcept;
  const_iterator cbegin() const noexcept;
  const_iterator cend() const noexcept;

 private:
  data_type data_;
  page_count<native_arch> size_ = page_count<native_arch>(0);
};

class page_list::proxy {
  friend page_list::iterator;

 private:
  explicit proxy(const page& pg) noexcept;

 public:
  proxy(const proxy& o) noexcept : r_(o.r_) {}
  proxy(proxy&& o) noexcept : r_(move(o.r_)) {}
  auto operator->() const noexcept -> const page_range* { return &r_; }

 private:
  page_range r_;
};

class page_list::iterator
: public std::iterator<
      iterator_traits<data_type::const_iterator>::iterator_category,
      value_type,
      iterator_traits<data_type::const_iterator>::difference_type,
      proxy,
      const page_range>
{
  friend page_list;

 private:
  explicit iterator(data_type::const_iterator impl) noexcept : impl_(impl) {}

 public:
  iterator() = default;
  iterator(const iterator&) = default;
  iterator(iterator&&) = default;
  iterator& operator=(const iterator&) = default;
  iterator& operator=(iterator&&) = default;

  iterator& operator++() noexcept;
  iterator operator++(int) noexcept;
  iterator& operator--() noexcept;
  iterator operator--(int) noexcept;

  bool operator==(const iterator& o) const noexcept;
  bool operator!=(const iterator& o) const noexcept;

  reference operator*() const noexcept;
  proxy operator->() const noexcept;

 private:
  data_type::const_iterator impl_;
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
