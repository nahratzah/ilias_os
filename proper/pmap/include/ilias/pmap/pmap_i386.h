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
class alignas(2U << 5) pmap<arch::i386> {
 public:
  pmap() noexcept;
  pmap(const pmap&) = delete;
  pmap(pmap&&) noexcept;
  pmap& operator=(const pmap&) = delete;
  pmap& operator=(pmap&&) noexcept;
  ~pmap() noexcept;

  void swap(pmap&) noexcept;

  /* Externally provided. */
  static page_no<arch::i386> allocate_page();
  static void deallocate_page(page_no<arch::i386>) noexcept;
  static vpage_no<arch::i386> map_page(page_no<arch::i386>);
  static void unmap_page(vpage_no<arch::i386>) noexcept;

  phys_addr<arch::i386> virt_to_phys(vaddr<arch::i386>) const;
  std::tuple<page_no<arch::i386>, size_t, uintptr_t> virt_to_page(
      vaddr<arch::i386>) const;
  void map(vpage_no<arch::i386>, page_no<arch::i386>);
  void unmap(vaddr<arch::i386>,
             page_count<arch::i386> = page_count<arch::i386>(1));

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

  static constexpr uint64_t pdpe_mask =
      uint64_t(0) - (uint64_t(1) << pdpe_addr_offset);
  static constexpr uint64_t pdp_mask =
      (uint64_t(1) << pdpe_addr_offset) - (uint64_t(1) << pdp_addr_offset);
  static constexpr uintptr_t pte_mask =
      (uint64_t(1) << pdp_addr_offset) - (uint64_t(1) << pte_addr_offset);

  using pdpe_record = x86_shared::pdpe_record;
  using pdp_record = x86_shared::pdp_record;
  using pte_record = x86_shared::pte_record;

  using pdpe = std::array<pdpe_record, 4>;
  using pdp = std::array<pdp_record, 512>;
  using pte = std::array<pte_record, 512>;

  /* Variables start here. */
  pdpe pdpe_ = {{{ 0 }, { 0 }, { 0 }, { 0 }}};

  static_assert(sizeof(pdpe) == 4 * 8,
                "PDPE table has wrong size.");
  static_assert(sizeof(pdp) == 4 * 1024,
                "PDP table has wrong size.");
  static_assert(sizeof(pte) == 4 * 1024,
                "PTE table has wrong size.");
};

void swap(pmap<arch::i386>&, pmap<arch::i386>&) noexcept;

}} /* namespace ilias::pmap */

#include <ilias/pmap/pmap_i386-inl.h>

#endif /* _ILIAS_PMAP_PMAP_I386_H_ */
