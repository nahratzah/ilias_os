#ifndef _ILIAS_VM_PAGE_CACHE_H_
#define _ILIAS_VM_PAGE_CACHE_H_

#include <ilias/vm/page.h>
#include <ilias/stats.h>
#include <atomic>
#include <mutex>
#include <ilias/linked_list.h>

namespace ilias {
namespace vm {


class page_cache {
 private:
  using list_type = linked_list<page, tags::page_cache>;  // XXX make atomic linked list

 public:
  page_cache(stats_group&) noexcept;
  page_cache(const page_cache&) = delete;
  page_cache(page_cache&&) = delete;
  page_cache& operator=(const page_cache&) = delete;
  page_cache& operator=(page_cache&&) = delete;
  ~page_cache() noexcept;

  void notify_page_access(page_ptr) noexcept;
  bool rebalance_job() noexcept;

  void manage(page_ptr, bool) noexcept;
  void unmanage(page_ptr) noexcept;

  page_list try_release_urgent(page_count<native_arch>) noexcept;
  void undirty(page_count<native_arch>) noexcept;  // XXX this is inefficient, consider another interface

 private:
  bool manage_internal_(page_ptr, bool) noexcept;
  bool unmanage_internal_(page_ptr) noexcept;

  list_type hot_, cold_, speculative_;
  atomic<intptr_t> hot_cold_diff_{ 0 };  // #hot_ - (#cold_ + #speculative_).
  mutex speculative_guard_;
  mutex cold_guard_;
  mutex hot_guard_;

  stats_counter manage_,
                unmanage_,
                rebalance_,
                speculative_hit_,
                speculative_miss_;
};


}} /* namespace ilias::vm */

#endif /* _ILIAS_VM_PAGE_CACHE_H_ */
