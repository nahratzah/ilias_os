#include <ilias/vm/page.h>
#include <abi/ext/atomic.h>
#include <algorithm>

namespace ilias {
namespace vm {


page::page(page_no<native_arch> pgno) noexcept
: pmap_page<native_arch, native_varch>(pgno)
{}

page::~page() noexcept {}

auto page::try_set_flag_iff_zero(flags_type f) noexcept ->
    pair<flags_type, bool> {
  flags_type expect = flags_.load(memory_order_relaxed);
  expect &= ~f;

  bool succes = !flags_.compare_exchange_strong(expect, expect | f,
                                                memory_order_acquire,
                                                memory_order_relaxed);
  return { expect, succes };
}

auto page::set_flag_iff_zero(flags_type f) noexcept -> flags_type {
  flags_type expect = 0;

  while (!flags_.compare_exchange_weak(expect, expect | f,
                                       memory_order_acquire,
                                       memory_order_relaxed)) {
    expect &= ~f;
    abi::ext::pause();
  }

  return expect;
}

auto page::try_release_urgent() noexcept -> page_ptr {
  class pgo_call_lock {
   public:
    pgo_call_lock() = delete;
    pgo_call_lock(const pgo_call_lock&) = delete;
    pgo_call_lock& operator=(const pgo_call_lock&) = delete;

    pgo_call_lock(page& self)
    : self_(self),
      locked_(self_.try_set_flag_iff_zero(fl_pgo_call).second)
    {}

    ~pgo_call_lock() noexcept {
      if (locked_) self_.clear_flag(fl_pgo_call);
    }

    explicit operator bool() const noexcept { return locked_; }

   private:
    page& self_;
    bool locked_ = false;
  };


  try {
    pgo_call_lock pgo_l{ *this };
    if (!pgo_l) return nullptr;

    page_owner* pgo;
    page_owner::offset_type off;
    {
      lock_guard<mutex> l{ guard_ };
      tie(pgo, off) = pgo_;
    }
    if (!pgo) return nullptr;

    if (!refcnt_acquire_iff_live(*this))
      return nullptr;
    page_ptr self_ptr = page_ptr(this, false);

    if (pgo->release_urgent(off, *this))
      return self_ptr;
  } catch (...) {
    /* SKIP */
  }
  return nullptr;
}

auto page::set_page_owner(page_owner& pgo, page_owner::offset_type off)
    noexcept -> void {
  lock_guard<mutex> l{ guard_ };
  pgo_ = make_tuple(&pgo, off);
}

auto page::clear_page_owner() noexcept -> void {
  lock_guard<mutex> l{ guard_ };
  pgo_ = make_tuple(nullptr, 0);
}


page_range::page_range(page* p, size_type n, bool do_acquire) noexcept
: pg_(p),
  npg_(n)
{
  if (do_acquire) {
    for_each(cbegin(), cend(),
             [](const page& p) { const_pointer(&p).release(); });
  }
}

page_range::page_range(const page_range& o) noexcept
: pg_(o.pg_),
  npg_(o.npg_)
{
  for_each(cbegin(), cend(),
           [](const page& p) { const_pointer(&p).release(); });
}

auto page_range::clear() noexcept -> void {
  for_each(begin(), end(),
           [](const page& p) { return const_pointer(&p, false); });
  npg_ = 0;
}


page_list::page_list(page_list&& o) noexcept
: data_(move(o.data_)),
  size_(exchange(o.size_, page_count<native_arch>(0)))
{}

page_list::~page_list() noexcept {
  clear();
}

auto page_list::operator=(page_list&& o) noexcept -> page_list& {
  assert(&o != this);
  clear();

  data_.splice(data_.end(), move(o.data_));
  size_ = exchange(o.size_, page_count<native_arch>(0));

  return *this;
}

auto page_list::n_blocks() const noexcept -> size_type {
  return distance(data_.begin(), data_.end());
}

auto page_list::push_front(page_ptr pg) noexcept -> void {
  assert(pg->npgl_ == page_count<native_arch>(0));

  pg->npgl_ = page_count<native_arch>(1);
  data_.link_front(pg.release());
  ++size_;
}

auto page_list::push_back(page_ptr pg) noexcept -> void {
  assert(pg->npgl_ == page_count<native_arch>(0));

  pg->npgl_ = page_count<native_arch>(1);
  data_.link_back(pg.release());
  ++size_;
}

auto page_list::push_pages_front(page_range r) noexcept -> void {
  if (r.empty()) return;
  page* pg;
  page_range::size_type npg;
  tie(pg, npg) = r.release();

  for (page* i = pg; i != pg + npg; ++i)
    assert(i->npgl_ == page_count<native_arch>(0));

  pg->npgl_ = page_count<native_arch>(npg);
  assert(pg->npgl_ == page_count<native_arch>(npg));  // Overflow?
  data_.link_front(pg);
  size_ += page_count<native_arch>(npg);
}

auto page_list::push_pages_back(page_range r) noexcept -> void {
  if (r.empty()) return;
  page* pg;
  page_range::size_type npg;
  tie(pg, npg) = r.release();

  for (page* i = pg; i != pg + npg; ++i)
    assert(i->npgl_ == page_count<native_arch>(0));

  pg->npgl_ = page_count<native_arch>(npg);
  assert(pg->npgl_ == page_count<native_arch>(npg));  // Overflow?
  data_.link_back(pg);
  size_ += page_count<native_arch>(npg);
}

auto page_list::pop_front() noexcept -> page_ptr {
  if (data_.empty()) return nullptr;

  page* pg = data_.unlink_front();
  assert(pg->npgl_ > page_count<native_arch>(0));
  if (pg->npgl_ != page_count<native_arch>(1)) {
    page* succ = pg + 1U;
    succ->npgl_ = exchange(pg->npgl_, page_count<native_arch>(0)) -
                   page_count<native_arch>(1);
    data_.link_front(succ);
  }

  --size_;
  pg->npgl_ = page_count<native_arch>(0);
  return page_ptr(pg, false);
}

auto page_list::pop_back() noexcept -> page_ptr {
  if (data_.empty()) return nullptr;

  page* pg = data_.unlink_back();
  assert(pg->npgl_ > page_count<native_arch>(0));
  if (pg->npgl_ != page_count<native_arch>(1)) {
    data_.link_back(pg);
    --pg->npgl_;
    pg += pg->npgl_.get();
  } else {
    pg->npgl_ = page_count<native_arch>(0);
  }

  --size_;
  return page_ptr(pg, false);
}

auto page_list::pop_pages_front() noexcept -> page_range {
  if (data_.empty()) return page_range();

  page* p = data_.unlink_front();
  assert(p->npgl_ > page_count<native_arch>(0));
  page_count<native_arch> n = exchange(p->npgl_, page_count<native_arch>(0));
  size_ -= n;

  page_range rv = page_range(p, n.get(), false);
  /* Assert against overflow. */
  assert(rv.size() ==
         static_cast<make_unsigned_t<decltype(n.get())>>(n.get()));
  return rv;
}

auto page_list::pop_pages_back() noexcept -> page_range {
  if (data_.empty()) return page_range();

  page* p = data_.unlink_back();
  assert(p->npgl_ > page_count<native_arch>(0));
  page_count<native_arch> n = exchange(p->npgl_, page_count<native_arch>(0));
  size_ -= n;

  page_range rv = page_range(p, n.get(), false);
  /* Assert against overflow. */
  assert(rv.size() ==
         static_cast<make_unsigned_t<decltype(n.get())>>(n.get()));
  return rv;
}

auto page_list::clear() noexcept -> void {
  while (!empty()) pop_pages_front();
}

auto page_list::sort_blocksize_ascending() noexcept -> void {
  data_.sort(data_.begin(), data_.end(),
             [](const page& x, const page& y) {
               return x.npgl_ < y.npgl_;
             });
}

namespace {

struct _page_address_less {
  bool operator()(const page& x, const page& y) const noexcept {
    return x.address < y.address;
  }
};

}

auto page_list::sort_address_ascending(bool merge) noexcept -> void {
  if (!is_sorted(data_.begin(), data_.end(), _page_address_less()))
    data_.sort(data_.begin(), data_.end(), _page_address_less());
  if (empty() || !merge) return;

  merge_adjecent_entries();
}

auto page_list::merge(page_list&& o, bool merge) noexcept -> void {
  using std::placeholders::_1;

  /* Sort prior to merge operation. */
  sort_address_ascending();
  o.sort_address_ascending();

  /* Combine data sets. */
  data_type::merge(data_.begin(), data_.end(), o.data_.begin(), o.data_.end(),
                   _page_address_less());
  size_ += exchange(o.size_, page_count<native_arch>(0));

  /* Merge adjecent entries. */
  if (merge) merge_adjecent_entries();
}

auto page_list::merge_adjecent_entries() noexcept -> void {
  if (_predict_false(empty())) return;

  for (data_type::iterator succ, i = data_.begin();
       next(i) != data_.end();
       i = succ) {
    succ = next(i);

    /* Verify that there is no overlap. */
    assert(i->address + i->npgl_ <= succ->address);

    if (i->address + i->npgl_ == succ->address &&
        &*i + i->npgl_.get() == &*succ) {
      i->npgl_ += exchange(succ->npgl_, page_count<native_arch>(0));
      data_.unlink(succ);

      succ = i;  // Redo this test with the next element.
    }
  }
}


}} /* namespace ilias::vm */
