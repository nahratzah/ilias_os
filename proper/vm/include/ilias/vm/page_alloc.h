#ifndef _ILIAS_VM_PAGE_ALLOC_H_
#define _ILIAS_VM_PAGE_ALLOC_H_

#include <ilias/vm/page.h>
#include <ilias/vm/page_cache.h>
#include <type_traits>
#include <mutex>
#include <ilias/linked_set.h>

namespace ilias {
namespace vm {


using namespace std;
using namespace ilias::pmap;


class page_alloc {
 private:
  struct ranges_cmp {
    bool operator()(const page& x, const page& y) const noexcept {
      return x.nfree_ < y.nfree_;
    }
  };

  using ranges_type = linked_set<page, tags::page_alloc, ranges_cmp>;

 public:
  enum class alloc_style : unsigned char {
    fail_ok = 0x00,
    fail_ok_nothrow = 0x01,
    fail_not_ok = 0x02,
  };

  struct spec;

  explicit page_alloc(stats_group&) noexcept;
  page_alloc(const page_alloc&) = delete;
  page_alloc& operator=(const page_alloc&) = delete;
  ~page_alloc() noexcept;

  page_ptr allocate(alloc_style);  // XXX return promise
  page_ptr allocate_urgent(alloc_style);
  page_list allocate(page_count<native_arch>, alloc_style);  // XXX return promise
  page_list allocate_urgent(page_count<native_arch>, alloc_style);
  page_list allocate(page_count<native_arch>, spec);  // XXX return promise
  page_list allocate_urgent(page_count<native_arch>, spec);

  void deallocate(page_list) noexcept;
  void deallocate(page_ptr) noexcept;

 private:
  page_ptr fetch_from_freelist_() noexcept;
  tuple<page_ptr, page_count<native_arch>>
      fetch_from_freelist_(page_count<native_arch>) noexcept;
  void add_to_freelist_(page_ptr, page_count<native_arch>) noexcept;

  stats_group cache_group_;

  mutex mtx_;
  ranges_type ranges_;
  page_cache cache_;
  page_count<native_arch> size_ = page_count<native_arch>(0);
  page_count<native_arch> free_ = page_count<native_arch>(0);
};

constexpr page_alloc::alloc_style operator&(const page_alloc::alloc_style& x,
                                            const page_alloc::alloc_style& y)
    noexcept {
  using int_type = typename underlying_type<page_alloc::alloc_style>::type;

  return static_cast<page_alloc::alloc_style>(static_cast<int_type>(x) &
                                              static_cast<int_type>(y));
}

constexpr page_alloc::alloc_style operator|(const page_alloc::alloc_style& x,
                                            const page_alloc::alloc_style& y)
    noexcept {
  using int_type = typename underlying_type<page_alloc::alloc_style>::type;

  return static_cast<page_alloc::alloc_style>(static_cast<int_type>(x) |
                                              static_cast<int_type>(y));
}

constexpr page_alloc::alloc_style operator^(const page_alloc::alloc_style& x,
                                            const page_alloc::alloc_style& y)
    noexcept {
  using int_type = typename underlying_type<page_alloc::alloc_style>::type;

  return static_cast<page_alloc::alloc_style>(static_cast<int_type>(x) ^
                                              static_cast<int_type>(y));
}

constexpr page_alloc::alloc_style operator~(const page_alloc::alloc_style& x)
    noexcept {
  using int_type = typename underlying_type<page_alloc::alloc_style>::type;

  return static_cast<page_alloc::alloc_style>(~static_cast<int_type>(x)) &
         (page_alloc::alloc_style::fail_ok_nothrow |
          page_alloc::alloc_style::fail_ok |
          page_alloc::alloc_style::fail_not_ok);
}

constexpr page_alloc::alloc_style alloc_fail_ok =
    page_alloc::alloc_style::fail_ok;
constexpr page_alloc::alloc_style alloc_fail_not_ok =
    page_alloc::alloc_style::fail_not_ok;
constexpr page_alloc::alloc_style alloc_fail_ok_nothrow =
    page_alloc::alloc_style::fail_ok_nothrow;


struct page_alloc::spec {
  page_count<native_arch> align = page_count<native_arch>(0);
  page_count<native_arch> offset = page_count<native_arch>(0);
  page_count<native_arch> boundary = page_count<native_arch>(0);
  alloc_style style = alloc_style::fail_ok;
};


}} /* namespace ilias::vm */

#endif /* _ILIAS_VM_PAGE_ALLOC_H_ */
