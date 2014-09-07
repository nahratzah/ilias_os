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
    for (list_type::iterator i_next = hot_.end();
         i_next != hot_.begin() && &*prev(i_next) != looped;
         --i_next) {
      const list_type::iterator i = prev(i);

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

    for (list_type::iterator i_next, i = cold_.begin();
         i != cold_.end();
         i = i_next) {
      i_next = next(i);

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
    for (list_type::iterator i_next = speculative_.end();
         i_next != speculative_.begin();
         --i_next) {
      const list_type::iterator i = prev(i_next);

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

auto page_cache::try_release_urgent(page_count<native_arch> npg) noexcept ->
    page_list {
  page_list result;

  /*
   * Claim speculatively loaded pages.
   * Pages that are actually accessed, are promoted.
   */
  if (result.size() < npg) {
    lock_guard<mutex> sl{ speculative_guard_ };
    for (list_type::iterator i_next = speculative_.end();
         i_next != speculative_.begin() && result.size() < npg;
         --i_next) {
      const list_type::iterator i = prev(i_next);

      i->update_accessed_dirty();
      cache_page_lock l{ *i, try_to_lock };
      if (l.flags() & page::fl_cannot_free_mask) continue;

      if (!l) {
        l.lock();
        if (l.flags() & page::fl_cannot_free_mask) continue;
      }

      if (l.flags() & page::fl_accessed) {
        /* Promote page. */
        lock_guard<mutex> cl{ cold_guard_ };
        i->assign_masked_flags(page::fl_cache_cold,
                               page::fl_cache_mask | page::fl_accessed);
        cold_.link_front(speculative_.unlink(i));
        speculative_hit_.add();  // Update stats.
        continue;
      }

      /* Try to release the page. */
      if (i->try_release_urgent()) {
        page* pg = speculative_.unlink(i);
        pg->clear_flag(page::fl_cache_present);

        /* Update stats. */
        if (pg->get_flags() & page::fl_accessed)
          speculative_hit_.add();
        else
          speculative_miss_.add();

        hot_cold_diff_.fetch_add(1, memory_order_relaxed);
        result.push_pages_no_merge(pg, page_count<native_arch>(1));
      }
    }
  }

  /*
   * Claim cold pages.
   * Pages that are accessed, are promoted.
   */
  if (result.size() < npg) {
    lock_guard<mutex> cl{ cold_guard_ };
    for (list_type::iterator i_next = cold_.end();
         i_next != cold_.begin() && result.size() < npg;
         --i_next) {
      const list_type::iterator i = prev(i_next);

      i->update_accessed_dirty();
      cache_page_lock l{ *i, try_to_lock };
      if (l.flags() & page::fl_cannot_free_mask) continue;

      if (!l) {
        l.lock();
        if (l.flags() & page::fl_cannot_free_mask) continue;
      }

      if (l.flags() & page::fl_accessed) {
        /* Promote page. */
        lock_guard<mutex> hl{ hot_guard_ };
        i->assign_masked_flags(page::fl_cache_hot,
                               page::fl_cache_mask | page::fl_accessed);
        hot_.link_front(cold_.unlink(i));

        hot_cold_diff_.fetch_add(2, memory_order_relaxed);
        continue;
      }

      /* Try to release the page. */
      if (i->try_release_urgent()) {
        page* pg = cold_.unlink(i);
        pg->clear_flag(page::fl_cache_present);
        hot_cold_diff_.fetch_add(1, memory_order_relaxed);
        result.push_pages_no_merge(pg, page_count<native_arch>(1));
      }
    }
  }

  /*
   * Claim hot pages.
   * Pages that are accessed, are promoted.
   */
  if (result.size() < npg) {
    page* looped = nullptr;
    lock_guard<mutex> hl{ hot_guard_ };
    for (list_type::iterator i_next = hot_.end();
         i_next != hot_.begin() && &*prev(i_next) != looped &&
         result.size() < npg;
         --i_next) {
      const list_type::iterator i = prev(i_next);

      i->update_accessed_dirty();
      cache_page_lock l{ *i, try_to_lock };
      if (l.flags() & page::fl_cannot_free_mask) continue;

      if (!l) {
        l.lock();
        if (l.flags() & page::fl_cannot_free_mask) continue;
      }

      if (l.flags() & page::fl_accessed) {
        /* Promote page. */
        i->clear_flag(page::fl_accessed);
        if (looped == nullptr) looped = &*i;
        hot_.link_front(hot_.unlink(i));
        continue;
      }

      /* Try to release the page. */
      if (i->try_release_urgent()) {
        page* pg = hot_.unlink(i);
        pg->clear_flag(page::fl_cache_present);
        hot_cold_diff_.fetch_sub(1, memory_order_relaxed);
        result.push_pages_no_merge(pg, page_count<native_arch>(1));
      }
    }
  }

  /*
   * Claim hot pages.
   * We don't care about promotion at this point.
   */
  if (result.size() < npg) {
    lock_guard<mutex> hl{ hot_guard_ };
    for (list_type::iterator i_next = hot_.end();
         i_next != hot_.begin() && result.size() < npg;
         --i_next) {
      const list_type::iterator i = prev(i_next);

      cache_page_lock l{ *i, try_to_lock };
      if (l.flags() & page::fl_cannot_free_mask) continue;

      if (!l) {
        l.lock();
        if (l.flags() & page::fl_cannot_free_mask) continue;
      }

      if (i->try_release_urgent()) {
        page* pg = hot_.unlink(i);
        pg->clear_flag(page::fl_cache_present);
        hot_cold_diff_.fetch_sub(1, memory_order_relaxed);
        result.push_pages_no_merge(pg, page_count<native_arch>(1));
      }
    }
  }

  // XXX fire rebalance function

  return result;
}

auto page_cache::undirty(page_count<native_arch> npg) noexcept -> void {
  if (npg > page_count<native_arch>(0)) {
    lock_guard<mutex> sl{ speculative_guard_ };
    for (list_type::iterator i_next = speculative_.end();
         i_next != speculative_.begin() && npg > page_count<native_arch>(0);
         --i_next) {
      const list_type::iterator i = prev(i_next);

      i->update_accessed_dirty();
      cache_page_lock l{ *i, try_to_lock };
      if (!l) continue;

      if (l.flags() & page::fl_accessed) {
        lock_guard<mutex> cl{ cold_guard_ };
        i->assign_masked_flags(page::fl_cache_cold,
                               page::fl_cache_mask | page::fl_accessed);
        cold_.link_front(speculative_.unlink(i));
        speculative_hit_.add();  // Update stats.
        continue;
      }

      if (!l.flags() & page::fl_dirty) {
        --npg;
        continue;
      }

      i->undirty();
    }
  }

  if (npg > page_count<native_arch>(0)) {
    lock_guard<mutex> cl{ cold_guard_ };
    for (list_type::iterator i_next = cold_.end();
         i_next != cold_.begin() && npg > page_count<native_arch>(0);
         --i_next) {
      const list_type::iterator i = prev(i_next);

      i->update_accessed_dirty();
      cache_page_lock l{ *i, try_to_lock };
      if (!l) continue;

      if (l.flags() & page::fl_accessed) {
        lock_guard<mutex> hl{ hot_guard_ };
        i->assign_masked_flags(page::fl_cache_hot,
                               page::fl_cache_mask | page::fl_accessed);
        hot_.link_front(cold_.unlink(i));
        hot_cold_diff_.fetch_add(2U, memory_order_relaxed);
        continue;
      }

      if (!l.flags() & page::fl_dirty) {
        --npg;
        continue;
      }

      i->undirty();
    }
  }

  if (npg > page_count<native_arch>(0)) {
    lock_guard<mutex> hl{ hot_guard_ };
    page* looped = nullptr;
    for (list_type::iterator i_next = cold_.end();
         i_next != cold_.begin() && &*prev(i_next) != looped &&
         npg > page_count<native_arch>(0);
         --i_next) {
      const list_type::iterator i = prev(i_next);

      i->update_accessed_dirty();
      cache_page_lock l{ *i, try_to_lock };
      if (!l) continue;

      if (l.flags() & page::fl_accessed) {
        i->clear_flag(page::fl_accessed);
        if (looped == nullptr) looped = &*i;
        hot_.link_front(hot_.unlink(i));
        continue;
      }

      if (!l.flags() & page::fl_dirty) {
        --npg;
        continue;
      }

      i->undirty();
    }
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
