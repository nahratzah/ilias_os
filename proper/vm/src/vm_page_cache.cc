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


class page_cache::hot_cold_rebalance_wqjob
: public workq_job
{
 public:
  hot_cold_rebalance_wqjob(workq_ptr, page_cache&);
  ~hot_cold_rebalance_wqjob() noexcept override;

  void run() noexcept override;

 private:
  page_cache& self_;
};

class page_cache::spec_need_rebalance_wqjob
: public workq_job {
 public:
  spec_need_rebalance_wqjob(workq_ptr, page_cache&);
  ~spec_need_rebalance_wqjob() noexcept;

  void run() noexcept override;

 private:
  page_cache& self_;
};


page_cache::hot_cold_rebalance_wqjob::hot_cold_rebalance_wqjob(
    workq_ptr wq, page_cache& self)
: workq_job(wq),
  self_(self)
{}

page_cache::hot_cold_rebalance_wqjob::~hot_cold_rebalance_wqjob() noexcept {}

auto page_cache::hot_cold_rebalance_wqjob::run() noexcept -> void {
  auto hc_diff = self_.hot_cold_diff_.load(memory_order_relaxed);
  if (hc_diff >= -1 && hc_diff <= 1) return;
  const unsigned int scan_zone = (hc_diff < 0 ? cold_zone : hot_zone);
  const bool demote_break = (hc_diff > 0);
  const bool promote_break = (hc_diff < 0);

  page* stop = nullptr;
  for (list_type::iterator i_next, i = self_.data_[scan_zone].begin();
       i != self_.data_[scan_zone].end() && &*i != stop;
       i = i_next) {
    i_next = next(i);

    cache_page_lock l{ *i, try_to_lock };
    if (!l) continue;

    i->update_accessed_dirty();
    if (i->clear_flag(page::fl_accessed) & page::fl_accessed) {
      page& i_ref = *i;
      if (self_.promote_(scan_zone, *i)) {
        if (stop == nullptr) stop = &i_ref;
        if (promote_break) break;
      }
      continue;
    }

    if (self_.demote_(scan_zone, *i) && demote_break) break;
  }
}


page_cache::spec_need_rebalance_wqjob::spec_need_rebalance_wqjob(
    workq_ptr wq, page_cache& self)
: workq_job(wq),
  self_(self)
{}

page_cache::spec_need_rebalance_wqjob::~spec_need_rebalance_wqjob() noexcept {}

auto page_cache::spec_need_rebalance_wqjob::run() noexcept -> void {
  auto cn_diff = self_.spec_need_diff_.load(memory_order_relaxed);
  if (cn_diff >= 0) return;

  for (list_type::iterator i_next, i = self_.data_[cold_zone].begin();
       i != self_.data_[cold_zone].end();
       i = i_next) {
    i_next = next(i);

    cache_page_lock l{ *i, try_to_lock };
    if (!l) continue;

    i->update_accessed_dirty();
    if (i->clear_flag(page::fl_accessed) & page::fl_accessed) {
      self_.promote_(cold_zone, *i);
      continue;
    }

    if (i->get_flags() & page::fl_dirty) {
      i->undirty();
      continue;
    }

    page_busy_lock busy_lock = i->map_ro_and_update_accessed_dirty();
    if (i->get_flags() & (page::fl_accessed | page::fl_dirty))
      continue;

    if (self_.demote_(cold_zone, *i))
      break;
  }
}


constexpr unsigned int page_cache::n_zones;
constexpr unsigned int page_cache::spec_zone;
constexpr unsigned int page_cache::cold_zone;
constexpr unsigned int page_cache::hot_zone;
constexpr array<page::flags_type, page_cache::n_zones> page_cache::pg_flags;


page_cache::page_cache(stats_group& group, workq_ptr wq)
: hot_cold_rebalance_job_(new_workq_job<hot_cold_rebalance_wqjob>(wq,
                                                                  *this)),
  spec_need_rebalance_job_(new_workq_job<spec_need_rebalance_wqjob>(wq,
                                                                    *this)),
  manage_(group, "manage"),
  unmanage_(group, "unmanage"),
  hot_cold_rebalance_(group, "hot_cold_rebalance"),
  spec_need_rebalance_(group, "spec_need_rebalance"),
  speculative_hit_(group, "speculative_hit"),
  speculative_miss_(group, "speculative_miss")
{}

page_cache::page_cache(stats_group& group, workq_service& wqs)
: page_cache(group, wqs.new_workq())
{}

page_cache::~page_cache() noexcept {
  assert(empty());

  workq_deactivate(hot_cold_rebalance_job_);
  hot_cold_rebalance_job_ = nullptr;

  workq_deactivate(spec_need_rebalance_job_);
  spec_need_rebalance_job_ = nullptr;
}

auto page_cache::empty() const noexcept -> bool {
  for (const auto& q : data_)
    if (!q.empty()) return false;
  return true;
}

auto page_cache::manage(const page_refptr& pg, bool speculative) noexcept ->
    void {
  cache_page_lock l{ *pg };
  assert(!(l.flags() & page::fl_cache_present));

  if (link_zone_((speculative ? spec_zone : cold_zone), *pg))
    manage_.add();
}

auto page_cache::unmanage(const page_refptr& pg) noexcept -> void {
  cache_page_lock l{ *pg };
  assert(!(l.flags() & page::fl_cache_present));

  bool unlinked;
  switch (pg->get_flags() & page::fl_cache_mask) {
  case page::fl_cache_speculative:
    unlinked = unlink_zone_(spec_zone, *pg);
    break;
  case page::fl_cache_cold:
    unlinked = unlink_zone_(cold_zone, *pg);
    break;
  case page::fl_cache_hot:
    unlinked = unlink_zone_(hot_zone, *pg);
    break;
  default:
    assert_msg(false, "Page cache mask value not recognized");
    __builtin_unreachable();
  }

  if (unlinked)
    unmanage_.add();
}

auto page_cache::try_release_urgent(page_count<native_arch> npg) noexcept ->
    page_list {
  return try_release_urgent_zone_(spec_zone, npg, page_list());
}

auto page_cache::try_release(page_count<native_arch> npg) noexcept ->
    future<page_list> {
  return try_release_zone_(spec_zone, npg);
}

auto page_cache::unlink_zone_(unsigned int zone, page& pg) noexcept -> bool {
  assert((pg.get_flags() &
          (page::fl_cache_present | page::fl_cache_modify)) ==
         (page::fl_cache_present | page::fl_cache_modify));
  assert(zone >= 0 && zone < data_.size());
  assert((pg.get_flags() & page::fl_cache_mask) == pg_flags[zone]);

  bool result = false;
  data_[zone].erase_and_dispose(
      data_[zone].iterator_to(&pg),
      [&result](page* pg) {
        pg->clear_flag(page::fl_cache_present);
        result = true;
      });
  if (!result) return false;

  if (zone == hot_zone) {
    if (hot_cold_diff_.fetch_sub(1, memory_order_relaxed) < 0)
      workq_activate(hot_cold_rebalance_job_);
  } else {
    if (hot_cold_diff_.fetch_add(1, memory_order_relaxed) > 0)
      workq_activate(hot_cold_rebalance_job_);
  }

  if (zone == spec_zone &&
      spec_need_diff_.fetch_sub(1, memory_order_relaxed) <= 0)
    workq_activate(spec_need_rebalance_job_);

  return true;
}

auto page_cache::link_zone_(unsigned int zone, page& pg) noexcept -> bool {
  assert((pg.get_flags() &
          (page::fl_cache_present | page::fl_cache_modify)) ==
         page::fl_cache_modify);
  assert(zone >= 0 && zone < data_.size());

  if (!data_[zone].push_back(&pg)) return false;
  pg.assign_masked_flags(page::fl_cache_present | pg_flags[zone],
                         page::fl_cache_present | page::fl_cache_mask);

  if (zone == hot_zone) {
    if (hot_cold_diff_.fetch_add(1, memory_order_relaxed) > 0)
      workq_activate(hot_cold_rebalance_job_);
  } else {
    if (hot_cold_diff_.fetch_sub(1, memory_order_relaxed) < 0)
      workq_activate(hot_cold_rebalance_job_);
  }

  if (zone == spec_zone)
    spec_need_diff_.fetch_add(1, memory_order_relaxed);

  return true;
}

auto page_cache::promote_(unsigned int zone, page& i) noexcept ->
    bool {
  assert(i.get_flags() & page::fl_cache_modify);
  assert((i.get_flags() & page::fl_cache_mask) == pg_flags[zone]);

  const auto dst_zone = (zone == hot_zone ? hot_zone : zone + 1U);
  page* pg = nullptr;
  data_[zone].erase_and_dispose(data_[zone].iterator_to(&i),
                                [&pg](page* p) { pg = p; });
  if (!pg) return false;
  data_[dst_zone].push_back(pg);

  if (zone == cold_zone) {
    if (hot_cold_diff_.fetch_add(2, memory_order_relaxed) >= 0)
      workq_activate(hot_cold_rebalance_job_);
  } else if (zone == spec_zone) {
    if (spec_need_diff_.fetch_sub(1, memory_order_relaxed) <= 0)
      workq_activate(spec_need_rebalance_job_);
  }

  if (zone == spec_zone)
    speculative_hit_.add();

  return true;
}

auto page_cache::demote_(unsigned int zone, page& i) noexcept ->
    bool {
  assert(i.get_flags() & page::fl_cache_modify);
  assert((i.get_flags() & page::fl_cache_mask) == pg_flags[zone]);
  if (zone == spec_zone) return false;

  const auto dst_zone = zone - 1U;
  page* pg = nullptr;
  data_[zone].erase_and_dispose(data_[zone].iterator_to(&i),
                                [&pg](page* p) { pg = p; });
  if (!pg) return false;
  data_[dst_zone].push_back(pg);

  if (zone == hot_zone) {
    if (hot_cold_diff_.fetch_sub(2, memory_order_relaxed) <= 0)
      workq_activate(hot_cold_rebalance_job_);
  } else if (zone == cold_zone) {
    if (spec_need_diff_.fetch_add(1, memory_order_relaxed) < -1)
      workq_activate(spec_need_rebalance_job_);
  }

  return true;
}

auto page_cache::try_release_urgent_zone_(unsigned int zone,
                                          page_count<native_arch> npg,
                                          page_list pgl)
    noexcept -> page_list {
  page* stop = nullptr;

  for (list_type::iterator i_next, i = data_[zone].begin();
       i != data_[zone].end() && &*i != stop && pgl.size() < npg;
       i = i_next) {
    i_next = next(i);

    cache_page_lock l{ *i };
    i->update_accessed_dirty();
    page::flags_type pgfl = i->clear_flag(page::fl_accessed);
    if (pgfl & page::fl_accessed) {
      page& i_ref = *i;
      if (promote_(zone, *i) && stop == nullptr) stop = &i_ref;
      continue;
    }
    if (pgfl & page::fl_cannot_free_mask) continue;

    page_refptr pg = i->try_release_urgent();
    const bool unlinked_succes = unlink_zone_(zone, *pg);
    assert(unlinked_succes);  // XXX can this fail?
    pgl.push_back(move(pg));
  }

  return pgl;
}

auto page_cache::try_release_zone_(unsigned int /*zone*/,
                                   page_count<native_arch> /*npg*/) noexcept ->
    future<page_list> {
  assert_msg(false, "XXX implement this");
  return new_promise<page_list>([](promise<page_list> out) { out.set(page_list()); });  // XXX implement
}


}} /* namespace ilias::vm */
