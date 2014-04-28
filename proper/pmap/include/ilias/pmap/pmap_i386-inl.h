#ifndef _ILIAS_PMAP_PMAP_I386_INL_H_
#define _ILIAS_PMAP_PMAP_I386_INL_H_

#include <ilias/pmap/pmap_i386.h>
#include <utility>

namespace ilias {
namespace pmap {

inline pmap<arch::i386>::pmap() noexcept {
  static_assert(offsetof(pmap<arch::i386>, pdpe_) == 0,
                "PDPE must be the first entry in pmap.");
}

inline pmap<arch::i386>::pmap(pmap&& other) noexcept
: pmap()
{
  other.swap(*this);
}

inline auto pmap<arch::i386>::operator=(pmap&& other) noexcept -> pmap& {
  pmap(std::move(other)).swap(*this);
  return *this;
}

inline auto pmap<arch::i386>::swap(pmap& other) noexcept -> void {
  using std::swap;

  swap(pdpe_, other.pdpe_);
}

inline auto swap(pmap<arch::i386>& a, pmap<arch::i386>& b) noexcept -> void {
  a.swap(b);
}

}} /* namespace ilias::pmap */

#endif /* _ILIAS_PMAP_PMAP_I386_INL_H_ */
