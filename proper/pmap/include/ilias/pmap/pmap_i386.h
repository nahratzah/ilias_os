#ifndef _ILIAS_PMAP_PMAP_I386_H_
#define _ILIAS_PMAP_PMAP_I386_H_

#include <ilias/arch.h>
#include <ilias/pmap/page.h>
#include <ilias/pmap/pmap.h>
#include <ilias/pmap/x86_shared.h>
#include <ilias/pmap/page_alloc_support.h>
#include <array>
#include <tuple>

namespace ilias {
namespace pmap {


/* PMAP, using PAE mode. */
template<> class pmap<arch::i386> final {
  friend pmap_map<pmap<arch::i386>>;

 public:
  pmap(pmap_support<arch::i386>&) noexcept;
  pmap(const pmap&) = delete;
  pmap& operator=(const pmap&) = delete;
  ~pmap() noexcept;

  void clear() noexcept;
  bool userspace() const noexcept { return support_.userspace; }

  phys_addr<arch::i386> virt_to_phys(vaddr<arch::i386>) const;
  std::tuple<page_no<arch::i386>, size_t, uintptr_t> virt_to_page(
      vaddr<arch::i386>) const;

  reduce_permission_result reduce_permission(vpage_no<arch::i386>, permission);
  void map(vpage_no<arch::i386>, page_no<arch::i386>, permission);
  void unmap(vpage_no<arch::i386>,
             page_count<arch::i386> = page_count<arch::i386>(1));

 private:
  reduce_permission_result reduce_permission_(vpage_no<arch::i386>,
                                              permission) noexcept;
  void map_(vpage_no<arch::i386>, page_no<arch::i386>, permission);
  void unmap_(vpage_no<arch::i386>, page_count<arch::i386>,
              bool) noexcept;

 public:
  static constexpr std::array<size_t, 2> N_PAGES = {{ 1, 1 << 9 }};

  const void* get_pmap_ptr() const noexcept { return &pdpe_; }  // XXX return phys addr

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

  /* Alignment constraint (pageno) for large pages at PDP level. */
  static constexpr uint64_t lp_pdp_pgno_align = 1ULL << 9;

  /* Number of bits describing indices for each level. */
  static constexpr unsigned int offset_bits = 12;
  static constexpr unsigned int pte_offset_bits = 9;
  static constexpr unsigned int pdp_offset_bits = 9;
  static constexpr unsigned int pdpe_offset_bits = 2;

  /* Shifts for converting addresses to page-table indices. */
  static constexpr unsigned int pte_addr_offset = offset_bits;
  static constexpr unsigned int pdp_addr_offset = pte_addr_offset +
                                                  pte_offset_bits;
  static constexpr unsigned int pdpe_addr_offset = pdp_addr_offset +
                                                   pdp_offset_bits;

  /* Masks for converting addresses to page-table indices. */
  static constexpr uint32_t pdpe_mask =
      uint32_t(0) - (uint32_t(1) << pdpe_addr_offset);
  static constexpr uint32_t pdp_mask =
      (uint32_t(1) << pdpe_addr_offset) - (uint32_t(1) << pdp_addr_offset);
  static constexpr uint32_t pte_mask =
      (uint32_t(1) << pdp_addr_offset) - (uint32_t(1) << pte_addr_offset);

  /* Number of records in each level. */
  static constexpr unsigned int N_PDPE = 1U << pdpe_offset_bits;
  static constexpr unsigned int N_PDP = 1U << pdp_offset_bits;
  static constexpr unsigned int N_PTE = 1U << pte_offset_bits;

  /* Worst case, the pmap requires this many pages to hold its information. */
  static constexpr page_count<arch::i386> worst_case_npages =
      page_count<arch::i386>(N_PDPE * (1 + N_PDP));

  /*
   * Memory layout for kva self-mapping:
   *
   * address           | data                          | #pages
   * ------------------+-------------------------------+-----------------
   * 0xffffffff        |                               |
   *                   | pdp[0..4)                     | N_PDPE
   * kva_map_self_pdp  |                               |
   *                   | pdp[0..4) pte[0..1024)        | N_PDPE * N_PDP
   * kva_map_self_pte  |                               |
   *   = kva_map_self  |                               |
   */
  static constexpr vpage_no<arch::i386> kva_map_self =
      vpage_no<arch::i386>(0xffffffff >> page_shift(arch::i386)) -
      worst_case_npages;
  static constexpr vpage_no<arch::i386> kva_map_self_pdp =
      kva_map_self + page_count<arch::i386>(1) * N_PDPE * N_PDP;
  static constexpr vpage_no<arch::i386> kva_map_self_pte =
      kva_map_self;

  /*
   * Use AVL bit 0 to mark pte's/pdp's as critical.
   * Critical pte's/pdp's won't be garbage collected during unmap.
   */
  static const unsigned int AVL_CRITICAL = 0;

  /* Declare our record types. */
  using pdpe_record = x86_shared::pdpe_record<arch::i386>;
  using pdp_record = x86_shared::pdp_record;
  using pte_record = x86_shared::pte_record;

  /*
   * Declare PDPE level (which has a strict alignment requirement,
   * due to the lower bits in the pointer being flags.
   */
  struct alignas(1 << 5) pdpe
  : public std::array<pdpe_record, N_PDPE>
  {
    pdpe() noexcept;
    pdpe(const pdpe&) noexcept = default;
    pdpe& operator=(const pdpe&) noexcept = default;
  };

  /* Declare types for PDP and PTE levels. */
  using pdp = std::array<pdp_record, N_PDP>;
  using pte = std::array<pte_record, N_PTE>;

  /*
   * Page mapping wrapper, which uses the kva_{pdp,pte}_entry addresses if the
   * page_map is a kernel pmap.
   */
  auto map_pdp(page_no<arch::i386>, vaddr<arch::i386>) const ->
      pmap_mapped_ptr<pdp, arch::i386>;
  auto map_pte(page_no<arch::i386>, vaddr<arch::i386>) const ->
      pmap_mapped_ptr<pte, arch::i386>;
  auto map_pdp(page_no<arch::i386>, unsigned int) const ->
      pmap_mapped_ptr<pdp, arch::i386>;
  auto map_pte(page_no<arch::i386>, unsigned int, unsigned int) const ->
      pmap_mapped_ptr<pte, arch::i386>;

  void maybe_gc(
      pdpe_record&,
      std::tuple<page_ptr<arch::i386>&,
                 pmap_mapped_ptr<pdp, arch::i386>&,
                 pdp_record&>,
      std::tuple<page_ptr<arch::i386>&,
                 pmap_mapped_ptr<pte, arch::i386>&,
                 pte_record&>)
      noexcept;
  void maybe_gc(
      pdpe_record&,
      std::tuple<page_ptr<arch::i386>&,
                 pmap_mapped_ptr<pdp, arch::i386>&,
                 pdp_record&>)
      noexcept;
  void maybe_gc(pdpe_record&) noexcept {}

  void break_large_page_(pdp_record&, vaddr<arch::i386>);

  void deregister_from_pg_(
      page_no<arch::i386>,
      vpage_no<arch::i386>,
      page_count<arch::i386> = page_count<arch::i386>(1)) noexcept;

  /* Variables start here. */
  pdpe pdpe_;
  pmap_support<arch::i386>& support_;
  bool kva_map_self_enabled_ = false;

  /*
   * Verify that everything behaves as planned.
   */
  static_assert(sizeof(pdpe) == 4 * 8,
                "PDPE table has wrong size.");
  static_assert(sizeof(pdp) == 4 * 1024,
                "PDP table has wrong size.");
  static_assert(sizeof(pte) == 4 * 1024,
                "PTE table has wrong size.");

  /*
   * Rigorous validation of masks.
   */
  static_assert((pdpe_mask | pdp_mask | pte_mask | page_mask(arch::i386)) ==
                0xffffffffU,
                "Masks must connect and describe the entire address space.");
  static_assert((pdpe_mask & pdp_mask) == 0 &&
                (pdp_mask & pte_mask) == 0 &&
                (pte_mask & page_mask(arch::i386)) == 0,
                "Masks may not overlap.");
  static_assert(pdpe_mask > pdp_mask && pdp_mask > pte_mask &&
                pte_mask > page_mask(arch::i386),
                "Masks must be ordered: PDPE > PDP > PTE > page_mask.");
  static_assert(offset_bits == page_shift(arch::i386),
                "Page shift or offset_bits are wrong.");

  /*
   * Validate self-mapping calculations.
   */
  static_assert(kva_map_self == kva_map_self_pte,
                "Self-mapping calculation went wrong.");
};

template<> class pmap_map<pmap<arch::i386>> {
 private:
  static constexpr unsigned int N_PDPE = pmap<arch::i386>::N_PDPE;
  static constexpr unsigned int N_PDP = pmap<arch::i386>::N_PDP;
  static constexpr unsigned int N_PTE = pmap<arch::i386>::N_PTE;

  static constexpr unsigned int pdpe_addr_offset =
      pmap<arch::i386>::pdpe_addr_offset;
  static constexpr unsigned int pdp_addr_offset =
      pmap<arch::i386>::pdp_addr_offset;
  static constexpr unsigned int pte_addr_offset =
      pmap<arch::i386>::pte_addr_offset;
  static constexpr uint32_t pdpe_mask = pmap<arch::i386>::pdpe_mask;
  static constexpr uint32_t pdp_mask = pmap<arch::i386>::pdp_mask;
  static constexpr uint32_t pte_mask = pmap<arch::i386>::pte_mask;

  using pdpe_record = pmap<arch::i386>::pdpe_record;
  using pdp_record = pmap<arch::i386>::pdp_record;
  using pte_record = pmap<arch::i386>::pte_record;
  using pdpe = pmap<arch::i386>::pdpe;
  using pdp = pmap<arch::i386>::pdp;
  using pte = pmap<arch::i386>::pte;

 public:
  pmap_map() noexcept = default;
  pmap_map(const pmap_map&) = delete;
  pmap_map& operator=(const pmap_map&) = delete;
  pmap_map(pmap<arch::i386>&, vpage_no<arch::i386>, vpage_no<arch::i386>);

  void push_back(page_no<arch::i386>, permission,
                 page_count<arch::i386> = page_count<arch::i386>(1));
  void commit();
  page_count<arch::i386> size() const noexcept;
  page_count<arch::i386> max_size() const noexcept;

 private:
  void load_pdp_ptr_(size_t) const;
  void load_pte_ptr_(size_t, size_t) const;
  bool pte_is_lp_convertible(size_t, size_t) const;

  pmap<arch::i386>* pmap_ = nullptr;
  vpage_no<arch::i386> va_start_{}, va_end_{}, va_{};
  bool commit_ = false;

  /* The pointers for PDP and PTE are cached between calls. */
  mutable pmap_mapped_ptr<pmap<arch::i386>::pdp, arch::i386> pdp_ptr_;
  mutable pmap_mapped_ptr<pmap<arch::i386>::pte, arch::i386> pte_ptr_;
};


}} /* namespace ilias::pmap */

#include "pmap_i386-inl.h"

#endif /* _ILIAS_PMAP_PMAP_I386_H_ */
