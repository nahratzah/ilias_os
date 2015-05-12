#include <ilias/vm/anon.h>
#include <ilias/vm/page_alloc.h>
#include <ilias/vm/page_unbusy_future.h>

namespace ilias {
namespace vm {


anon_vme::entry::~entry() noexcept {
  page_->clear_page_owner();
}

auto anon_vme::entry::fault(monitor_token /*mt*/,
                            shared_ptr<page_alloc> pga, workq_ptr wq) ->
    cb_future<tuple<page_ptr, monitor_token>> {
  using std::placeholders::_1;
  using std::placeholders::_2;
  using std::placeholders::_3;
  using std::placeholders::_4;

  {
    auto lck = guard_.try_lock(monitor_access::read);
    if (lck && page_ != nullptr) {
      cb_promise<tuple<page_ptr, monitor_token>> pgptr_promise;
      pgptr_promise.set_value(make_tuple(page_, lck));
      return pgptr_promise.get_future();
    }
  }

  return async(wq, launch::parallel | launch::defer,
               pass_promise<tuple<page_ptr, monitor_token>>(
                   [this, pga](cb_promise<tuple<page_ptr, monitor_token>>
                                   pgptr,
                               monitor_token mt,
                               workq_ptr wq) {
                     if (page_ != nullptr) {
                       pgptr.set_value(make_tuple(page_,
                                                  mt.downgrade_to_read()));
                     } else {
                       convert(
                           move(pgptr),
                           async(
                               wq, launch::parallel | launch::defer,
                               bind(&anon_vme::entry::allocation_callback_,
                                    this, _1, _2),
                               mt.upgrade_to_write(),
                               pga->allocate(
                                   alloc_fail_not_ok)));
                     }
                   }),
               guard_.queue(monitor_access::upgrade), wq);
}

auto anon_vme::entry::assign(monitor_token /*mt*/, workq_ptr wq, page_ptr f)
    -> cb_future<tuple<page_ptr, monitor_token>> {
  using std::placeholders::_1;
  using std::placeholders::_2;

  auto mt_future = guard_.queue(monitor_access::write);

  return async(
             wq, launch::parallel,
             bind_once(&anon_vme::entry::allocation_callback_,
                       this, _1, _2),
             move(mt_future), move(f));
}

auto anon_vme::entry::allocation_callback_(monitor_token mt, page_ptr pg)
    noexcept -> tuple<page_ptr, monitor_token> {
  assert(mt.locked() && mt.access() == monitor_access::write &&
         mt.owner() == &guard_);
  assert(page_ == nullptr);

  page_ = pg;
  if (_predict_true(pg != nullptr))
    pg->set_page_owner(*this);
  return make_tuple(pg, mt.downgrade_to_read());
}

auto anon_vme::entry::release_urgent(page_owner::offset_type, page& pg) ->
    page_ptr {
  assert(pg.get_flags() & page::fl_pgo_call);

  page_ptr rv = nullptr;
  const monitor_token l = guard_.try_lock();
  if (!l.locked()) return rv;

  if (_predict_false(&pg != page_)) return rv;
  if (pg.get_flags() & page::fl_dirty) return rv;
  page_busy_lock busy_lock = pg.map_ro_and_update_accessed_dirty();
  if (_predict_false(pg.get_flags() & page::fl_dirty)) return rv;

  if (_predict_false(!refcnt_is_solo(pg))) return rv;
  pg.unmap_all(move(busy_lock));
  rv = std::exchange(page_, nullptr);
  return rv;
}


anon_vme::anon_vme(const anon_vme& o)
: vmmap_entry(o),
  data_(o.data_)
{}

anon_vme::anon_vme(anon_vme&& o) noexcept
: vmmap_entry(move(o)),
  data_(move(o.data_))
{}

anon_vme::anon_vme(workq_ptr wq, page_count<native_arch> npg)
: vmmap_entry(move(wq)),
  data_(npg.get())
{}

anon_vme::~anon_vme() noexcept {}

auto anon_vme::all_present() const noexcept -> bool {
  return all_of(data_.begin(), data_.end(),
                [](const entry_ptr& e) noexcept -> bool {
                  return (e != nullptr && e->present());
                });
}

auto anon_vme::present(page_count<native_arch> off) const noexcept -> bool {
  using unsigned_off = make_unsigned_t<decltype(off.get())>;

  assert(off.get() >= 0 &&
         (static_cast<unsigned_off>(off.get()) < data_.size()));

  const auto& elem = data_[off.get()];
  return (elem != nullptr && elem->present());
}

auto anon_vme::fault_read(monitor_token mt,
                          shared_ptr<page_alloc> pga,
                          page_count<native_arch> off) ->
    cb_future<tuple<page_ptr, monitor_token>> {
  return fault_rw_(move(mt), move(pga), move(off));
}

auto anon_vme::fault_write(monitor_token mt,
                           shared_ptr<page_alloc> pga,
                           page_count<native_arch> off) ->
    cb_future<tuple<page_ptr, monitor_token>> {
  return fault_rw_(move(mt), move(pga), move(off));
}

auto anon_vme::fault_assign(monitor_token mt,
                            page_count<native_arch> off,
                            page_ptr pg) ->
    cb_future<tuple<page_ptr, monitor_token>> {
  assert(off.get() >= 0 &&
         (static_cast<make_unsigned_t<decltype(off.get())>>(off.get()) <
          data_.size()));

  auto& elem = data_[off.get()];
  if (elem == nullptr) elem = new entry();
  return elem->assign(move(mt), get_workq(), move(pg));
}

auto anon_vme::mincore() const -> vector<bool> {
  vector<bool> rv = vector<bool>(data_.size());
  transform(data_.begin(), data_.end(), rv.begin(),
            [](const entry_ptr& e_ptr) noexcept -> bool {
              return (e_ptr != nullptr && e_ptr->present());
            });
  return rv;
}

auto anon_vme::clone() const -> vmmap_entry_ptr {
  return make_vmmap_entry<anon_vme>(*this);
}

auto anon_vme::split(page_count<native_arch> off) const ->
    pair<vmmap_entry_ptr, vmmap_entry_ptr> {
  auto anon_on_stack = split_no_alloc(off);
  return { make_vmmap_entry<anon_vme>(get<0>(move(anon_on_stack))),
           make_vmmap_entry<anon_vme>(get<1>(move(anon_on_stack))) };
}

auto anon_vme::split_no_alloc(page_count<native_arch> off) const ->
    pair<anon_vme, anon_vme> {
  auto split = off.get();
  assert(split > 0 &&
         static_cast<make_unsigned_t<decltype(split)>>(split) < data_.size());
  return { anon_vme(get_workq(), data_.begin(), data_.begin() + split),
           anon_vme(get_workq(), data_.begin() + split, data_.end()) };
}

auto anon_vme::fault_rw_(monitor_token mt,
                         shared_ptr<page_alloc> pga,
                         page_count<native_arch> off) ->
    cb_future<tuple<page_ptr, monitor_token>> {
  assert(off.get() >= 0 &&
         (static_cast<make_unsigned_t<decltype(off.get())>>(off.get()) <
          data_.size()));

  try {
    auto& elem = data_[off.get()];
    if (elem == nullptr) elem = new entry();
    return elem->fault(move(mt), move(pga), this->get_workq());
  } catch (...) {
    cb_promise<tuple<page_ptr, monitor_token>> pgptr_promise;
    pgptr_promise.set_exception(std::current_exception());
    return pgptr_promise.get_future();
  }
}


}} /* namespace ilias::vm */
