#include <ilias/vm/anon.h>
#include <ilias/wq_promise.h>

namespace ilias {
namespace vm {


auto anon_vme::entry::fault(workq_ptr wq) -> future<page_ptr> {
  unique_lock<mutex> l{ guard_ };

  if (in_progress_.is_initialized()) return in_progress_;

  if (page_ != nullptr) {
    promise<page_ptr> rv = new_promise<page_ptr>();
    rv.set(page_);
    return rv;
  }

  future<page_ptr> f;  // XXX = system-wide-allocator.allocate(alloc_fail_not_ok);
  assert_msg(false, "XXX: implement call to system-wide-allocator.");
  return assign_locked_(move(wq), move(l), move(f));
}

auto anon_vme::entry::assign(workq_ptr wq, future<page_ptr> f) ->
    future<page_ptr> {
  return assign_locked_(move(wq), unique_lock<mutex>{ guard_ }, move(f));
}

auto anon_vme::entry::assign_locked_(workq_ptr wq, unique_lock<mutex>&& l,
                                     future<page_ptr> f) -> future<page_ptr> {
  using std::placeholders::_1;

  future<page_ptr> rv;

  assert(l.owns_lock());
  assert(page_ == nullptr && !in_progress_.is_initialized());

  rv = in_progress_ = new_promise<page_ptr>();
  l.unlock();  // In case promise has already completed.
  callback(f, move(wq),
           bind(&entry::allocation_callback_, entry_ptr(this), _1));
  return rv;
}

auto anon_vme::entry::allocation_callback_(future<page_ptr> f) noexcept ->
    void {
  promise<page_ptr> tmp;
  page_ptr pg = f.move_or_copy();

  {
    lock_guard<mutex> l{ guard_ };
    assert(page_ == nullptr && in_progress_.is_initialized());
    page_ = pg;
    swap(in_progress_, tmp);
  }

  if (_predict_true(pg != nullptr)) tmp.set(pg);
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

auto anon_vme::fault_read(page_count<native_arch> off) ->
    future<page_ptr> {
  assert(off.get() >= 0 &&
         (static_cast<make_unsigned_t<decltype(off.get())>>(off.get()) <
          data_.size()));

  auto& elem = data_[off.get()];
  if (elem == nullptr) elem = new entry();
  return elem->fault(this->get_workq());
}

auto anon_vme::fault_write(page_count<native_arch> off) ->
    future<page_ptr> {
  assert(off.get() >= 0 &&
         (static_cast<make_unsigned_t<decltype(off.get())>>(off.get()) <
          data_.size()));

  auto& elem = data_[off.get()];
  if (elem == nullptr) elem = new entry();
  return elem->fault(this->get_workq());
}

auto anon_vme::fault_assign(page_count<native_arch> off,
                            future<page_ptr> pg) -> future<page_ptr> {
  assert(off.get() >= 0 &&
         (static_cast<make_unsigned_t<decltype(off.get())>>(off.get()) <
          data_.size()));

  auto& elem = data_[off.get()];
  if (elem == nullptr) elem = new entry();
  return elem->assign(get_workq(), move(pg));
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


}} /* namespace ilias::vm */
