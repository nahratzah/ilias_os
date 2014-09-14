#include <ilias/vm/page_alloc.h>
#include <cdecl.h>
#include <new>
#include <abi/panic.h>
#include <ilias/wq_promise.h>

namespace ilias {
namespace vm {
namespace {

constexpr auto fail_mask = page_alloc::alloc_style::fail_ok |
                           page_alloc::alloc_style::fail_ok_nothrow |
                           page_alloc::alloc_style::fail_not_ok;

} /* namespace ilias::vm::<unnamed> */


using abi::panic;


page_alloc::page_alloc(workq_service& wqs) noexcept
: wq_(wqs.new_workq())
{}

page_alloc::~page_alloc() noexcept {}


default_page_alloc::default_page_alloc(stats_group& parent_group,
                                       workq_service& wqs) noexcept
: page_alloc(wqs),
  cache_group_(parent_group, "page_cache"),
  cache_(this->cache_group_)
{}

default_page_alloc::~default_page_alloc() noexcept {
  ranges_.unlink_all();
}

auto default_page_alloc::allocate(alloc_style style) -> future<page_ptr> {
  using alloc_style::fail_not_ok;
  using alloc_style::fail_ok;
  using alloc_style::fail_ok_nothrow;
  using std::placeholders::_1;

  /* Verify arguments. */
  switch (style & fail_mask) {
  default:
    assert_msg(false, "alloc style failure mode not recognized");
    break;
  case fail_not_ok:
  case fail_ok:
  case fail_ok_nothrow:
    break;
  }

  auto self_ptr =
      static_pointer_cast<default_page_alloc>(this->shared_from_this());
  return new_promise<page_ptr>(
      this->get_workq(),
      bind([](promise<page_ptr> out, const shared_ptr<default_page_alloc>& p,
              alloc_style style) {
             out.set(p->allocate_prom_(style));
           },
           _1, move(self_ptr), style));
}

auto default_page_alloc::allocate_prom_(alloc_style style) -> page_ptr {
  using alloc_style::fail_not_ok;
  using alloc_style::fail_ok;
  using alloc_style::fail_ok_nothrow;

  /* Verify arguments. */
  switch (style & fail_mask) {
  default:
    assert_msg(false, "alloc style failure mode not recognized");
    break;
  case fail_not_ok:
  case fail_ok:
  case fail_ok_nothrow:
    break;
  }

  promise<page_ptr> rv_prom = new_promise<page_ptr>();
  lock_guard<mutex> l{ mtx_ };

  page_ptr rv = fetch_from_freelist_();
  if (_predict_true(rv)) return rv;

  assert_msg(false, "XXX implement");  // XXX implement, return promise that may or may not be complete.

  switch (style & fail_mask) {
  case fail_not_ok:
    panic("Failed to allocate page.");
    break;
  case fail_ok:
    __throw_bad_alloc();
    break;
  case fail_ok_nothrow:
    return nullptr;
  }

  __builtin_unreachable();
  for (;;);
}

auto default_page_alloc::allocate_urgent(alloc_style style) -> page_ptr {
  using alloc_style::fail_not_ok;
  using alloc_style::fail_ok;
  using alloc_style::fail_ok_nothrow;

  /* Verify arguments. */
  switch (style & fail_mask) {
  default:
    assert_msg(false, "alloc style failure mode not recognized");
    break;
  case fail_not_ok:
  case fail_ok:
  case fail_ok_nothrow:
    break;
  }

  lock_guard<mutex> l{ mtx_ };

  /* Try normal allocation. */
  page_ptr rv = fetch_from_freelist_();
  if (_predict_true(rv)) return rv;

  /* Try poking the cache to release a page. */
  page_list pgl = cache_.try_release_urgent(page_count<native_arch>(1));
  assert(pgl.size() == page_count<native_arch>(1) || pgl.empty());
  if (!pgl.empty()) {
    rv = pgl.pop();
    if (rv) return rv;
  }

  /* Complete failure, handle failure style. */
  switch (style & fail_mask) {
  default:
  case fail_not_ok:
    panic("Failed to allocate pages.");
    break;
  case fail_ok:
    __throw_bad_alloc();
    break;
  case fail_ok_nothrow:
    return nullptr;
  }

  __builtin_unreachable();
  for (;;);
}

auto default_page_alloc::allocate(page_count<native_arch> npg,
                                  alloc_style style) -> future<page_list> {
  using alloc_style::fail_not_ok;
  using alloc_style::fail_ok;
  using alloc_style::fail_ok_nothrow;
  using std::placeholders::_1;

  /* Verify arguments. */
  switch (style & fail_mask) {
  default:
    assert_msg(false, "alloc style failure mode not recognized");
    break;
  case fail_not_ok:
  case fail_ok:
  case fail_ok_nothrow:
    break;
  }

  promise<page_list> rv = new_promise<page_list>();
  if (_predict_false(npg == page_count<native_arch>(0))) {
    rv.set(page_list());
    return rv;
  }
  assert(npg > page_count<native_arch>(0));

  auto self_ptr =
      static_pointer_cast<default_page_alloc>(this->shared_from_this());
  callback(rv, this->get_workq(),
           bind([](promise<page_list> out,
                   const shared_ptr<default_page_alloc>& p,
                   page_count<native_arch> npg, alloc_style style) {
                  out.set(p->allocate_prom_(npg, style));
                },
                _1, move(self_ptr), npg, style));
  return rv;
}

auto default_page_alloc::allocate_prom_(page_count<native_arch> npg,
                                alloc_style style) -> page_list {
  using alloc_style::fail_not_ok;
  using alloc_style::fail_ok;
  using alloc_style::fail_ok_nothrow;

  /* Verify arguments. */
  switch (style & fail_mask) {
  default:
    assert_msg(false, "alloc style failure mode not recognized");
    break;
  case fail_not_ok:
  case fail_ok:
  case fail_ok_nothrow:
    break;
  }

  page_list rv;
  lock_guard<mutex> l{ mtx_ };

  while (rv.size() < npg) {
    page_ptr p;
    page_count<native_arch> n;
    tie(p, n) = fetch_from_freelist_(npg - rv.size());

    /* Bi-implication:  (p == nullptr)  <==>  (n == 0). */
    assert(bool(p == nullptr) == bool(n == page_count<native_arch>(0)));
    if (_predict_false(p == nullptr)) break;  // GUARD

    rv.push_pages_no_merge(move(p), move(n));
  }
  if (rv.size() == npg) return rv;

  /*
   * Try to reclaim from page cache.
   */
  rv.merge(move(cache_.try_release(npg - rv.size()).get_mutable()));  // XXX: may throw, may fail, may block

  if (_predict_true(rv.size() == npg)) return rv;

  /*
   * Failed to allocate.
   *
   * Undo allocation and perform failure.
   */
  while (!rv.empty()) {
    page_ptr pg;
    page_count<native_arch> npg;
    tie(pg, npg) = rv.pop_pages();
    add_to_freelist_(pg, npg);
  }

  switch (style & fail_mask) {
  case fail_not_ok:
    panic("Failed to allocate pages.");
    break;
  case fail_ok:
    __throw_bad_alloc();
  case fail_ok_nothrow:
    return page_list();
  }

  __builtin_unreachable();
  for (;;);
}

auto default_page_alloc::allocate_urgent(page_count<native_arch>,
                                         alloc_style) -> page_list {
  assert_msg(false, "XXX: implement");  // XXX implement
  for (;;);
}

auto default_page_alloc::allocate(page_count<native_arch>, spec) ->
    future<page_list> {
  assert_msg(false, "XXX: implement");  // XXX implement
  for (;;);
}

auto default_page_alloc::allocate_urgent(page_count<native_arch>, spec) ->
    page_list {
  assert_msg(false, "XXX: implement");  // XXX implement
  for (;;);
}

auto default_page_alloc::deallocate(page_list pgl) noexcept -> void {
  if (_predict_false(pgl.empty())) return;
  pgl.sort_address_ascending();

  lock_guard<mutex> l{ mtx_ };
  while (!pgl.empty()) {
    page_ptr pg;
    page_count<native_arch> n;
    tie(pg, n) = pgl.pop_pages();
    add_to_freelist_(pg, n);
  }
}

auto default_page_alloc::deallocate(page_ptr pg) noexcept -> void {
  lock_guard<mutex> l{ mtx_ };
  add_to_freelist_(pg, page_count<native_arch>(1));
}

auto default_page_alloc::fetch_from_freelist_() noexcept -> page_ptr {
  const auto i = ranges_.root();
  if (_predict_false(i == ranges_.end()))
    return nullptr;

  page_ptr rv;
  const auto i_pred = (i == ranges_.begin() ? ranges_.end() : prev(i));
  if (_predict_true(i->nfree_ > page_count<native_arch>(1) &&
                    i->nfree_ > i_pred->nfree_)) {
    auto off = --i->nfree_;
    rv = &*i + off.get();
  } else if (i->nfree_ == page_count<native_arch>(1)) {
    rv = ranges_.unlink(i);
  } else {
    assert(i->nfree_ > page_count<native_arch>(1));
    page* r = ranges_.unlink(i);
    auto off = --r->nfree_;

    bool link_succes;
    tie(ignore, link_succes) = ranges_.link(r, true);
    assert(link_succes);
    rv = r + off.get();
  }

  --free_;
  auto old_flags = rv->clear_flag(page::fl_free);
  assert(old_flags & page::fl_free);
  return rv;
}

auto default_page_alloc::fetch_from_freelist_(page_count<native_arch> n)
    noexcept -> tuple<page_ptr, page_count<native_arch>> {
  const auto i = ranges_.root();
  if (_predict_false(i == ranges_.end()))
    return make_tuple(nullptr, page_count<native_arch>(0));

  page_ptr rv_ptr;
  page_count<native_arch> rv_npg;
  const auto i_pred = (i == ranges_.begin() ? ranges_.end() : prev(i));
  if (_predict_true(i->nfree_ > n && i->nfree_ - n >= i_pred->nfree_)) {
    auto off = (i->nfree_ -= n);
    rv_ptr = &*i + off.get();
    rv_npg = n;
  } else if (i->nfree_ <= n) {
    rv_ptr = ranges_.unlink(i);
    rv_npg = exchange(rv_ptr->nfree_, page_count<native_arch>(0));
  } else {
    assert(i->nfree_ > n);
    page* r = ranges_.unlink(i);
    auto off = (i->nfree_ -= n);

    bool link_succes;
    tie(ignore, link_succes) = ranges_.link(r, true);
    assert(link_succes);
    rv_ptr = r + off.get();
    rv_npg = n;
  }

  free_ -= rv_npg;
  for_each(&*rv_ptr, &*rv_ptr + rv_npg.get(),
           [](page& pg) {
             auto old_flags = pg.clear_flag(page::fl_free);
             assert(old_flags & page::fl_free);
           });
  return make_tuple(move(rv_ptr), move(rv_npg));
}

auto default_page_alloc::add_to_freelist_(page_ptr pg,
                                          page_count<native_arch> n)
    noexcept -> void {
  assert(pg->nfree_ != page_count<native_arch>(0));

  /* Remove freeed pages from cache, mark as free. */
  {
    page* pgi = &*pg;

    for (page_count<native_arch> i = page_count<native_arch>(0);
         i != n;
         ++n, ++pgi) {
      cache_.unmanage(pgi);
      const auto old_flags = pgi->set_flag(page::fl_free);
      assert(!(old_flags & page::fl_free));
    }
  }

  pg->nfree_ = n;
  bool link_succes;
  ranges_type::iterator pgi;
  tie(pgi, link_succes) = ranges_.link(pg, true);
  assert(link_succes);

  /* Merge range forward. */
  ranges_type::iterator pg_succ = next(pgi);
  assert(pg_succ->address() >= pgi->address() + n);
  if (pg_succ->address() == pgi->address() + n &&
      &*pg_succ == &*pgi + n.get()) {
    pgi->nfree_ += exchange(pg_succ->nfree_, page_count<native_arch>(0));
    ranges_.unlink(pg_succ);
  }

  /* Merge range backward. */
  ranges_type::iterator pg_pred = prev(pgi);
  assert(pg_pred->address() + pg_pred->nfree_ <= pgi->address());
  if (pg_pred->address() + pg_pred->nfree_ == pgi->address() &&
      &*pgi == &*pg_pred + pg_pred->nfree_.get()) {
    pg_pred->nfree_ += exchange(pgi->nfree_, page_count<native_arch>(0));
    ranges_.unlink(pgi);
  }

  free_ += n;
}


}} /* namespace ilias::vm */
