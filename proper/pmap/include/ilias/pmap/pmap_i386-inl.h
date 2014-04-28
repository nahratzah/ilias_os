#ifndef _ILIAS_PMAP_PMAP_I386_INL_H_
#define _ILIAS_PMAP_PMAP_I386_INL_H_

#include <ilias/pmap/pmap_i386.h>
#include <cassert>
#include <utility>

namespace ilias {
namespace pmap {

inline pmap<arch::i386>::pmap(pmap_support<arch::i386>& support) noexcept
: support_(support)
{
  static_assert(alignof(pmap<arch::i386>) >= (1 << 5),
                "PMAP requires strict alignment "
                "(lowest 5 bits in PDPE pointer are essentially zero).");
  static_assert(offsetof(pmap<arch::i386>, pdpe_) % (1 << 5) == 0,
                "PDPE must be properly aligned.");
  assert_msg(reinterpret_cast<uintptr_t>(this) % (1 << 5) == 0,
             "Mis-aligned pmap.");
}

}} /* namespace ilias::pmap */

#endif /* _ILIAS_PMAP_PMAP_I386_INL_H_ */
