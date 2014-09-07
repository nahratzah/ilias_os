#include <ilias/vm/anon.h>

namespace ilias {
namespace vm {


anon_vme::anon_vme(const anon_vme& o) {
  data_.reserve(o.data_.size());
  transform(o.data_.begin(), o.data_.end(), back_inserter(data_),
            &copy_entry_);
}

anon_vme::anon_vme(anon_vme&& o) noexcept
: data_(move(o.data_))
{}

anon_vme::anon_vme(page_count<native_arch> npg) {
  data_.reserve(npg.get());
  generate_n(back_inserter(data_), npg.get(),
             []() { return entry_ptr(new entry()); });
}

anon_vme::~anon_vme() noexcept {}

auto anon_vme::fault_read(page_count<native_arch>) ->
    page_no<native_arch> {
  assert_msg(false, "XXX implement");  // XXX implement
  return page_no<native_arch>(0);
}

auto anon_vme::fault_write(page_count<native_arch>) ->
    page_no<native_arch> {
  assert_msg(false, "XXX implement");  // XXX implement
  return page_no<native_arch>(0);
}

auto anon_vme::clone() const -> vmmap_entry_ptr {
  return make_vmmap_entry<anon_vme>(*this);
}

auto anon_vme::split(page_count<native_arch> off) const ->
    pair<vmmap_entry_ptr, vmmap_entry_ptr> {
  auto split = off.get();
  assert(split > 0 &&
         static_cast<make_unsigned_t<decltype(split)>>(split) < data_.size());
  return { make_vmmap_entry<anon_vme>(data_.begin(), data_.begin() + split),
           make_vmmap_entry<anon_vme>(data_.begin() + split, data_.end()) };
}

auto anon_vme::copy_entry_(const entry_ptr& p) noexcept -> entry_ptr {
  entry_ptr rv = entry_ptr(p.get());
  if (rv) rv->refcnt_.fetch_add(1U, memory_order_acquire);
  return rv;
}


}} /* namespace ilias::vm */
