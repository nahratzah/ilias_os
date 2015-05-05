#ifndef _ILIAS_PMAP_PMAP_AMD64_H_
#define _ILIAS_PMAP_PMAP_AMD64_H_

#include <ilias/arch.h>
#include <ilias/pmap/page.h>
#include <ilias/pmap/pmap.h>
#include <ilias/pmap/x86_shared.h>
#include <ilias/pmap/page_alloc_support.h>
#include <array>
#include <tuple>

namespace ilias {
namespace pmap {


/* PMAP for amd64. */
template<> class pmap<arch::amd64> final {
  friend pmap_map<pmap<arch::amd64>>;

 public:
  pmap(pmap_support<arch::amd64>&) noexcept;
  pmap(const pmap&) = delete;
  pmap& operator=(const pmap&) = delete;
  ~pmap() noexcept;

  void clear() noexcept;
  bool userspace() const noexcept { return support_.userspace; }
  pmap_support<arch::amd64>& get_support() const noexcept { return support_; }

  phys_addr<arch::amd64> virt_to_phys(vaddr<arch::amd64>) const;
  std::tuple<page_no<arch::amd64>, size_t, uintptr_t> virt_to_page(
      vaddr<arch::amd64>) const;

  reduce_permission_result reduce_permission(vpage_no<arch::amd64>,
                                             permission,
                                             bool = false);
  void map(vpage_no<arch::amd64>, page_no<arch::amd64>, permission);
  void unmap(vpage_no<arch::amd64>,
             page_count<arch::amd64> = page_count<arch::amd64>(1));
  void flush_accessed_dirty(vpage_no<arch::amd64>) noexcept;

 private:
  reduce_permission_result reduce_permission_(vpage_no<arch::amd64>,
                                              permission, bool) noexcept;
  void map_(vpage_no<arch::amd64>, page_no<arch::amd64>, permission);
  void unmap_(vpage_no<arch::amd64>, page_count<arch::amd64>,
              bool do_deregister) noexcept;
  void flush_accessed_dirty_(vpage_no<arch::amd64>) noexcept;

 public:
  static constexpr std::array<size_t, 3> N_PAGES = {{ 1, 1 << 9, 1 << 18 }};
  static constexpr bool valid_sign_extend(vpage_no<arch::amd64>) noexcept;

  page_no<arch::amd64> get_pmap_ptr() const noexcept { return pml4_; }

  /*
   * The memory range that this pmap can manage.
   *
   * Note that this uses array semantics: the second item points at the
   * first unmanageble page.
   * Also note that if the pmap is a kernel map (support_.userspace == false)
   * the pmap may reduce the manageble range to accomodate its own tracking.
   */
  std::tuple<vpage_no<arch::amd64>, vpage_no<arch::amd64>> managed_range()
      const noexcept;

 private:
  /*
   * Since the kernel pmap maps itself, these functions calculate the mapped
   * addresses of the pdpe/pte/pdp.
   */
  static constexpr vpage_no<arch::amd64> kva_pml4_entry();
  static constexpr vpage_no<arch::amd64> kva_pdpe_entry(unsigned int);
  static constexpr vpage_no<arch::amd64> kva_pdp_entry(unsigned int,
                                                       unsigned int);
  static constexpr vpage_no<arch::amd64> kva_pte_entry(unsigned int,
                                                       unsigned int,
                                                       unsigned int);
  static constexpr vpage_no<arch::amd64> kva_pml4_entry(vaddr<arch::amd64>);
  static constexpr vpage_no<arch::amd64> kva_pdpe_entry(vaddr<arch::amd64>);
  static constexpr vpage_no<arch::amd64> kva_pdp_entry(vaddr<arch::amd64>);
  static constexpr vpage_no<arch::amd64> kva_pte_entry(vaddr<arch::amd64>);

  /* Alignment constraint (pageno) for large pages at PDPE/PDP level. */
  static constexpr uint64_t lp_pdpe_pgno_align = 1ULL << 18;
  static constexpr uint64_t lp_pdp_pgno_align = 1ULL << 9;

  /* Numer of bits describing indices for each level. */
  static constexpr unsigned int offset_bits = 12;
  static constexpr unsigned int pte_offset_bits = 9;
  static constexpr unsigned int pdp_offset_bits = 9;
  static constexpr unsigned int pdpe_offset_bits = 9;
  static constexpr unsigned int pml4_offset_bits = 9;

  /* Shifts for converting addresses to page-table indices. */
  static constexpr unsigned int pte_addr_offset = offset_bits;
  static constexpr unsigned int pdp_addr_offset = pte_addr_offset +
                                                  pte_offset_bits;
  static constexpr unsigned int pdpe_addr_offset = pdp_addr_offset +
                                                   pdp_offset_bits;
  static constexpr unsigned int pml4_addr_offset = pdpe_addr_offset +
                                                   pdpe_offset_bits;
  static constexpr unsigned int pml4_addr_off_end = pml4_addr_offset +
                                                    pml4_offset_bits;

  /* AMD64 pointers are sign-extended. */
  static constexpr uint64_t sign_mask =
      uint64_t(0) - (uint64_t(1) << pml4_addr_off_end);
  static constexpr uint64_t sign_choice =
      uint64_t(1) << (pml4_addr_off_end - 1U);

  /* Masks for converting addresses to page-table indices. */
  static constexpr uint64_t pml4_mask =
      (uint64_t(1) << pml4_addr_off_end) - (uint64_t(1) << pml4_addr_offset);
  static constexpr uint64_t pdpe_mask =
      (uint64_t(1) << pml4_addr_offset) - (uint64_t(1) << pdpe_addr_offset);
  static constexpr uint64_t pdp_mask =
      (uint64_t(1) << pdpe_addr_offset) - (uint64_t(1) << pdp_addr_offset);
  static constexpr uint64_t pte_mask =
      (uint64_t(1) << pdp_addr_offset) - (uint64_t(1) << pte_addr_offset);

  /* Number of records in each level. */
  static constexpr unsigned int N_PML4 = 1U << pml4_offset_bits;
  static constexpr unsigned int N_PDPE = 1U << pdpe_offset_bits;
  static constexpr unsigned int N_PDP = 1U << pdp_offset_bits;
  static constexpr unsigned int N_PTE = 1U << pte_offset_bits;

  /* Worst case, the pmap requires this many pages to hold its information. */
  static constexpr page_count<arch::amd64> worst_case_npages =
      page_count<arch::amd64>(1 + N_PML4 * (1 + N_PDPE * (1 + N_PDP)));

  /*
   * Memory layout for kva self-mapping:
   *
   * address            | data                          | #pages
   * -------------------+-------------------------------------------------
   * 0xffffffffffffffff |                               |
   *                    | pml4                          | 1
   * kva_map_self_pml4  |                               |
   *                    | pml4[0..1) pdpe[0..1024)      | N_PML4
   * kva_map_self_pdpe  |                               |
   *                    | pml4[0..1) pdpe[0..1024)      | N_PML4 * N_PDPE
   *                    |  pdp[0..1024)                 |
   * kva_map_self_pdp   |                               |
   *                    | pml4[0..1) pdpe[0..1024)      | N_PML4 * N_PDPE
   *                    |  pdp[0..1024) pte[0..1024)    |  * N_PDP
   * kva_map_self_pte   |                               |
   *   = kva_map_self   |                               |
   */
  static constexpr vpage_no<arch::amd64> kva_map_self =
      vpage_no<arch::amd64>(0xffffffffffffffffULL >> page_shift(arch::amd64)) -
      worst_case_npages;
  static constexpr vpage_no<arch::amd64> kva_map_self_pml4 =
      vpage_no<arch::amd64>(0xffffffffffffffffULL >> page_shift(arch::amd64)) -
      page_count<arch::amd64>(1);
  static constexpr vpage_no<arch::amd64> kva_map_self_pdpe =
      kva_map_self_pml4 -
      page_count<arch::amd64>(1) * N_PML4;
  static constexpr vpage_no<arch::amd64> kva_map_self_pdp =
      kva_map_self_pdpe -
      page_count<arch::amd64>(1) * N_PML4 * N_PDPE;
  static constexpr vpage_no<arch::amd64> kva_map_self_pte =
      kva_map_self_pdp -
      page_count<arch::amd64>(1) * N_PML4 * N_PDPE * N_PDP;

  /*
   * Use AVL bit 0 to mark pte's/pdp's/pdpe's as critical.
   * Critical pte's/pdp's/pdpe's won't be garbage collected during unmap.
   */
  static const unsigned int AVL_CRITICAL = 0;

  /* Declare our record types. */
  using pml4_record = x86_shared::pml4_record;
  using pdpe_record = x86_shared::pdpe_record<arch::amd64>;
  using pdp_record = x86_shared::pdp_record;
  using pte_record = x86_shared::pte_record;

  /* Declare types for PDPE, PDP and PTE levels. */
  using pml4 = std::array<pml4_record, N_PDPE>;
  using pdpe = std::array<pdpe_record, N_PDPE>;
  using pdp = std::array<pdp_record, N_PDP>;
  using pte = std::array<pte_record, N_PTE>;

  /*
   * Page mapping wrapper, which uses the kva_{pdp,pte}_entry addresses if the
   * page_map is a kernel pmap.
   */
  auto map_pml4(page_no<arch::amd64>, vaddr<arch::amd64>) const ->
      pmap_mapped_ptr<pml4, arch::amd64>;
  auto map_pdpe(page_no<arch::amd64>, vaddr<arch::amd64>) const ->
      pmap_mapped_ptr<pdpe, arch::amd64>;
  auto map_pdp(page_no<arch::amd64>, vaddr<arch::amd64>) const ->
      pmap_mapped_ptr<pdp, arch::amd64>;
  auto map_pte(page_no<arch::amd64>, vaddr<arch::amd64>) const ->
      pmap_mapped_ptr<pte, arch::amd64>;
  auto map_pml4(page_no<arch::amd64>) const ->
      pmap_mapped_ptr<pml4, arch::amd64>;
  auto map_pdpe(page_no<arch::amd64>, unsigned int) const ->
      pmap_mapped_ptr<pdpe, arch::amd64>;
  auto map_pdp(page_no<arch::amd64>, unsigned int, unsigned int) const ->
      pmap_mapped_ptr<pdp, arch::amd64>;
  auto map_pte(page_no<arch::amd64>, unsigned int, unsigned int, unsigned int)
      const -> pmap_mapped_ptr<pte, arch::amd64>;

  void maybe_gc(
      std::tuple<page_ptr<arch::amd64>&,
                 pmap_mapped_ptr<pml4, arch::amd64>&,
                 pml4_record&>,
      std::tuple<page_ptr<arch::amd64>&,
                 pmap_mapped_ptr<pdpe, arch::amd64>&,
                 pdpe_record&>,
      std::tuple<page_ptr<arch::amd64>&,
                 pmap_mapped_ptr<pdp, arch::amd64>&,
                 pdp_record&>,
      std::tuple<page_ptr<arch::amd64>&,
                 pmap_mapped_ptr<pte, arch::amd64>&,
                 pte_record&>)
      noexcept;
  void maybe_gc(
      std::tuple<page_ptr<arch::amd64>&,
                 pmap_mapped_ptr<pml4, arch::amd64>&,
                 pml4_record&>,
      std::tuple<page_ptr<arch::amd64>&,
                 pmap_mapped_ptr<pdpe, arch::amd64>&,
                 pdpe_record&>,
      std::tuple<page_ptr<arch::amd64>&,
                 pmap_mapped_ptr<pdp, arch::amd64>&,
                 pdp_record&>)
      noexcept;
  void maybe_gc(
      std::tuple<page_ptr<arch::amd64>&,
                 pmap_mapped_ptr<pml4, arch::amd64>&,
                 pml4_record&>,
      std::tuple<page_ptr<arch::amd64>&,
                 pmap_mapped_ptr<pdpe, arch::amd64>&,
                 pdpe_record&>)
      noexcept;
  void maybe_gc(
      std::tuple<page_ptr<arch::amd64>&,
                 pmap_mapped_ptr<pml4, arch::amd64>&,
                 pml4_record&>)
      noexcept;

  void break_large_page_(pdpe_record&, vaddr<arch::amd64>);
  void break_large_page_(pdp_record&, vaddr<arch::amd64>);

  void deregister_from_pg_(
      page_no<arch::amd64>,
      vpage_no<arch::amd64>,
      bool, bool,
      page_count<arch::amd64> = page_count<arch::amd64>(1)) noexcept;
  void add_flags_to_pg_(
      page_no<arch::amd64>, bool, bool,
      page_count<arch::amd64> = page_count<arch::amd64>(1)) noexcept;

  /* Variables start here. */
  page_no<arch::amd64> pml4_;
  pmap_support<arch::amd64>& support_;
  bool kva_map_self_enabled_ = false;

  /*
   * Verify that everything behaves as planned.
   */
  static_assert(sizeof(pml4) == 4 * 1024,
                "PML4E table has wrong size.");
  static_assert(sizeof(pdpe) == 4 * 1024,
                "PDPE table has wrong size.");
  static_assert(sizeof(pdp) == 4 * 1024,
                "PDP table has wrong size.");
  static_assert(sizeof(pte) == 4 * 1024,
                "PTE table has wrong size.");

  /*
   * Rigorous validation of masks.
   */
  static_assert((pml4_mask | pdpe_mask | pdp_mask | pte_mask |
                 page_mask(arch::amd64)) ==
                0xffffffffffffU,
                "Masks must connect and describe 48 bits of address space.");
  static_assert((pml4_mask & pdpe_mask) == 0 &&
                (pdpe_mask & pdp_mask) == 0 &&
                (pdp_mask & pte_mask) == 0 &&
                (pte_mask & page_mask(arch::amd64)) == 0,
                "Masks may not overlap.");
  static_assert(pml4_mask > pdpe_mask && pdpe_mask > pdp_mask &&
                pdp_mask > pte_mask && pte_mask > page_mask(arch::amd64),
                "Masks must be ordered: PDPE > PDP > PTE > page_mask.");
  static_assert(offset_bits == page_shift(arch::amd64),
                "Page shift or offset_bits are wrong.");

  /*
   * Validate self-mapping calculations.
   */
  static_assert(kva_map_self == kva_map_self_pte,
                "Self-mapping calculation went wrong.");
};

template<> class pmap_map<pmap<arch::amd64>> {
 private:
  static constexpr unsigned int N_PML4 = pmap<arch::amd64>::N_PML4;
  static constexpr unsigned int N_PDPE = pmap<arch::amd64>::N_PDPE;
  static constexpr unsigned int N_PDP = pmap<arch::amd64>::N_PDP;
  static constexpr unsigned int N_PTE = pmap<arch::amd64>::N_PTE;

  static constexpr unsigned int pml4_addr_offset =
      pmap<arch::amd64>::pml4_addr_offset;
  static constexpr unsigned int pdpe_addr_offset =
      pmap<arch::amd64>::pdpe_addr_offset;
  static constexpr unsigned int pdp_addr_offset =
      pmap<arch::amd64>::pdp_addr_offset;
  static constexpr unsigned int pte_addr_offset =
      pmap<arch::amd64>::pte_addr_offset;
  static constexpr uint64_t pml4_mask = pmap<arch::amd64>::pml4_mask;
  static constexpr uint64_t pdpe_mask = pmap<arch::amd64>::pdpe_mask;
  static constexpr uint64_t pdp_mask = pmap<arch::amd64>::pdp_mask;
  static constexpr uint64_t pte_mask = pmap<arch::amd64>::pte_mask;

  using pml4_record = pmap<arch::amd64>::pml4_record;
  using pdpe_record = pmap<arch::amd64>::pdpe_record;
  using pdp_record = pmap<arch::amd64>::pdp_record;
  using pte_record = pmap<arch::amd64>::pte_record;
  using pml4 = pmap<arch::amd64>::pml4;
  using pdpe = pmap<arch::amd64>::pdpe;
  using pdp = pmap<arch::amd64>::pdp;
  using pte = pmap<arch::amd64>::pte;

 public:
  pmap_map() noexcept = default;
  pmap_map(const pmap_map&) = delete;
  pmap_map& operator=(const pmap_map&) = delete;
  pmap_map(pmap<arch::amd64>&, vpage_no<arch::amd64>, vpage_no<arch::amd64>);

  void push_back(page_no<arch::amd64>, permission,
                 page_count<arch::amd64> = page_count<arch::amd64>(1));
  void commit();
  page_count<arch::amd64> size() const noexcept;
  page_count<arch::amd64> max_size() const noexcept;

 private:
  void load_pml4_ptr() const;
  void load_pdpe_ptr_(size_t) const;
  void load_pdp_ptr_(size_t, size_t) const;
  void load_pte_ptr_(size_t, size_t, size_t) const;
  bool pte_is_lp_convertible(size_t, size_t) const;
  bool pte_is_hp_convertible(size_t, size_t) const;

  pmap<arch::amd64>* pmap_ = nullptr;
  vpage_no<arch::amd64> va_start_{}, va_end_{}, va_{};
  bool commit_ = false;

  /* The pointers for PML4E, PDPE, PDP and PTE are cached between calls. */
  mutable pmap_mapped_ptr<pmap<arch::amd64>::pml4, arch::amd64> pml4_ptr_;
  mutable pmap_mapped_ptr<pmap<arch::amd64>::pdpe, arch::amd64> pdpe_ptr_;
  mutable pmap_mapped_ptr<pmap<arch::amd64>::pdp, arch::amd64> pdp_ptr_;
  mutable pmap_mapped_ptr<pmap<arch::amd64>::pte, arch::amd64> pte_ptr_;
};


}} /* namespace ilias::pmap */

#include "pmap_amd64-inl.h"

#endif /* _ILIAS_PMAP_PMAP_AMD64_H_ */
