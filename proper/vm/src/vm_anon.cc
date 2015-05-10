#include <ilias/vm/anon.h>
#include <ilias/vm/page_alloc.h>
#include <ilias/vm/page_unbusy_future.h>

namespace ilias {
namespace vm {


anon_vme::entry::~entry() noexcept {
  page_->clear_page_owner();
}

auto anon_vme::entry::fault(cb_promise<page_ptr> pgptr_promise,
                            monitor_token /*mt*/,
                            shared_ptr<page_alloc> pga, workq_ptr wq) ->
    void {
  using std::placeholders::_1;
  using std::placeholders::_2;

  {
    auto lck = guard_.try_lock(monitor_access::read);
    if (lck && page_ != nullptr) {
      pgptr_promise.set_value(page_);
      return;
    }
  }

  cb_promise_exceptor<page_ptr> pgptr_exceptor = pgptr_promise;
  try {
    auto mt_future = guard_.queue(monitor_access::upgrade);

    cb_future<page_ptr> read_pg =
        async(wq, launch::parallel | launch::defer,
              pass_promise<page_ptr>([this, pga](cb_promise<page_ptr> pgptr,
                                                 monitor_token mt,
                                                 workq_ptr wq) {
                                       if (page_ != nullptr) {
                                         pgptr.set_value(page_);
                                       } else {
                                         assign_locked_(
                                             move(pgptr), wq,
                                             mt.upgrade_to_write(),
                                             pga->allocate(alloc_fail_not_ok));
                                       }
                                     }),
              move(mt_future), wq);
    convert(move(pgptr_promise), move(read_pg), [](page_ptr p) { return p; });
  } catch (...) {
    pgptr_exceptor.set_current_exception();
  }
}

auto anon_vme::entry::assign(
    cb_promise<page_ptr> pgptr_promise,
    monitor_token /*mt*/,
    workq_ptr wq, cb_future<page_ptr> f) noexcept -> void {
  cb_promise_exceptor<page_ptr> pgptr_exceptor = pgptr_promise;

  try {
    auto mt_future = guard_.queue(monitor_access::write);

    return assign_locked_(move(pgptr_promise), wq, move(mt_future),
                          page_unbusy_future(wq, move(f)));
  } catch (...) {
    pgptr_exceptor.set_current_exception();
  }
}

auto anon_vme::entry::assign_locked_(cb_promise<page_ptr> pgptr_promise,
                                     workq_ptr wq, cb_future<monitor_token> mt,
                                     cb_future<page_ptr> f) noexcept -> void {
  using std::placeholders::_1;

  cb_promise_exceptor<page_ptr> pgptr_exceptor = pgptr_promise;
  try {
    auto acf = async(move(wq), launch::parallel | launch::aid,
                     &entry::allocation_callback_,
                     entry_ptr(this),
                     move(mt), move(f));
    convert(move(pgptr_promise), move(acf),
            [](page_ptr pg) { return pg; });
  } catch (...) {
    pgptr_exceptor.set_current_exception();
  }
}

auto anon_vme::entry::allocation_callback_(monitor_token mt, page_ptr pg)
    noexcept -> page_ptr {
  assert(mt.locked() && mt.access() == monitor_access::write &&
         mt.owner() == &guard_);
  assert(page_ == nullptr);

  page_ = pg;
  if (_predict_true(pg != nullptr))
    pg->set_page_owner(*this);
  return pg;
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

auto anon_vme::fault_read(cb_promise<page_ptr> pgptr_promise,
                          monitor_token mt,
                          shared_ptr<page_alloc> pga,
                          page_count<native_arch> off) noexcept -> void {
  fault_rw_(move(pgptr_promise), move(mt), move(pga), move(off));
}

auto anon_vme::fault_write(cb_promise<page_ptr> pgptr_promise,
                           monitor_token mt,
                           shared_ptr<page_alloc> pga,
                           page_count<native_arch> off) noexcept -> void {
  fault_rw_(move(pgptr_promise), move(mt), move(pga), move(off));
}

auto anon_vme::fault_assign(cb_promise<page_ptr> pgptr_promise,
                            monitor_token mt,
                            page_count<native_arch> off,
                            cb_future<page_ptr> pg) noexcept -> void {
  assert(off.get() >= 0 &&
         (static_cast<make_unsigned_t<decltype(off.get())>>(off.get()) <
          data_.size()));

  cb_promise_exceptor<page_ptr> pgptr_exceptor = pgptr_promise;
  try {
    auto& elem = data_[off.get()];
    if (elem == nullptr) elem = new entry();
    elem->assign(move(pgptr_promise), move(mt), get_workq(), move(pg));
  } catch (...) {
    pgptr_exceptor.set_current_exception();
  }
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

auto anon_vme::fault_rw_(cb_promise<page_ptr> pgptr_promise,
                         monitor_token mt,
                         shared_ptr<page_alloc> pga,
                         page_count<native_arch> off) noexcept -> void {
  assert(off.get() >= 0 &&
         (static_cast<make_unsigned_t<decltype(off.get())>>(off.get()) <
          data_.size()));

  cb_promise_exceptor<page_ptr> pgptr_exceptor = pgptr_promise;
  try {
    auto& elem = data_[off.get()];
    if (elem == nullptr) elem = new entry();
    elem->fault(move(pgptr_promise), move(mt), move(pga), this->get_workq());
  } catch (...) {
    pgptr_exceptor.set_current_exception();
  }
}


}} /* namespace ilias::vm */
