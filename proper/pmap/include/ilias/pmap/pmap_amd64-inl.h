#ifndef _ILIAS_PMAP_PMAP_AMD64_INL_H_
#define _ILIAS_PMAP_PMAP_AMD64_INL_H_

#include "pmap_amd64.h"
#include <cassert>
#include <algorithm>
#include <utility>
#include <stdexcept>
#include <tuple>

namespace ilias {
namespace pmap {


inline pmap<arch::amd64>::pmap(pmap_support<arch::amd64>& support) noexcept
: support_(support)
{}

inline auto pmap<arch::amd64>::managed_range() const noexcept ->
    std::tuple<vpage_no<arch::amd64>, vpage_no<arch::amd64>> {
  constexpr auto shift = pml4_offset_bits + pdpe_offset_bits +
                         pdp_offset_bits + pte_offset_bits;
  constexpr auto delta = page_count<arch::amd64>(1ULL << shift);

  if (support_.userspace) {
    return std::make_tuple(vpage_no<arch::amd64>(0),
                           vpage_no<arch::amd64>(0) + delta);
  }
  return std::make_tuple(vpage_no<arch::amd64>(0), kva_map_self);
}

constexpr auto pmap<arch::amd64>::kva_pml4_entry() ->
    vpage_no<arch::amd64> {
  return kva_map_self_pml4;
}

constexpr auto pmap<arch::amd64>::kva_pdpe_entry(unsigned int pml4_idx) ->
    vpage_no<arch::amd64> {
  if (pml4_idx >= N_PML4)
    throw std::out_of_range("pml4");

  return kva_map_self_pdpe +
         page_count<arch::amd64>(1) * pml4_idx;
}

constexpr auto pmap<arch::amd64>::kva_pdp_entry(unsigned int pml4_idx,
                                                unsigned int pdpe_idx) ->
    vpage_no<arch::amd64> {
  if (pml4_idx >= N_PML4)
    throw std::out_of_range("pml4");
  if (pdpe_idx >= N_PDPE)
    throw std::out_of_range("pdpe");

  return kva_map_self_pdp +
         page_count<arch::amd64>(N_PDPE) * pml4_idx +
         page_count<arch::amd64>(1) * pdpe_idx;
}

constexpr auto pmap<arch::amd64>::kva_pte_entry(unsigned int pml4_idx,
                                                unsigned int pdpe_idx,
                                                unsigned int pdp_idx) ->
    vpage_no<arch::amd64> {
  if (pml4_idx >= N_PML4)
    throw std::out_of_range("pml4");
  if (pdpe_idx >= N_PDPE)
    throw std::out_of_range("pdpe");
  if (pdp_idx >= N_PDP)
    throw std::out_of_range("pdp");

  return kva_map_self_pte +
         page_count<arch::amd64>(N_PDPE * N_PDP) * pml4_idx +
         page_count<arch::amd64>(N_PDP) * pdpe_idx +
         page_count<arch::amd64>(1) * pdp_idx;
}

constexpr auto pmap<arch::amd64>::kva_pml4_entry(vaddr<arch::amd64>) ->
    vpage_no<arch::amd64> {
  return kva_pml4_entry();
}

constexpr auto pmap<arch::amd64>::kva_pdpe_entry(vaddr<arch::amd64> va) ->
    vpage_no<arch::amd64> {
  return kva_pdpe_entry((va.get() & pml4_mask) >> pml4_addr_offset);
}

constexpr auto pmap<arch::amd64>::kva_pdp_entry(vaddr<arch::amd64> va) ->
    vpage_no<arch::amd64> {
  return kva_pdp_entry((va.get() & pml4_mask) >> pml4_addr_offset,
                       (va.get() & pdpe_mask) >> pdpe_addr_offset);
}

constexpr auto pmap<arch::amd64>::kva_pte_entry(vaddr<arch::amd64> va) ->
    vpage_no<arch::amd64> {
  return kva_pte_entry((va.get() & pml4_mask) >> pml4_addr_offset,
                       (va.get() & pdpe_mask) >> pdpe_addr_offset,
                       (va.get() & pdp_mask) >> pdp_addr_offset);
}


constexpr auto pmap<arch::amd64>::valid_sign_extend(vpage_no<arch::amd64> v)
    noexcept -> bool {
  /* Highest relevant bit is sign extended. */
  return (v.get() & sign_mask) + ((v.get() & sign_choice) << 1) == 0;
}


}} /* namespace ilias::pmap */

#endif /* _ILIAS_PMAP_PMAP_AMD64_INL_H_ */
