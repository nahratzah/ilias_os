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
  void map(vpage_no<arch::i386>, page_no<arch::i386>, permission);
  void unmap(vpage_no<arch::i386>,
             page_count<arch::i386> = page_count<arch::i386>(1)) noexcept;

  static constexpr std::array<size_t, 2> N_PAGES = {{ 1, 1 << 9 }};

  const void* get_pmap_ptr() const noexcept { return &pdpe_; }

  /* Worst case, the pmap requires this many pages to hold its information. */
  static constexpr page_count<arch::i386> worst_case_npages =
      page_count<arch::i386>(4 /* PDP */ + 4 * 512 /* PTE */);
  static constexpr vpage_no<arch::i386> kva_map_self =
      vpage_no<arch::i386>(0xffffffff >> page_shift(arch::i386)) -
      worst_case_npages;

  /*
   * The memory range that this pmap can manage.
   *
   * Note that this uses array semantics: the second item points at the
   * first unmanageble page.
   * Also note that if the pmap is a kernel map (support_.userspace == false)
   * the pmap may reduce the manageble range to accomodate its own tracking.
   */
  std::tuple<vpage_no<arch::i386>, vpage_no<arch::i386>> managed_range()
      const noexcept;

 private:
  /*
   * Since the kernel pmap maps itself, these functions calculate the mapped
   * addresses of the pte/pdp.
   */
  static constexpr vpage_no<arch::i386> kva_pdp_entry(unsigned int);
  static constexpr vpage_no<arch::i386> kva_pte_entry(unsigned int,
                                                      unsigned int);
  static constexpr vpage_no<arch::i386> kva_pdp_entry(vaddr<arch::i386>);
  static constexpr vpage_no<arch::i386> kva_pte_entry(vaddr<arch::i386>);

  static constexpr unsigned int offset_bits = 12;
  static constexpr unsigned int pte_offset_bits = 9;
  static constexpr unsigned int pdp_offset_bits = 9;
  static constexpr unsigned int pdpe_offset_bits = 2;

  static constexpr unsigned int pte_addr_offset = offset_bits;
  static constexpr unsigned int pdp_addr_offset = pte_addr_offset +
                                                  pte_offset_bits;
  static constexpr unsigned int pdpe_addr_offset = pdp_addr_offset +
                                                   pdp_offset_bits;

  static constexpr unsigned int N_PDPE = 1U << pdpe_offset_bits;
  static constexpr unsigned int N_PDP = 1U << pdp_offset_bits;
  static constexpr unsigned int N_PTE = 1U << pte_offset_bits;

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
  : public std::array<pdpe_record, N_PDPE>
  {
    pdpe() noexcept;
    pdpe(const pdpe&) noexcept = default;
    pdpe& operator=(const pdpe&) noexcept = default;
  };

  using pdp = std::array<pdp_record, N_PDP>;
  using pte = std::array<pte_record, N_PTE>;

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
