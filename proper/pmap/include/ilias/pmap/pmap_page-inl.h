#ifndef _ILIAS_PMAP_PMAP_PAGE_INL_H_
#define _ILIAS_PMAP_PMAP_PAGE_INL_H_

#include <ilias/pmap/pmap_page.h>
#include <cassert>
#include <mutex>

namespace ilias {
namespace pmap {


template<arch Arch>
constexpr rmap_entry<Arch>::rmap_entry(pmap<Arch>& p, vaddr<Arch> va) noexcept
: pmap_(&p),
  va_(va)
{}


template<arch Arch>
pmap_page<Arch>::pmap_page(page_no<Arch> pgno) noexcept
: address(pgno)
{}

template<arch Arch>
pmap_page<Arch>::~pmap_page() noexcept {
  /*
   * Note:
   * the lock is required, since any pmap may still be able to call
   * into this.
   */
  unlink(true);
}

template<arch Arch>
auto pmap_page<Arch>::linked() const noexcept -> bool {
  std::lock_guard<std::mutex> lck{ guard_ };
  return linked_();
}

template<arch Arch>
auto pmap_page<Arch>::reduce_permissions(bool reduce_kernel, permission perm)
    noexcept -> void {
  std::lock_guard<std::mutex> lck{ guard_ };
  reduce_permissions_(reduce_kernel, perm);
}

template<arch Arch>
auto pmap_page<Arch>::unmap(bool unmap_kernel) noexcept -> void {
  std::lock_guard<std::mutex> lck{ guard_ };
  unmap_(unmap_kernel);
}

template<arch Arch>
auto pmap_page<Arch>::linked_() const noexcept -> bool {
  return !rmap_.empty();
}

template<arch Arch>
auto pmap_page<Arch>::reduce_permissions(bool reduce_kernel, permission perm)
    noexcept -> void {
  /* This page is already locked. */
  using std::next;

  for (rmap_list i_succ, i = rmap_.begin(); i != rmap_.end(); i = i_succ) {
    i_succ = next(i);

    assert(i->pmap_);
    if (reduce_kernel || i->pmap_->userspace()) {
      if (i->pmap_->reduce_permissions(i->va_, *this, perm) ==
          reduce_permission_result::UNMAPPED)
        delete rmap_.unlink(i);
    }
  }
}

template<arch Arch>
auto pmap_page<Arch>::unmap_(bool unmap_kernel) noexcept -> void {
  /* This page is already locked. */
  using std::next;

  for (rmap_list i_succ, i = rmap_.begin(); i != rmap_.end(); i = i_succ) {
    i_succ = next(i);

    assert(i->pmap_);
    if (unmap_kernel || i->pmap_->userspace()) {
      i->pmap_->unmap_no_rmap_(i->va_, *this);
      delete rmap_.unlink(i);
    }
  }
}


}} /* namespace ilias::pmap */

#endif /* _ILIAS_PMAP_PMAP_PAGE_INL_H_ */
