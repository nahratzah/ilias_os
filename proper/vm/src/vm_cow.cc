#include <ilias/vm/cow.h>
#include <ilias/stats.h>
#include <ilias/vm/page_alloc.h>
#include <ilias/vm/page_unbusy_future.h>

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

auto cow_vme::fault_read(monitor_token mt,
                         shared_ptr<page_alloc> pga,
                         page_count<native_arch> off) ->
    cb_future<page_ptr> {
  if (this->anon_vme::present(off))
    return this->anon_vme::fault_read(move(mt), move(pga), off);
  return nested_->fault_read(move(mt), move(pga), off);
}

auto cow_vme::fault_write(monitor_token mt,
                          shared_ptr<page_alloc> pga,
                          page_count<native_arch> off) ->
    cb_future<page_ptr> {
  using std::placeholders::_1;
  using std::placeholders::_2;
  using std::placeholders::_3;
  using std::placeholders::_4;

  if (this->anon_vme::present(off))
    return this->anon_vme::fault_write(move(mt), move(pga), off);

  stats::cow.add();  // Record copy-on-write operation.

  assert_msg(false, "XXX copy nested page into anon");  // XXX implement
  for (;;);

  /* Allocate page for anon. */
  cb_future<page_ptr> pg = pga->allocate(alloc_fail_not_ok);
  /* Fault underlying storage for read access. */
  cb_future<page_ptr> orig_pg = nested_->fault_read(mt, move(pga), off);

  /* Copy original page to anon page. */
  cb_future<page_ptr> copy_pg =
      async(this->get_workq(), launch::aid | launch::parallel | launch::defer,
            [](page_ptr pg, page_ptr orig_pg) -> page_ptr {
              assert(pg != nullptr);
              assert(orig_pg != nullptr);

              assert_msg(false, "XXX implement page_copy");
              // XXX: page_copy(pg, std::move(orig_pg));
              return pg;
            },
            move(pg),
            page_unbusy_future(this->get_workq(), move(orig_pg)));

  /* Assign the whole thing to the anon. */
  return async_lazy(pass_promise<page_ptr>(
      [this](cb_promise<page_ptr> out, monitor_token mt,
             page_count<native_arch> off, page_ptr copy_pg) {
        convert(move(out), this->anon_vme::fault_assign(mt, off, copy_pg));
      }),
      move(mt), move(off), move(copy_pg));
}

auto cow_vme::mincore() const -> vector<bool> {
  vector<bool> rv = this->anon_vme::mincore();
  if (nested_) {
    vector<bool> nested_rv = nested_->mincore();
    assert(rv.size() == nested_rv.size());

    transform(rv.begin(), rv.end(), nested_rv.begin(), rv.begin(),
              logical_or<bool>());
  }
  return rv;
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
