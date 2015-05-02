#ifndef _ILIAS_PMAP_PMAP_PAGE_INL_H_
#define _ILIAS_PMAP_PMAP_PAGE_INL_H_

#include "pmap_page.h"
#include <cassert>
#include <mutex>
#include <array>
#include <algorithm>

namespace ilias {
namespace pmap {
namespace impl {


template<arch PhysArch, arch VirtArch>
bool linked_(const rmap<PhysArch, VirtArch>& r) noexcept {
  return r.linked();
}

template<arch PhysArch, arch VirtArch>
void reduce_permissions_(rmap<PhysArch, VirtArch>& r,
                         bool reduce_kernel, permission perm) noexcept {
  r.reduce_permissions(reduce_kernel, perm);
}

template<arch PhysArch, arch VirtArch>
void unmap(rmap<PhysArch, VirtArch>& r, bool unmap_kernel) noexcept {
  r.unmap(unmap_kernel);
}


template<arch PhysArch, arch... VirtArch, size_t... N>
bool linked_(const std::tuple<rmap<PhysArch, VirtArch>...>& t,
             std::index_sequence<N...>) noexcept {
  using std::array;
  using std::any_of;
  using std::get;

  array<bool, sizeof...(N)> v{{ linked_(get<N>(t))... }};
  return any_of(v.begin(), v.end());
}


template<arch PhysArch, arch VirtArch, typename... Rmap>
void reduce_permissions_(bool reduce_kernel, permission perm,
                         rmap<PhysArch, VirtArch>& r0, Rmap&... r) noexcept {
  reduce_permissions_(r0, reduce_kernel, perm);
  reduce_permissions_(r..., reduce_kernel, perm);
}

template<arch PhysArch, arch... VirtArch, size_t... N>
bool reduce_permissions_(bool reduce_kernel, permission perm,
                         const std::tuple<rmap<PhysArch, VirtArch>...>& t,
                         std::index_sequence<N...>) noexcept {
  using std::get;

  reduce_permissions_(reduce_kernel, perm, get<N>(t)...);
}


template<arch PhysArch, arch VirtArch, typename... Rmap>
void unmap_(bool unmap_kernel, rmap<PhysArch, VirtArch>& r0, Rmap&... r)
    noexcept {
  unmap_(unmap_kernel, r0);
  unmap_(unmap_kernel, r...);
}

template<arch PhysArch, arch... VirtArch, size_t... N>
bool unmap_(bool unmap_kernel,
            const std::tuple<rmap<PhysArch, VirtArch>...>& t,
            std::index_sequence<N...>) noexcept {
  using std::get;

  unmap_(unmap_kernel, get<N>(t)...);
}


} /* namespace ilias::pmap::impl */


template<arch PhysArch, arch... VirtArch>
pmap_page<PhysArch, arch_set<VirtArch...>>::pmap_page(page_no<PhysArch> pgno)
    noexcept
: address(pgno)
{}

template<arch PhysArch, arch... VirtArch>
auto pmap_page<PhysArch, arch_set<VirtArch...>>::linked() const noexcept ->
    bool {
  std::lock_guard<std::mutex> lck{ guard_ };
  return linked_();
}

template<arch PhysArch, arch... VirtArch>
auto pmap_page<PhysArch, arch_set<VirtArch...>>::reduce_permissions(
    bool reduce_kernel, permission perm) noexcept -> void {
  std::lock_guard<std::mutex> lck{ guard_ };
  reduce_permissions_(reduce_kernel, perm);
}

template<arch PhysArch, arch... VirtArch>
auto pmap_page<PhysArch, arch_set<VirtArch...>>::unmap(bool unmap_kernel)
    noexcept -> void {
  std::lock_guard<std::mutex> lck{ guard_ };
  unmap_(unmap_kernel);
}

template<arch PhysArch, arch... VirtArch>
auto pmap_page<PhysArch, arch_set<VirtArch...>>::linked_() const noexcept ->
    bool {
  /* This page is already locked. */
  return impl::linked_(rmap_,
                       std::index_sequence_for<decltype(VirtArch)...>());
}

template<arch PhysArch, arch... VirtArch>
auto pmap_page<PhysArch, arch_set<VirtArch...>>::reduce_permissions_(
    bool reduce_kernel, permission perm) noexcept -> void {
  /* This page is already locked. */
  impl::reduce_permissions_(reduce_kernel, perm,
                            rmap_,
                            std::index_sequence_for<decltype(VirtArch)...>());
}

template<arch PhysArch, arch... VirtArch>
auto pmap_page<PhysArch, arch_set<VirtArch...>>::unmap_(bool unmap_kernel)
    noexcept -> void {
  /* This page is already locked. */
  impl::unmap_(unmap_kernel, rmap_,
               std::index_sequence_for<decltype(VirtArch)...>());
}


}} /* namespace ilias::pmap */

#endif /* _ILIAS_PMAP_PMAP_PAGE_INL_H_ */
