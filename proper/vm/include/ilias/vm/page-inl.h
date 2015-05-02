#ifndef _ILIAS_VM_PAGE_INL_H_
#define _ILIAS_VM_PAGE_INL_H_

#include <ilias/vm/page.h>
#include <ilias/pmap/page.h>
#include <abi/ext/atomic.h>

namespace ilias {
namespace vm {


inline page_busy_lock::page_busy_lock(page& pg) noexcept
: pg_(&pg)
{
  lock();
}

inline page_busy_lock::page_busy_lock(page& pg, try_to_lock_t) noexcept
: pg_(&pg)
{
  try_lock();
}

inline page_busy_lock::page_busy_lock(page& pg, defer_lock_t) noexcept
: pg_(&pg)
{}

inline page_busy_lock::page_busy_lock(page_busy_lock&& o) noexcept
: pg_(exchange(o.pg_, nullptr)),
  locked_(exchange(o.locked_, false))
{}

inline page_busy_lock::~page_busy_lock() noexcept {
  if (locked_) unlock();
}

inline auto page_busy_lock::lock() noexcept -> void {
  assert(!locked_);
  assert(pg_ != nullptr);

  pg_->set_flag_iff_zero(page::fl_busy);
  locked_ = true;
}

inline auto page_busy_lock::try_lock() noexcept -> bool {
  assert(!locked_);
  assert(pg_ != nullptr);

  return locked_ = pg_->try_set_flag_iff_zero(page::fl_busy).second;
}

inline auto page_busy_lock::unlock() noexcept -> void {
  assert(pg_ != nullptr);
  assert(locked_);

  pg_->clear_flag(page::fl_busy);
}

inline page_busy_lock::operator bool() const noexcept {
  return owns_lock();
}

inline auto page_busy_lock::owns_lock() const noexcept -> bool {
  return locked_;
}

inline auto page_busy_lock::get_page() const noexcept -> page* {
  return pg_;
}


inline auto page::get_flags() const noexcept -> flags_type {
  return flags_.load(memory_order_relaxed);
}

inline auto page::set_flag(flags_type f) noexcept -> flags_type {
  return flags_.fetch_or(f, memory_order_acq_rel);
}

inline auto page::clear_flag(flags_type f) noexcept -> flags_type {
  return flags_.fetch_and(~f, memory_order_acq_rel);
}

inline auto page::assign_masked_flags(flags_type f, flags_type msk) noexcept ->
    flags_type {
  assert((f & msk) == 0);
  flags_type expect = 0;

  while (!flags_.compare_exchange_weak(expect, (expect & ~msk) | f,
                                       memory_order_acq_rel,
                                       memory_order_relaxed)) {
    abi::ext::pause();
  }

  return expect;
}


inline page_range::page_range(page_range&& o) noexcept
: pg_(exchange(o.pg_, nullptr)),
  npg_(exchange(o.npg_, 0))
{}

inline page_range::page_range(page_ptr p) noexcept
: pg_(p.release()),
  npg_(pg_ == nullptr ? 0 : 1)
{}

inline page_range::~page_range() noexcept {
  if (!empty()) clear();
}

inline auto page_range::operator=(page_range r) noexcept -> page_range& {
  swap(*this, r);
  return *this;
}

inline auto page_range::operator==(const page_range& o) const noexcept ->
    bool {
  return (pg_ == o.pg_ && npg_ == o.npg_) || (empty() && o.empty());
}

inline auto page_range::operator!=(const page_range& o) const noexcept ->
    bool {
  return !(*this == o);
}

inline auto page_range::operator[](size_type idx) noexcept -> reference {
  assert(idx >= 0 && idx < npg_);
  return pg_[idx];
}

inline auto page_range::operator[](size_type idx) const noexcept ->
    const_reference {
  assert(idx >= 0 && idx < npg_);
  return pg_[idx];
}

inline auto page_range::at(size_type idx) -> reference {
  if (!(idx >= 0 && idx < npg_))
    throw out_of_range("page_range::at");
  return (*this)[idx];
}

inline auto page_range::at(size_type idx) const -> const_reference {
  if (!(idx >= 0 && idx < npg_))
    throw out_of_range("page_range::at");
  return (*this)[idx];
}

inline auto page_range::front() noexcept -> reference {
  return (*this)[0];
}

inline auto page_range::front() const noexcept -> const_reference {
  return (*this)[0];
}

inline auto page_range::back() noexcept -> reference {
  return (*this)[npg_ - 1U];
}

inline auto page_range::back() const noexcept -> const_reference {
  return (*this)[npg_ - 1U];
}

inline auto page_range::pop_front() noexcept -> pointer {
  pointer rv;
  if (!empty()) {
    rv = pointer(&front(), false);
    ++pg_;
    --npg_;
  }
  return rv;
}

inline auto page_range::pop_back() noexcept -> pointer {
  pointer rv;
  if (!empty()) {
    rv = pointer(&back(), false);
    --npg_;
  }
  return rv;
}

inline auto page_range::empty() const noexcept -> bool {
  return size() == 0;
}

inline auto page_range::size() const noexcept -> size_type {
  return npg_;
}

inline auto page_range::release() noexcept -> pair<page*, size_type> {
  return make_pair(pg_, exchange(npg_, 0));
}

inline auto page_range::begin() noexcept -> iterator {
  return pg_;
}

inline auto page_range::end() noexcept -> iterator {
  return pg_ + npg_;
}

inline auto page_range::begin() const noexcept -> const_iterator {
  return pg_;
}

inline auto page_range::end() const noexcept -> const_iterator {
  return pg_ + npg_;
}

inline auto page_range::cbegin() const noexcept -> const_iterator {
  return begin();
}

inline auto page_range::cend() const noexcept -> const_iterator {
  return end();
}

inline auto swap(page_range& x, page_range& y) noexcept -> void {
  using std::swap;

  swap(x.pg_, y.pg_);
  swap(x.npg_, y.npg_);
}


inline auto page_list::empty() const noexcept -> bool {
  return data_.empty();
}

inline auto page_list::size() const noexcept -> page_count<native_arch> {
  return size_;
}

inline auto page_list::begin() const noexcept -> const_iterator {
  return iterator(data_.begin());
}

inline auto page_list::end() const noexcept -> const_iterator {
  return iterator(data_.end());
}

inline auto page_list::cbegin() const noexcept -> const_iterator {
  return begin();
}

inline auto page_list::cend() const noexcept -> const_iterator {
  return end();
}


inline page_list::proxy::proxy(const page& pg) noexcept
: r_(const_cast<page*>(&pg), pg.npgl_.get())
{}


inline auto page_list::iterator::operator++() noexcept -> iterator& {
  ++impl_;
  return *this;
}

inline auto page_list::iterator::operator++(int) noexcept -> iterator {
  return iterator(impl_++);
}

inline auto page_list::iterator::operator--() noexcept -> iterator& {
  --impl_;
  return *this;
}

inline auto page_list::iterator::operator--(int) noexcept -> iterator {
  return iterator(impl_--);
}

inline auto page_list::iterator::operator==(const iterator& o)
    const noexcept -> bool {
  return impl_ == o.impl_;
}

inline auto page_list::iterator::operator!=(const iterator& o)
    const noexcept -> bool {
  return impl_ != o.impl_;
}

inline auto page_list::iterator::operator*() const noexcept -> reference {
  const page& pg = *impl_;
  return page_range(const_cast<page*>(&pg), pg.npgl_.get());
}

inline auto page_list::iterator::operator->() const noexcept -> pointer {
  return pointer(*impl_);
}


}} /* namespace ilias::vm */

namespace std {

inline auto hash<ilias::vm::page>::operator()(const ilias::vm::page& pg)
    const noexcept -> size_t {
  return hash<ilias::pmap::page_no<ilias::native_arch>>()(pg.address);
}

} /* namespace std */

#endif /* _ILIAS_VM_PAGE_INL_H_ */
