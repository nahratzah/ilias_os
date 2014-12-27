#ifndef _ILIAS_VM_PAGE_CACHE_H_
#define _ILIAS_VM_PAGE_CACHE_H_

#include <ilias/vm/page.h>
#include <ilias/stats.h>
#include <atomic>
#include <mutex>
#include <ilias/linked_list.h>
#include <ilias/future.h>
#include <memory>
#include <ilias/ll_list.h>
#include <ilias/workq.h>

namespace ilias {
namespace vm {


class page_cache {
 private:
  using list_type = ll_smartptr_list<page, tags::page_cache,
                                     ll_list_detail::no_acqrel>;

  static constexpr unsigned int n_zones = 3;
  static constexpr unsigned int spec_zone = 0;  // Speculative zone.
  static constexpr unsigned int cold_zone = 1;
  static constexpr unsigned int hot_zone  = 2;
  static constexpr array<page::flags_type, n_zones> pg_flags = {{
      page::fl_cache_speculative, page::fl_cache_cold, page::fl_cache_hot
  }};

  class hot_cold_rebalance_wqjob;
  class spec_need_rebalance_wqjob;

 public:
  page_cache(stats_group&, workq_ptr);
  page_cache(stats_group&, workq_service&);
  page_cache(const page_cache&) = delete;
  page_cache(page_cache&&) = delete;
  page_cache& operator=(const page_cache&) = delete;
  page_cache& operator=(page_cache&&) = delete;
  ~page_cache() noexcept;

  bool empty() const noexcept;

  void manage(const page_ptr&, bool) noexcept;
  void unmanage(const page_ptr&) noexcept;

  page_list try_release_urgent(page_count<native_arch>) noexcept;
  future<page_list> try_release(page_count<native_arch>) noexcept;

 private:
  bool unlink_zone_(unsigned int, page&) noexcept;
  bool link_zone_(unsigned int, page&) noexcept;
  bool promote_(unsigned int, page&) noexcept;
  bool demote_(unsigned int, page&) noexcept;

  page_list try_release_urgent_zone_(unsigned int zone,
                                     page_count<native_arch> npg,
                                     page_list pgl) noexcept;
  future<page_list> try_release_zone_(unsigned int,
                                      page_count<native_arch> npg) noexcept;

  atomic<intptr_t> hot_cold_diff_{ 0 };  // #hot - (#cold + #speculative).
  atomic<intptr_t> spec_need_diff_{ 0 };  // #speculative - min(#speculative)
  array<list_type, n_zones> data_;

  workq_job_ptr hot_cold_rebalance_job_;
  workq_job_ptr spec_need_rebalance_job_;

  stats_counter manage_,
                unmanage_,
                hot_cold_rebalance_,
                spec_need_rebalance_,
                speculative_hit_,
                speculative_miss_;
};


}} /* namespace ilias::vm */

#endif /* _ILIAS_VM_PAGE_CACHE_H_ */
