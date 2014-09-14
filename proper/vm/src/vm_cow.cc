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

cow_vme::cow_vme(workq_ptr wq, page_count<native_arch> npg,
                 vmmap_entry_ptr&& nested)
: anon_vme(move(wq), npg),
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

  assert_msg(false, "XXX copy nested page into anon");  // XXX implement
  for (;;);

  /* Allocate page for anon. */
  future<page_ptr> pg;  // XXX = system-wide-allocator.allocate(1)
  /* Fault underlying storage for read access. */
  future<page_ptr> orig_pg = nested_->fault_read(off);

  /* Copy original page to anon page. */
  future<page_ptr> copy_pg;  // XXX =
#if 0
      combine([](promise<page_ptr> out,
                 future<page_ptr> pg, future<page_ptr> orig_pg) {
                page_copy(pg.get_mutable(), orig_pg.move_or_copy());
                out.set(pg.move_or_copy());
              },
              pg, orig_pg);
#endif

  /* Assign the whole thing to the anon. */
  return this->anon_vme::fault_assign(off, move(copy_pg));
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
