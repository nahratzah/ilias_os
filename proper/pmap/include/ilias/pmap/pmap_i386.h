#ifndef _ILIAS_PMAP_PMAP_I386_H_
#define _ILIAS_PMAP_PMAP_I386_H_

#include <ilias/arch.h>
#include <ilias/pmap/page.h>
#include <ilias/pmap/pmap.h>
#include <ilias/pmap/x86_shared.h>
#include <array>
#include <tuple>

namespace ilias {
namespace pmap {

/* PMAP, using PAE mode. */
template<>
class pmap<arch::i386> {
 public:
  pmap(pmap_support<arch::i386>&) noexcept;
  pmap(const pmap&) = delete;
  pmap& operator=(const pmap&) = delete;
  ~pmap() noexcept;

  void clear() noexcept;

  phys_addr<arch::i386> virt_to_phys(vaddr<arch::i386>) const;
  std::tuple<page_no<arch::i386>, size_t, uintptr_t> virt_to_page(
      vaddr<arch::i386>) const;
  void map(vpage_no<arch::i386>, page_no<arch::i386>);
  void unmap(vpage_no<arch::i386>,
             page_count<arch::i386> = page_count<arch::i386>(1)) noexcept;

  static constexpr std::array<size_t, 2> N_PAGES = {{ 1, 1 << 9 }};

 private:
  static constexpr unsigned int offset_bits = 12;
  static constexpr unsigned int pte_offset_bits = 9;
  static constexpr unsigned int pdp_offset_bits = 9;
  static constexpr unsigned int pdpe_offset_bits = 2;

  static constexpr unsigned int pte_addr_offset = offset_bits;
  static constexpr unsigned int pdp_addr_offset = pte_addr_offset +
                                                  pte_offset_bits;
  static constexpr unsigned int pdpe_addr_offset = pdp_addr_offset +
                                                   pdp_offset_bits;

  static constexpr uint32_t pdpe_mask =
      uint32_t(0) - (uint32_t(1) << pdpe_addr_offset);
  static constexpr uint32_t pdp_mask =
      (uint32_t(1) << pdpe_addr_offset) - (uint32_t(1) << pdp_addr_offset);
  static constexpr uintptr_t pte_mask =
      (uint32_t(1) << pdp_addr_offset) - (uint32_t(1) << pte_addr_offset);

  using pdpe_record = x86_shared::pdpe_record;
  using pdp_record = x86_shared::pdp_record;
  using pte_record = x86_shared::pte_record;

  struct alignas(1 << 5) pdpe
  : public std::array<pdpe_record, 4>
  {
    constexpr pdpe() noexcept
    : std::array<pdpe_record, 4>{{{ 0 }, { 0 }, { 0 }, { 0 }}}
    {}

    constexpr pdpe(const pdpe&) noexcept = default;
    pdpe& operator=(const pdpe&) noexcept = default;
  };

  using pdp = std::array<pdp_record, 512>;
  using pte = std::array<pte_record, 512>;

  /* Variables start here. */
  pdpe pdpe_;
  pmap_support<arch::i386>& support_;

  static_assert(sizeof(pdpe) == 4 * 8,
                "PDPE table has wrong size.");
  static_assert(sizeof(pdp) == 4 * 1024,
                "PDP table has wrong size.");
  static_assert(sizeof(pte) == 4 * 1024,
                "PTE table has wrong size.");
};

}} /* namespace ilias::pmap */

#include <ilias/pmap/pmap_i386-inl.h>

#endif /* _ILIAS_PMAP_PMAP_I386_H_ */
