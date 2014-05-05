#ifndef _ILIAS_PMAP_PMAP_I386_INL_H_
#define _ILIAS_PMAP_PMAP_I386_INL_H_

#include <ilias/pmap/pmap_i386.h>
#include <cassert>
#include <algorithm>
#include <utility>
#include <stdexcept>

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

inline auto pmap<arch::i386>::managed_range() const noexcept ->
    std::tuple<vpage_no<arch::i386>, vpage_no<arch::i386>> {
  vpage_no<arch::i386> top = kva_map_self;
  if (support_.userspace) {
    uint32_t top_addr = (0xffffffffU >> page_shift(arch::i386));
    top = vpage_no<arch::i386>(top_addr + 1U);
  }
  return std::make_tuple(vpage_no<arch::i386>(0), top);
}

constexpr auto pmap<arch::i386>::kva_pdp_entry(unsigned int pdpe_idx) ->
    vpage_no<arch::i386> {
  if (pdpe_idx >= N_PDPE)
    throw std::out_of_range("pdpe");

  return kva_map_self + page_count<arch::i386>(pdpe_idx);
}

constexpr auto pmap<arch::i386>::kva_pte_entry(unsigned pdpe_idx,
                                               unsigned int pdp_idx) ->
    vpage_no<arch::i386> {
  if (pdpe_idx >= N_PDPE)
    throw std::out_of_range("pdpe");
  if (pdp_idx >= N_PDP)
    throw std::out_of_range("pdp");

  return kva_map_self +
         page_count<arch::i386>(N_PDPE) +
         page_count<arch::i386>(N_PDP) * pdpe_idx +
         page_count<arch::i386>(pdp_idx);
}

constexpr auto pmap<arch::i386>::kva_pdp_entry(vaddr<arch::i386> va) ->
    vpage_no<arch::i386> {
  return kva_pdp_entry((va.get() & pdpe_mask) >> pdpe_addr_offset);
}

constexpr auto pmap<arch::i386>::kva_pte_entry(vaddr<arch::i386> va) ->
    vpage_no<arch::i386> {
  return kva_pte_entry((va.get() & pdpe_mask) >> pdpe_addr_offset,
                       (va.get() & pdp_mask) >> pdp_addr_offset);
}


inline pmap<arch::i386>::pdpe::pdpe() noexcept
: std::array<pdpe_record, N_PDPE>()
{
  std::fill_n(begin(), size(), pdpe_record{ 0 });
}

}} /* namespace ilias::pmap */

#endif /* _ILIAS_PMAP_PMAP_I386_INL_H_ */
