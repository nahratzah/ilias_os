#include <ilias/vm/anon.h>
#include <ilias/vm/page_alloc.h>
#include <ilias/vm/page_unbusy_future.h>

namespace ilias {
namespace vm {


anon_vme::entry::~entry() noexcept {
  page_->clear_page_owner();
}

auto anon_vme::entry::fault(shared_ptr<page_alloc> pga, workq_ptr wq) ->
    shared_cb_future<page_ptr> {
  unique_lock<mutex> l{ guard_ };

  if (in_progress_.valid()) return in_progress_;

  if (page_ != nullptr) {
    cb_promise<page_ptr> rv;
    rv.set_value(page_);
    return rv.get_future();
  }

  cb_future<page_ptr> f = pga->allocate(alloc_fail_not_ok);
  assert_msg(false, "XXX: implement call to system-wide-allocator.");
  return assign_locked_(move(wq), move(l), move(f));
}

auto anon_vme::entry::assign(workq_ptr wq, cb_future<page_ptr> f) ->
    shared_cb_future<page_ptr> {
  return assign_locked_(wq, unique_lock<mutex>{ guard_ },
                        page_unbusy_future(wq, move(f)));
}

auto anon_vme::entry::assign_locked_(workq_ptr wq, unique_lock<mutex>&& l,
                                     cb_future<page_ptr> f) ->
    shared_cb_future<page_ptr> {
  using std::placeholders::_1;

  assert(l.owns_lock());
  assert(page_ == nullptr && !in_progress_.valid());

  in_progress_ = async(move(wq), launch::aid | launch::defer,
                       &entry::allocation_callback_,
                       entry_ptr(this),
                       move(f));

  shared_cb_future<page_ptr> rv = in_progress_;
  l.unlock();  // In case promise has already completed.
  rv.start();
  return rv;
}

auto anon_vme::entry::allocation_callback_(page_ptr pg) noexcept ->
    page_ptr {
  {
    lock_guard<mutex> l{ guard_ };
    assert(page_ == nullptr && in_progress_.valid());
    page_ = pg;
  }
  if (_predict_true(pg != nullptr))
    pg->set_page_owner(*this);
  return pg;
}

auto anon_vme::entry::release_urgent(page_owner::offset_type, page& pg) ->
    page_ptr {
  assert(pg.get_flags() & page::fl_pgo_call);

  page_ptr rv = nullptr;
  lock_guard<mutex> l{ guard_ };

  if (_predict_false(&pg != page_)) return rv;
  if (pg.get_flags() & page::fl_dirty) return rv;
  page_busy_lock busy_lock = pg.map_ro_and_update_accessed_dirty();
  if (_predict_false(pg.get_flags() & page::fl_dirty)) return rv;

  if (_predict_false(!refcnt_is_solo(pg))) return rv;
  pg.unmap_all(move(busy_lock));
  rv = move(page_);
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
  assert(off.get() >= 0 &&
         (static_cast<make_unsigned_t<decltype(off.get())>>(off.get()) <
          data_.size()));

  const auto& elem = data_[off.get()];
  return (elem != nullptr && elem->present());
}

auto anon_vme::fault_read(shared_ptr<page_alloc> pga,
                          page_count<native_arch> off) ->
    shared_cb_future<page_ptr> {
  return fault_rw_(move(pga), move(off));
}

auto anon_vme::fault_write(shared_ptr<page_alloc> pga,
                           page_count<native_arch> off) ->
    shared_cb_future<page_ptr> {
  return fault_rw_(move(pga), move(off));
}

auto anon_vme::fault_assign(page_count<native_arch> off,
                            cb_future<page_ptr> pg) ->
    shared_cb_future<page_ptr> {
  assert(off.get() >= 0 &&
         (static_cast<make_unsigned_t<decltype(off.get())>>(off.get()) <
          data_.size()));

  auto& elem = data_[off.get()];
  if (elem == nullptr) elem = new entry();
  return elem->assign(get_workq(), move(pg));
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

auto anon_vme::fault_rw_(shared_ptr<page_alloc> pga,
                         page_count<native_arch> off) ->
    shared_cb_future<page_ptr> {
  assert(off.get() >= 0 &&
         (static_cast<make_unsigned_t<decltype(off.get())>>(off.get()) <
          data_.size()));

  auto& elem = data_[off.get()];
  if (elem == nullptr) elem = new entry();
  return elem->fault(move(pga), this->get_workq());
}


}} /* namespace ilias::vm */
