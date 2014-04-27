#ifndef _ILIAS_PMAP_PAGE_ALLOC_SUPPORT_INL_H_
#define _ILIAS_PMAP_PAGE_ALLOC_SUPPORT_INL_H_

#include <cassert>
#include <ilias/pmap/page_alloc.h>
#include <ilias/pmap/page_alloc_support.h>
#include <ilias/arch.h>
#include <ilias/pmap/page.h>

namespace ilias {
namespace pmap {


template<arch Arch>
void swap(page_ptr<Arch>& a, page_ptr<Arch>& b) noexcept {
  a.swap(b);
}


template<arch Arch>
page_ptr<Arch>::page_ptr() noexcept
: pgno_(),
  release_on_destruction_(false),
  valid_(false)
{}

template<arch Arch>
page_ptr<Arch>::page_ptr(page_ptr&& other) noexcept
: pgno_(other.pgno_),
  release_on_destruction_(other.release_on_destruction_),
  valid_(other.valid_)
{
  other.release_on_destruction_ = false;
  other.valid_ = false;
}

template<arch Arch>
allocate_page<Arch>::page_ptr(page_no<Arch> pgno) noexcept
: pgno_(pgno),
  valid_(true),
  release_on_destruction_(false)
{}

template<arch Arch>
page_ptr<Arch>::~page_ptr() noexcept {
  if (release_on_destruction_) {
    assert(valid_);
    deallocate_page(pgno_);
  }
}

template<arch Arch>
auto page_ptr<Arch>::operator=(page_ptr&& other) noexcept ->
    page_ptr& {
  page_ptr(move(other)).swap(*this);
  return *this;
}

template<arch Arch>
auto page_ptr<Arch>::swap(page_ptr& other) noexcept {
  using std::swap;

  swap(pgno_, other.pgno_);

  bool valid = valid_;
  valid_ = other.valid_;
  other.valid_ = valid;

  bool release_on_destruction = release_on_destruction_;
  release_on_destruction_ = other.release_on_destruction_;
  other.release_on_destruction_ = release_on_destruction;
}

template<arch Arch>
page_ptr<Arch>::operator bool() const noexcept {
  return valid_;
}

template<arch Arch>
auto page_ptr<Arch>::is_allocated() const noexcept -> bool {
  return release_on_destruction_;
}

template<arch Arch>
auto page_ptr<Arch>::get() const noexcept -> page_no<Arch> {
  assert(valid_);
  return pgno_;
}

template<arch Arch>
auto page_ptr<Arch>::release() noexcept -> page_no<Arch> {
  assert(release_on_destruction_);
  assert(valid);
  release_on_destruction_ = false;
  return pgno_;
}

template<arch Arch>
auto page_ptr<Arch>::allocate() -> page_ptr {
  page_ptr result;
  result.pgno_ = allocate_page();
  result.valid_ = true;
  result.release_on_destruction_ = true;
  return result;
}


}} /* namespace ilias::pmap */

#endif /* _ILIAS_PMAP_PAGE_ALLOC_SUPPORT_INL_H_ */
