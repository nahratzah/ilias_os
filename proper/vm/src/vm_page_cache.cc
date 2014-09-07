#include <ilias/vm/page_cache.h>
#include <tuple>

namespace ilias {
namespace vm {
namespace {


class cache_page_lock {
 public:
  cache_page_lock() = delete;
  cache_page_lock(const cache_page_lock&) = delete;
  cache_page_lock& operator=(const cache_page_lock&) = delete;

  cache_page_lock(page& pg) noexcept
  : pg_(pg)
  {
    lock();
  }

  cache_page_lock(page& pg, try_to_lock_t) noexcept
  : pg_(pg)
  {
    try_lock();
  }

  cache_page_lock(page& pg, defer_lock_t) noexcept
  : pg_(pg),
    pgfl_(pg.get_flags())
  {}

  ~cache_page_lock() noexcept {
    if (locked_) unlock();
  }

  void lock() noexcept {
    assert(!owns_lock());
    pgfl_ = pg_.set_flag_iff_zero(page::fl_cache_modify);
    locked_ = true;
  }

  bool try_lock() noexcept {
    assert(!owns_lock());
    tie(pgfl_, locked_) = pg_.try_set_flag_iff_zero(page::fl_cache_modify);
    return owns_lock();
  }

  void unlock() noexcept {
    assert(owns_lock());
    pg_.clear_flag(page::fl_cache_modify);
    locked_ = false;
  }

  bool owns_lock() const noexcept { return bool(*this); }
  explicit operator bool() const noexcept { return locked_; }
  page::flags_type flags() const noexcept { return pgfl_; }

 private:
  page& pg_;
  bool locked_ = false;
  page::flags_type pgfl_ = 0;
};


} /* namespace ilias::vm::<unnamed> */


page_cache::page_cache(stats_group& group) noexcept
: manage_(group, "manage"),
  unmanage_(group, "unmanage"),
  rebalance_(group, "rebalance"),
  speculative_hit_(group, "speculative_hit"),
  speculative_miss_(group, "speculative_miss")
{}

page_cache::~page_cache() noexcept {}

auto page_cache::notify_page_access(page_ptr pg) noexcept -> void {
  assert(pg != nullptr);

  cache_page_lock l{ *pg, try_to_lock };
  if (!l.owns_lock() || !(l.flags() & page::fl_cache_present)) return;

  pg->update_accessed_dirty();
  pg->clear_flag(page::fl_accessed);

  const page::flags_type queue = (l.flags() & page::fl_cache_mask);
  page::flags_type dst_queue;
  switch (queue) {
  default:
    return;
  case page::fl_cache_speculative:
    speculative_.unlink(&*pg);
    cold_.link_front(&*pg);
    dst_queue = page::fl_cache_cold;
    break;
  case page::fl_cache_cold:
    {
      auto diff = hot_cold_diff_.fetch_add(2, memory_order_relaxed) + 2;
      if (diff < -1 || diff > 1) {
        /* XXX notify rebalance job */
      }
    }
    cold_.unlink(&*pg);
    hot_.link_front(&*pg);
    dst_queue = page::fl_cache_hot;
    break;
  case page::fl_cache_hot:
    hot_.unlink(&*pg);
    hot_.link_front(&*pg);
    dst_queue = page::fl_cache_hot;
    break;
  }

  pg->assign_masked_flags(dst_queue, page::fl_cache_mask);
}

auto page_cache::rebalance_job() noexcept -> bool {
  rebalance_.add();  // Update stats.

  auto diff = hot_cold_diff_.load(memory_order_relaxed);
  if (diff > 1) {
    /*
     * hot - cold > 1, demote a page from hot to cold zone.
     */
    assert(!hot_.empty());
    lock_guard<mutex> hl{ hot_guard_ };

    page* looped = nullptr;
    for (list_type::reverse_iterator i_next, i = hot_.rbegin();
         i != hot_.rend() && &*i != looped;
         i = i_next) {
      i_next = next(i);

      i->update_accessed_dirty();
      cache_page_lock l{ *i, try_to_lock };
      if (l) {
        page& pg = *i;
        if (l.flags() & page::fl_accessed) {
          hot_.unlink(&pg);
          pg.clear_flag(page::fl_accessed);
          hot_.link_front(&pg);

          if (looped == nullptr) looped = &pg;
        } else {
          unique_lock<mutex> cl{ cold_guard_, try_to_lock };

          if (cl) {
            hot_.unlink(&pg);
            diff = hot_cold_diff_.fetch_sub(2, memory_order_relaxed) - 2;
            cold_.link_front(&pg);
            break;
          }
        }
      }
    }
  } else if (diff < -1) {
    /*
     * hot - cold < -1, promote a page from cold to hot zone.
     */
    assert(!cold_.empty());
    lock_guard<mutex> cl{ cold_guard_ };

    for (list_type::iterator i = cold_.begin(); i != cold_.end(); ++i) {
      i->update_accessed_dirty();
      cache_page_lock l{ *i, try_to_lock };
      if (l) {
        lock_guard<mutex> hl{ hot_guard_ };

        page& pg = *i;
        cold_.unlink(&pg);
        pg.clear_flag(page::fl_accessed);
        if (l.flags() & page::fl_accessed)
          hot_.link_front(&pg);
        else
          hot_.link_back(&pg);
        diff = hot_cold_diff_.fetch_add(2, memory_order_relaxed) + 2;
        break;
      }
    }
  } else {
    return false;
  }

  /*
   * Scan a page in the speculative range.
   */
  {
    lock_guard<mutex> sl{ speculative_guard_ };
    for (list_type::reverse_iterator i_next, i = speculative_.rbegin();
         i != speculative_.rend();
         i = i_next) {
      i_next = next(i);

      page& pg = *i;
      pg.update_accessed_dirty();

      cache_page_lock l{ pg, try_to_lock };
      if (l && (l.flags() & page::fl_accessed)) {
        lock_guard<mutex> cl{ cold_guard_ };
        speculative_.unlink(&pg);
        cold_.link_front(&pg);

        speculative_hit_.add();  // Update stats.
        break;
      }
    }
  }

  return diff < -1 || diff > 1;
}

auto page_cache::manage(page_ptr pg, bool speculative) noexcept -> void {
  if (manage_internal_(pg, speculative) && rebalance_job()) {
    // XXX activate rebalance job
  }
}

auto page_cache::unmanage(page_ptr pg) noexcept -> void {
  if (unmanage_internal_(pg) && rebalance_job()) {
    // XXX activate rebalance job
  }
}

auto page_cache::manage_internal_(page_ptr pg, bool speculative) noexcept ->
    bool {
  cache_page_lock l{ *pg };
  assert(!(l.flags() & page::fl_cache_present));

  /*
   * Mark page as being in the cache, clear its access bit.
   */
  pg->update_accessed_dirty();
  pg->assign_masked_flags(((speculative ?
                            page::fl_cache_speculative :
                            page::fl_cache_cold) |
                           page::fl_cache_present),
                          (page::fl_cache_mask | page::fl_cache_present |
                           page::fl_accessed));

  if (speculative) {
    lock_guard<mutex> cl{ speculative_guard_ };
    speculative_.link_front(&*pg);
  } else {
    lock_guard<mutex> cl{ cold_guard_ };
    cold_.link_front(&*pg);
  }
  auto diff = hot_cold_diff_.fetch_sub(1, memory_order_relaxed) - 1;

  manage_.add();  // Update stats.

  return diff < -1;
}

auto page_cache::unmanage_internal_(page_ptr pg) noexcept -> bool {
  cache_page_lock l{ *pg };
  assert(l.flags() & page::fl_cache_present);

  bool fire;
  switch (l.flags() & page::fl_cache_mask) {
  default:
    assert_msg(false, "Page cache flags are wrong");
    __builtin_unreachable();
  case page::fl_cache_speculative:
    {
      /* Update speculative hit/miss statistic before page goes away. */
      if (l.flags() & page::fl_accessed) {
        speculative_hit_.add();
      } else {
        pg->update_accessed_dirty();
        if (pg->get_flags() & page::fl_accessed)
          speculative_hit_.add();
        else
          speculative_miss_.add();
      }

      lock_guard<mutex> cl{ speculative_guard_ };
      speculative_.unlink(&*pg);
      fire = (hot_cold_diff_.fetch_add(1, memory_order_relaxed) + 1 > 1);
    }
  case page::fl_cache_cold:
    {
      lock_guard<mutex> cl{ cold_guard_ };
      cold_.unlink(&*pg);
      fire = (hot_cold_diff_.fetch_add(1, memory_order_relaxed) + 1 > 1);
    }
    break;
  case page::fl_cache_hot:
    {
      lock_guard<mutex> hl{ hot_guard_ };
      hot_.unlink(&*pg);
      fire = (hot_cold_diff_.fetch_sub(1, memory_order_relaxed) - 1 < -1);
    }
    break;
  }
  pg->clear_flag(page::fl_cache_present | page::fl_cache_mask);

  unmanage_.add();  // Update stats.

  return fire;
}


}} /* namespace ilias::vm */
