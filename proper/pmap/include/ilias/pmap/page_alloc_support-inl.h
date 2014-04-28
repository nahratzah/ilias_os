#ifndef _ILIAS_PMAP_PAGE_ALLOC_SUPPORT_INL_H_
#define _ILIAS_PMAP_PAGE_ALLOC_SUPPORT_INL_H_

#include <cassert>
#include <ilias/pmap/page_alloc.h>
#include <ilias/pmap/page_alloc_support.h>
#include <ilias/arch.h>
#include <ilias/pmap/page.h>
#include <utility>

namespace ilias {
namespace pmap {


template<arch Arch>
void swap(page_ptr<Arch>& a, page_ptr<Arch>& b) noexcept {
  a.swap(b);
}


template<arch Arch>
page_ptr<Arch>::page_ptr(page_ptr&& other) noexcept
: pgno_(other.pgno_),
  release_on_destruction_(std::move(other.release_on_destruction_)),
  valid_(std::exchange(other.valid_, false))
{}

template<arch Arch>
page_ptr<Arch>::page_ptr(page_no<Arch> pgno) noexcept
: pgno_(pgno),
  valid_(true)
{}

template<arch Arch>
page_ptr<Arch>::~page_ptr() noexcept {
  if (is_allocated()) {
    assert(valid_);
    release_on_destruction_->deallocate_page(pgno_);
  }
}

template<arch Arch>
auto page_ptr<Arch>::operator=(page_ptr&& other) noexcept ->
    page_ptr& {
  page_ptr(std::move(other)).swap(*this);
  return *this;
}

template<arch Arch>
auto page_ptr<Arch>::swap(page_ptr& other) noexcept -> void {
  using std::swap;

  swap(pgno_, other.pgno_);
  swap(valid_, other.valid_);
  swap(release_on_destruction_, other.release_on_destruction_);
}

template<arch Arch>
page_ptr<Arch>::operator bool() const noexcept {
  return valid_;
}

template<arch Arch>
auto page_ptr<Arch>::is_allocated() const noexcept -> bool {
  return release_on_destruction_ != nullptr;
}

template<arch Arch>
auto page_ptr<Arch>::set_allocated(pmap_support<Arch>& support)
    noexcept -> void {
  assert(valid_);
  assert(!release_on_destruction_);
  release_on_destruction_ = &support;
}

template<arch Arch>
auto page_ptr<Arch>::get() const noexcept -> page_no<Arch> {
  assert(valid_);
  return pgno_;
}

template<arch Arch>
auto page_ptr<Arch>::release() noexcept -> page_no<Arch> {
  assert(release_on_destruction_);
  assert(valid_);
  release_on_destruction_ = nullptr;
  return pgno_;
}

template<arch Arch>
auto page_ptr<Arch>::allocate(pmap_support<Arch>& support) ->
    page_ptr {
  page_ptr result;
  result.pgno_ = support.allocate_page();
  result.valid_ = true;
  result.release_on_destruction_ = &support;
  return result;
}


template<typename T, arch Arch>
struct unmap_page_deleter {
 public:
  unmap_page_deleter() noexcept = default;
  unmap_page_deleter(const unmap_page_deleter&) noexcept = default;
  unmap_page_deleter& operator=(const unmap_page_deleter&) noexcept = default;

  unmap_page_deleter(unmap_page_deleter&& other) noexcept
  : support_(std::move(other.support_))
  {}

  unmap_page_deleter& operator=(unmap_page_deleter&& other) noexcept {
    support_ = std::move(other.support_);
    return *this;
  }

  unmap_page_deleter(pmap_support<Arch>* support) noexcept
  : support_(support)
  {
    assert(support_);
  }

  static_assert(sizeof(T) == page_size(Arch), "Not a page.");

  void operator()(T* p) const noexcept {
    assert(support_);
    uintptr_t int_p = reinterpret_cast<uintptr_t>(p);
    support_->unmap_page(vaddr<Arch>(int_p));
  }

 private:
  pmap_support<Arch>* support_ = nullptr;
};

template<typename T, arch Arch>
auto pmap_map_page(page_no<Arch> pg, pmap_support<Arch>& support) ->
    pmap_mapped_ptr<T, Arch> {
  vaddr<Arch> vaddr = support.map_page(pg);
  uintptr_t addr = vaddr.get();
  return pmap_mapped_ptr<T, Arch>(reinterpret_cast<T*>(addr), &support);
}


}} /* namespace ilias::pmap */

#endif /* _ILIAS_PMAP_PAGE_ALLOC_SUPPORT_INL_H_ */
