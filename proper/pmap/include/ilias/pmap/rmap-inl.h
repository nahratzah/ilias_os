#ifndef _ILIAS_PMAP_RMAP_INL_H_
#define _ILIAS_PMAP_RMAP_INL_H_

#include "rmap.h"
#include <algorithm>
#include <functional>
#include <iterator>

namespace ilias {
namespace pmap {


template<arch Arch>
rmap_entry<Arch>::rmap_entry(pmap<Arch>& pmap, vpage_no<Arch>& addr) noexcept
: pmap_(&pmap),
  addr_(addr)
{}


template<arch PhysArch, arch VirtArch>
rmap<PhysArch, VirtArch>::rmap() noexcept {}

template<arch PhysArch, arch VirtArch>
rmap<PhysArch, VirtArch>::~rmap() noexcept {
  assert(rmap_.empty());
}

template<arch PhysArch, arch VirtArch>
auto rmap<PhysArch, VirtArch>::pmap_register(
    std::unique_ptr<rmap_entry<VirtArch>> p) noexcept -> void {
  assert(p != nullptr);

  rmap_.link(p.release(), true);
}

template<arch PhysArch, arch VirtArch>
auto rmap<PhysArch, VirtArch>::pmap_deregister(pmap<VirtArch>& p,
                                               vpage_no<VirtArch> va)
    noexcept -> std::unique_ptr<rmap_entry<VirtArch>> {
  using std::cref;
  using tuple_type = typename rmap_entry<VirtArch>::tuple_type;

  auto i = rmap_.find(tuple_type(cref(p), va));
  if (i == rmap_.end()) return nullptr;
  return std::unique_ptr<rmap_entry<VirtArch>>(rmap_.unlink(i));
}

template<arch PhysArch, arch VirtArch>
auto rmap<PhysArch, VirtArch>::linked() const noexcept -> bool {
  return !rmap_.empty();
}

template<arch PhysArch, arch VirtArch>
auto rmap<PhysArch, VirtArch>::reduce_permissions(bool reduce_kernel,
                                                  permission perm,
                                                  bool update_ad) noexcept ->
    void {
  using std::next;
  using iter = typename rmap_set::iterator;

  for (iter i_succ, i = rmap_.begin(); i != rmap_.end(); i = i_succ) {
    i_succ = next(i);

    if (reduce_kernel || i->pmap_->userspace()) {
      if (i->pmap_->reduce_permission(i->addr_, perm, update_ad) ==
          reduce_permission_result::UNMAPPED)
        delete rmap_.unlink(i);
    }
  }
}

template<arch PhysArch, arch VirtArch>
auto rmap<PhysArch, VirtArch>::unmap(bool unmap_kernel) noexcept ->
    void {
  using std::next;
  using iter = typename rmap_set::iterator;

  for (iter i_succ, i = rmap_.begin(); i != rmap_.end(); i = i_succ) {
    i_succ = next(i);

    if (unmap_kernel || i->pmap_->userspace()) {
      i->pmap_->unmap(i->addr_);
      delete rmap_.unlink(i);
    }
  }
}

template<arch PhysArch, arch VirtArch>
auto rmap<PhysArch, VirtArch>::flush_accessed_dirty() const noexcept ->
    void {
  std::for_each(rmap_.begin(), rmap_.end(),
                [](const rmap_entry<VirtArch>& e) {
                  e.pmap_->flush_accessed_dirty(e.addr_);
                });
}


template<arch Arch>
auto rmap_entry<Arch>::hash::operator()(const pmap<Arch>* p) const noexcept ->
    size_t {
  using std::hash;

  const hash<pmap<Arch>*> pmap_ptr_hash;
  return pmap_ptr_hash(p);
}

template<arch Arch>
auto rmap_entry<Arch>::hash::operator()(const pmap<Arch>& p) const noexcept ->
    size_t {
  return (*this)(&p);
}

template<arch Arch>
auto rmap_entry<Arch>::hash::operator()(const rmap_entry& e) const noexcept ->
    size_t {
  return (*this)(e.pmap_);
}


template<arch Arch>
auto rmap_entry<Arch>::equality::convert_(tuple_type v) noexcept ->
    tuple_type {
  return v;
}

template<arch Arch>
auto rmap_entry<Arch>::equality::convert_(const rmap_entry& v) noexcept ->
    tuple_type {
  using std::cref;

  return make_tuple(cref(*v.pmap_), v.addr_);
}

template<arch Arch>
auto rmap_entry<Arch>::equality::compare_(const tuple_type& x,
                                          const tuple_type& y) noexcept ->
    bool {
  using std::get;

  return &get<0>(x) == &get<0>(y) && get<1>(x) == get<1>(y);
}

template<arch Arch>
template<typename T, typename U>
auto rmap_entry<Arch>::equality::operator()(const T& x, const U& y)
    const noexcept -> bool {
  return compare_(convert_(x), convert_(y));
}


template<arch Arch>
auto rmap_entry<Arch>::less::convert_(tuple_type v) noexcept ->
    tuple_type {
  return v;
}

template<arch Arch>
auto rmap_entry<Arch>::less::convert_(const rmap_entry& v) noexcept ->
    tuple_type {
  using std::cref;

  return make_tuple(cref(*v.pmap_), v.addr_);
}

template<arch Arch>
auto rmap_entry<Arch>::less::compare_(const tuple_type& x,
                                      const tuple_type& y) noexcept ->
    bool {
  using std::get;

  return &get<0>(x) < &get<0>(y) ||
         (&get<0>(x) == &get<0>(y) && get<1>(x) < get<1>(y));
}

template<arch Arch>
template<typename T, typename U>
auto rmap_entry<Arch>::less::operator()(const T& x, const U& y)
    const noexcept -> bool {
  return compare_(convert_(x), convert_(y));
}


}} /* namespace ilias::pmap */

#endif /* _ILIAS_PMAP_RMAP_INL_H_ */
