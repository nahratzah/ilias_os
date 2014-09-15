#ifndef _ILIAS_VM_PAGE_ALLOC_H_
#define _ILIAS_VM_PAGE_ALLOC_H_

#include <ilias/vm/page.h>
#include <ilias/vm/page_cache.h>
#include <type_traits>
#include <mutex>
#include <ilias/linked_set.h>
#include <ilias/promise.h>
#include <ilias/workq.h>

namespace ilias {
namespace vm {


using namespace std;
using namespace ilias::pmap;


class page_alloc
: public enable_shared_from_this<page_alloc>
{
 public:
  enum class alloc_style : unsigned char {
    fail_ok = 0x00,
    fail_ok_nothrow = 0x01,
    fail_not_ok = 0x02,
  };

  struct spec;

  page_alloc(workq_service&) noexcept;
  page_alloc(const page_alloc&) = delete;
  page_alloc& operator=(const page_alloc&) = delete;
  virtual ~page_alloc() noexcept;

  virtual future<page_refptr> allocate(alloc_style) = 0;
  virtual future<page_list> allocate(page_count<native_arch>, alloc_style) = 0;
  virtual future<page_list> allocate(page_count<native_arch>, spec) = 0;

 private:
  virtual void deallocate(page*) noexcept = 0;

 protected:
  workq_ptr get_workq() const noexcept { return wq_; }

 private:
  workq_ptr wq_;
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


class default_page_alloc
: public page_alloc
{
 private:
  struct ranges_cmp {
    bool operator()(const page& x, const page& y) const noexcept {
      return x.nfree_ < y.nfree_;
    }
  };

  using ranges_type = linked_set<page, tags::page_alloc, ranges_cmp>;

 public:
  default_page_alloc(stats_group&, workq_service&) noexcept;
  ~default_page_alloc() noexcept override;

  future<page_refptr> allocate(alloc_style) override;
 private:
  page_refptr allocate_prom_(alloc_style);

 public:
  page_refptr allocate_urgent(alloc_style);

  future<page_list> allocate(page_count<native_arch>, alloc_style) override;
 private:
  page_list allocate_prom_(page_count<native_arch>, alloc_style);

 public:
  page_list allocate_urgent(page_count<native_arch>, alloc_style);

  future<page_list> allocate(page_count<native_arch>, spec) override;
  page_list allocate_urgent(page_count<native_arch>, spec);

  void deallocate(page*) noexcept override;

 private:
  page_refptr fetch_from_freelist_() noexcept;
  page_range fetch_from_freelist_(page_count<native_arch>) noexcept;
  void add_to_freelist_(page*, page_count<native_arch>) noexcept;

  stats_group cache_group_;
  mutex mtx_;
  ranges_type ranges_;
  page_cache cache_;
  page_count<native_arch> size_ = page_count<native_arch>(0);
  page_count<native_arch> free_ = page_count<native_arch>(0);
};


}} /* namespace ilias::vm */

#endif /* _ILIAS_VM_PAGE_ALLOC_H_ */
