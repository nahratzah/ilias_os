#include <ilias/vm/cow.h>
#include <ilias/stats.h>

namespace ilias {
namespace vm {
namespace stats {

stats_counter cow{ vmmap_group, "copy_on_write" };

} /* namespace ilias::vm::stats */


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

auto cow_vme::fault_read(page_count<native_arch> off) -> future<page_ptr> {
  if (this->anon_vme::present(off))
    return this->anon_vme::fault_read(off);
  return nested_->fault_read(off);
}

auto cow_vme::fault_write(page_count<native_arch> off) -> future<page_ptr> {
  if (this->anon_vme::present(off))
    return this->anon_vme::fault_write(off);

  stats::cow.add();  // Record copy-on-write operation.

#if 0  // Pseudo code.  I want to have some promises here, so I can use async code.
  /* Fault underlying storage for read access. */
  future<page_ptr> original = nested_->fault_read(off);
  future<page_ptr> dst = page_copy(original);
  return this->anon_vme::assign(off, dst);
#endif

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
