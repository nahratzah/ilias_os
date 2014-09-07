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
  auto anon_on_stack = split_no_alloc(off);
  return { make_vmmap_entry<anon_vme>(get<0>(move(anon_on_stack))),
           make_vmmap_entry<anon_vme>(get<1>(move(anon_on_stack))) };
}

auto anon_vme::split_no_alloc(page_count<native_arch> off) const ->
    pair<anon_vme, anon_vme> {
  auto split = off.get();
  assert(split > 0 &&
         static_cast<make_unsigned_t<decltype(split)>>(split) < data_.size());
  return { anon_vme(data_.begin(), data_.begin() + split),
           anon_vme(data_.begin() + split, data_.end()) };
}

auto anon_vme::copy_entry_(const entry_ptr& p) noexcept -> entry_ptr {
  entry_ptr rv = entry_ptr(p.get());
  if (rv) rv->refcnt_.fetch_add(1U, memory_order_acquire);
  return rv;
}


cow_vme::cow_vme(anon_vme&& anon, vmmap_entry_ptr&& nested) noexcept
: anon_vme(move(anon)),
  nested_(move(nested))
{}

cow_vme::cow_vme(page_count<native_arch> npg, vmmap_entry_ptr&& nested)
: anon_vme(npg),
  nested_(move(nested))
{
  if (nested_ == nullptr)
    throw invalid_argument("cow_vme: null nested pointer");
}

cow_vme::cow_vme(const cow_vme& o)
: anon_vme(o),
  nested_(o.nested_)
{}

cow_vme::cow_vme(cow_vme&& o) noexcept
: anon_vme(move(o)),
  nested_(move(o.nested_))
{}

cow_vme::~cow_vme() noexcept {}

auto cow_vme::fault_read(page_count<native_arch> off) -> page_no<native_arch> {
  if (this->anon_vme::present(off))
    return this->anon_vme::fault_read(off);
  return nested_->fault_read(off);
}

auto cow_vme::fault_write(page_count<native_arch> off) ->
    page_no<native_arch> {
  if (this->anon_vme::present(off))
    return this->anon_vme::fault_write(off);
  assert_msg(false, "XXX copy nested page into anon");  // XXX implement
  for (;;);
}

auto cow_vme::clone() const -> vmmap_entry_ptr {
  return make_vmmap_entry<cow_vme>(*this);
}

auto cow_vme::split(page_count<native_arch> off) const ->
    pair<vmmap_entry_ptr, vmmap_entry_ptr> {
  auto anon_on_stack = this->anon_vme::split_no_alloc(off);
  vmmap_entry_ptr nested0, nested1;
  if (nested_)
    tie(nested0, nested1) = nested_->split(off);

  return { make_vmmap_entry<cow_vme>(get<0>(move(anon_on_stack)),
                                     move(nested0)),
           make_vmmap_entry<cow_vme>(get<1>(move(anon_on_stack)),
                                     move(nested1)) };
}


}} /* namespace ilias::vm */
