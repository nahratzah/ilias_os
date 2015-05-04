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
void reduce_permissions_(bool reduce_kernel, permission perm,
                         rmap<PhysArch, VirtArch>& r) noexcept {
  r.reduce_permissions(reduce_kernel, perm);
}

template<arch PhysArch, arch VirtArch>
void unmap_(bool unmap_kernel, rmap<PhysArch, VirtArch>& r) noexcept {
  r.unmap(unmap_kernel);
}

template<arch PhysArch, arch VirtArch>
void flush_accessed_dirty_(std::tuple<bool, bool>& out,
                           rmap<PhysArch, VirtArch>& r0) noexcept {
  using std::get;
  using std::tie;

  bool a, d;
  tie(a, d) = r0.flush_accessed_dirty();
  get<0>(out) |= a;
  get<1>(out) |= d;
}


template<arch PhysArch, arch... VirtArch, size_t... N>
bool linked_(const std::tuple<rmap<PhysArch, VirtArch>...>& t,
             std::index_sequence<N...>) noexcept {
  using std::array;
  using std::any_of;
  using std::get;

  array<bool, sizeof...(N)> v{{ linked_(get<N>(t))... }};
  return any_of(v.begin(), v.end(), [](bool b) -> bool { return b; });
}


template<arch PhysArch, arch VirtArch, typename... Rmap>
void reduce_permissions_(bool reduce_kernel, permission perm,
                         rmap<PhysArch, VirtArch>& r0, Rmap&... r) noexcept {
  reduce_permissions_(reduce_kernel, perm, r0);
  reduce_permissions_(reduce_kernel, perm, r...);
}

template<arch PhysArch, arch... VirtArch, size_t... N>
void reduce_permissions_(bool reduce_kernel, permission perm,
                         std::tuple<rmap<PhysArch, VirtArch>...>& t,
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
void unmap_(bool unmap_kernel,
            std::tuple<rmap<PhysArch, VirtArch>...>& t,
            std::index_sequence<N...>) noexcept {
  using std::get;

  unmap_(unmap_kernel, get<N>(t)...);
}


template<arch PhysArch, arch VirtArch, typename... Rmap>
void flush_accessed_dirty_(std::tuple<bool, bool>& out,
                           rmap<PhysArch, VirtArch>& r0, Rmap&... r)
    noexcept {
  using std::get;
  using std::tie;

  {
    bool a, d;
    tie(a, d) = r0.flush_accessed_dirty();
    get<0>(out) |= a;
    get<1>(out) |= d;
  }
  flush_accessed_dirty_(out, r...);
}

template<arch PhysArch, arch... VirtArch, size_t... N>
void flush_accessed_dirty_(std::tuple<bool, bool>& out,
                           std::tuple<rmap<PhysArch, VirtArch>...>& t,
                           std::index_sequence<N...>) noexcept {
  using std::get;

  flush_accessed_dirty_(out, get<N>(t)...);
}


} /* namespace ilias::pmap::impl */


template<arch PhysArch, arch... VirtArch>
pmap_page_tmpl<PhysArch, arch_set<VirtArch...>>::pmap_page_tmpl() noexcept {}

template<arch PhysArch, arch... VirtArch>
auto pmap_page_tmpl<PhysArch, arch_set<VirtArch...>>::linked_()
    const noexcept -> bool {
  /* This page is already locked. */
  return impl::linked_(rmap_,
                       std::index_sequence_for<decltype(VirtArch)...>());
}

template<arch PhysArch, arch... VirtArch>
auto pmap_page_tmpl<PhysArch, arch_set<VirtArch...>>::reduce_permissions_(
    bool reduce_kernel, permission perm) noexcept -> void {
  /* This page is already locked. */
  impl::reduce_permissions_(reduce_kernel, perm,
                            rmap_,
                            std::index_sequence_for<decltype(VirtArch)...>());
}

template<arch PhysArch, arch... VirtArch>
auto pmap_page_tmpl<PhysArch, arch_set<VirtArch...>>::unmap_(bool unmap_kernel)
    noexcept -> void {
  /* This page is already locked. */
  impl::unmap_(unmap_kernel, rmap_,
               std::index_sequence_for<decltype(VirtArch)...>());
}

template<arch PhysArch, arch... VirtArch>
auto pmap_page_tmpl<PhysArch, arch_set<VirtArch...>>::flush_accessed_dirty_()
    noexcept -> std::tuple<bool, bool> {
  /* This page is already locked. */
  std::tuple<bool, bool> rv = std::make_tuple(false, false);
  impl::flush_accessed_dirty_(
      rv, rmap_, std::index_sequence_for<decltype(VirtArch)...>());
  return rv;
}

template<arch PhysArch, arch... VirtArch>
template<arch VA>
auto pmap_page_tmpl<PhysArch, arch_set<VirtArch...>>::pmap_deregister_(
    pmap<VA>& p, vpage_no<VA> va) noexcept -> void {
  using std::get;

  /* This page is already locked. */
  get<rmap<PhysArch, VA>>(rmap_).pmap_deregister(p, va);
}


inline auto pmap_page::address() const noexcept -> page_no<native_arch> {
  return page_no<native_arch>(address_and_ad_.load(std::memory_order_relaxed) &
                              pgno_mask);
}

inline auto pmap_page::mark_accessed() noexcept -> void {
  address_and_ad_.fetch_or(accessed_mask, std::memory_order_relaxed);
}

inline auto pmap_page::mark_dirty() noexcept -> void {
  address_and_ad_.fetch_or(dirty_mask, std::memory_order_relaxed);
}

inline auto pmap_page::mark_accessed_and_dirty() noexcept -> void {
  address_and_ad_.fetch_or(accessed_mask | dirty_mask,
                           std::memory_order_relaxed);
}

inline auto pmap_page::get_accessed_dirty() noexcept ->
    std::tuple<bool, bool> {
  constexpr auto mask = accessed_mask | dirty_mask;

  const auto m = address_and_ad_.load(std::memory_order_relaxed);
  if ((m & mask) == mask) return std::make_tuple(true, true);
  return flush_accessed_dirty();
}


}} /* namespace ilias::pmap */

#endif /* _ILIAS_PMAP_PMAP_PAGE_INL_H_ */
