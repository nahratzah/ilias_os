#include <ilias/vm/page.h>
#include <abi/ext/atomic.h>

namespace ilias {
namespace vm {


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

auto page::try_release_urgent() noexcept -> bool {
  try {
    return (release_ && release_(this, false));
  } catch (...) {
    return false;
  }
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

auto page_list::push_pages(page_ptr pg, page_count<native_arch> n)
    noexcept -> void {
  assert(pg->nfree_ == page_count<native_arch>(0));

  if (n == page_count<native_arch>(0)) return;

#ifndef NDEBUG
  /* Assert that there is no overlap with already added ranges. */
  for (const auto& i : data_) {
    assert(pg->address() >= i.address() + i.nfree_ ||
           pg->address() + n <= i.address());
  }
#endif

  /* Merge backward. */
  data_type::iterator succ;
  for (succ = data_.begin(); succ != data_.end(); ++succ) {
    if (pg->address() + n == succ->address()) {
      if (&*pg + n.get() == &*pg) {
        page* succ_ptr = &*succ;
        const page_count<native_arch> n_rm =
            exchange(succ_ptr->nfree_, page_count<native_arch>(0));
        size_ -= n_rm;
        n += n_rm;
      }
      break;
    }
  }

  /* Merge forward. */
  data_type::iterator pred;
  for (pred = data_.begin(); pred != data_.end(); ++pred) {
    if (pred->address() + pred->nfree_ == pg->address() &&
        &*pred + pred->nfree_.get() == &*pg) {
      pred->nfree_ += n;
      size_ += n;
      return;
    }
  }

  /* New entry, link directly. */
  push_pages_no_merge(pg, n);
}

auto page_list::push_pages_no_merge(page_ptr pg, page_count<native_arch> n)
    noexcept -> void {
  pg->nfree_ = n;
  data_.link_back(&*pg);
  size_ += n;
}

auto page_list::pop() noexcept -> page_ptr {
  if (data_.empty()) return nullptr;

  page* pg = data_.unlink_front();
  if (pg->nfree_ > page_count<native_arch>(1)) {
    page* succ = pg + 1U;
    succ->nfree_ = exchange(pg->nfree_, page_count<native_arch>(0)) -
                   page_count<native_arch>(1);
    data_.link_front(succ);
  }

  --size_;
  pg->nfree_ = page_count<native_arch>(0);
  return pg;
}

auto page_list::pop_pages() noexcept ->
    pair<page_ptr, page_count<native_arch>> {
  if (data_.empty()) return { nullptr, page_count<native_arch>(0) };

  page_ptr p = data_.unlink_front();
  page_count<native_arch> n = exchange(p->nfree_, page_count<native_arch>(0));
  size_ -= n;
  return { p, n };
}

auto page_list::clear() noexcept -> void {
  while (!empty()) pop();
}

auto page_list::sort_blocksize_ascending() noexcept -> void {
  data_.sort(data_.begin(), data_.end(),
             [](const page& x, const page& y) {
               return x.nfree_ < y.nfree_;
             });
}

auto page_list::sort_address_ascending(bool merge) noexcept -> void {
  data_.sort(data_.begin(), data_.end(),
             [](const page& x, const page& y) {
               return x.address() < y.address();
             });
  if (empty() || !merge) return;

  /* Merge adjecent entries. */
  for (data_type::iterator succ, i = data_.begin();
       next(i) != data_.end();
       i = succ) {
    succ = next(i);
    if (i->address() + i->nfree_ == succ->address() &&
        &*i + i->nfree_.get() == &*succ) {
      i->nfree_ += exchange(succ->nfree_, page_count<native_arch>(0));
      data_.unlink(succ);

      succ = i;  // Redo this test with the next element.
    }
  }
}


}} /* namespace ilias::vm */
