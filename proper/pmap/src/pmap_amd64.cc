#include <ilias/pmap/pmap_amd64.h>
#include <ilias/pmap/page_alloc.h>
#include <ilias/pmap/page_alloc_support.h>
#include <ilias/pmap/pmap_page.h>
#include <algorithm>
#include <tuple>
#include <utility>

namespace ilias {
namespace pmap {


constexpr std::array<size_t, 3> pmap<arch::amd64>::N_PAGES;

constexpr uint64_t pmap<arch::amd64>::lp_pdpe_pgno_align;
constexpr uint64_t pmap<arch::amd64>::lp_pdp_pgno_align;

constexpr unsigned int pmap<arch::amd64>::offset_bits;
constexpr unsigned int pmap<arch::amd64>::pte_offset_bits;
constexpr unsigned int pmap<arch::amd64>::pdp_offset_bits;
constexpr unsigned int pmap<arch::amd64>::pdpe_offset_bits;
constexpr unsigned int pmap<arch::amd64>::pml4_offset_bits;

constexpr unsigned int pmap<arch::amd64>::pte_addr_offset;
constexpr unsigned int pmap<arch::amd64>::pdp_addr_offset;
constexpr unsigned int pmap<arch::amd64>::pdpe_addr_offset;
constexpr unsigned int pmap<arch::amd64>::pml4_addr_offset;
constexpr unsigned int pmap<arch::amd64>::pml4_addr_off_end;

constexpr uint64_t pmap<arch::amd64>::sign_mask;
constexpr uint64_t pmap<arch::amd64>::sign_choice;

constexpr uint64_t pmap<arch::amd64>::pml4_mask;
constexpr uint64_t pmap<arch::amd64>::pdpe_mask;
constexpr uint64_t pmap<arch::amd64>::pdp_mask;
constexpr uint64_t pmap<arch::amd64>::pte_mask;

constexpr unsigned int pmap<arch::amd64>::N_PML4;
constexpr unsigned int pmap<arch::amd64>::N_PDPE;
constexpr unsigned int pmap<arch::amd64>::N_PDP;
constexpr unsigned int pmap<arch::amd64>::N_PTE;

constexpr page_count<arch::amd64> pmap<arch::amd64>::worst_case_npages;
constexpr vpage_no<arch::amd64> pmap<arch::amd64>::kva_map_self;
constexpr vpage_no<arch::amd64> pmap<arch::amd64>::kva_map_self_pml4;
constexpr vpage_no<arch::amd64> pmap<arch::amd64>::kva_map_self_pdpe;
constexpr vpage_no<arch::amd64> pmap<arch::amd64>::kva_map_self_pdp;
constexpr vpage_no<arch::amd64> pmap<arch::amd64>::kva_map_self_pte;


pmap<arch::amd64>::~pmap() noexcept {
  clear();
}

auto pmap<arch::amd64>::clear() noexcept -> void {
  if (pml4_ == page_no<arch::amd64>(0)) return;

  /* Mark PML4 page for desctruction. */
  auto pml4_ptr = page_ptr<arch::amd64>(pml4_);
  pml4_ptr.set_allocated(support_);
  auto va = vpage_no<arch::amd64>(0);

  /* Traverse PML4 page. */
  auto mapped_pml4 = pmap_map_page<pml4>(pml4_ptr.get(), support_);
  for (pml4_record pml4_value : *mapped_pml4) {
    constexpr auto pml4_skip = page_count<arch::amd64>(N_PTE * N_PDP * N_PDPE);
    if (!pml4_value.p()) {
      va += pml4_skip;
      continue;  // Skip non-present entries.
    }
    /* pml4_value has no page-select bit. */

    /* Mark this page for destruction. */
    auto pdpe_ptr = page_ptr<arch::amd64>(pml4_value.address());
    pdpe_ptr.set_allocated(support_);

    /* Traverse PDPE page. */
    auto mapped_pdpe = pmap_map_page<pdpe>(pdpe_ptr.get(), support_);
    for (pdpe_record pdpe_value : *mapped_pdpe) {
      constexpr auto pdpe_skip = page_count<arch::amd64>(N_PTE * N_PDP);
      if (!pdpe_value.p()) {
        va += pdpe_skip;
        continue;  // Skip non-present entries.
      }
      if (pdpe_value.ps()) {  // Large page.
        const bool accessed = pdpe_value.flags().a();
        const bool dirty = pdpe_value.flags().d();
        deregister_from_pg_(pdpe_value.address(), va, accessed, dirty,
                            pdpe_skip);
        va += pdpe_skip;
        continue;
      }

      /* Mark this page for destruction. */
      auto pdp_ptr = page_ptr<arch::amd64>(pdpe_value.address());
      pdp_ptr.set_allocated(support_);

      /* Traverse PDP page. */
      auto mapped_pdp = pmap_map_page<pdp>(pdp_ptr.get(), support_);
      for (pdp_record pdp_value : *mapped_pdp) {
        constexpr auto pdp_skip = page_count<arch::amd64>(N_PTE);
        if (!pdp_value.p()) {
          va += pdp_skip;
          continue;  // Skip non-present entries.
        }
        if (pdp_value.ps()) {  // Large page.
          const bool accessed = pdp_value.flags().a();
          const bool dirty = pdp_value.flags().d();
          deregister_from_pg_(pdp_value.address(), va, accessed, dirty,
                              pdp_skip);
          va += pdpe_skip;
          continue;
        }

        /* Mark this page for destruction. */
        auto pte_ptr = page_ptr<arch::amd64>(pdp_value.address());
        pte_ptr.set_allocated(support_);

        /* Traverse PTE page. */
        auto mapped_pte = pmap_map_page<pte>(pte_ptr.get(), support_);
        for (pte_record pte_value : *mapped_pte) {
          /* Release the page. */
          if (pte_value.p()) {
            const bool accessed = pte_value.flags().a();
            const bool dirty = pte_value.flags().d();
            deregister_from_pg_(pte_value.address(), va, accessed, dirty);
          }
          va += page_count<arch::amd64>(1);
        }
      }
    }
  }
}

auto pmap<arch::amd64>::virt_to_phys(vaddr<arch::amd64> p) const ->
    phys_addr<arch::amd64> {
  phys_addr<arch::amd64> paddr;
  uintptr_t off;

  std::tie(paddr, std::ignore, off) = virt_to_page(p);
  return phys_addr<arch::amd64>(paddr.get() + off);
}

auto pmap<arch::amd64>::virt_to_page(vaddr<arch::amd64> va) const ->
    std::tuple<page_no<arch::amd64>, size_t, uintptr_t> {
  if (_predict_false(!valid_sign_extend(va))) throw efault(va.get());
  auto p = va.get();
  p &= ~sign_mask;

  /* Resolve pml4 offset. */
  const uintptr_t pml4_off = (p & pml4_mask) >> pml4_addr_offset;
  p &= ~pml4_mask;

  /* Resolve pml4. */
  const pml4_record pml4_value =
      (*map_pml4(pml4_, va))[pml4_off];
  if (_predict_false(!pml4_value.p())) throw efault(va.get());
  const page_no<arch::amd64> pdpe_addr = pml4_value.address();

  /* Resolve pdpe offset. */
  const uintptr_t pdpe_off = (p & pdpe_mask) >> pdpe_addr_offset;
  p &= ~pdpe_mask;

  /* Resolve pdpe. */
  const pdpe_record pdpe_value =
      (*map_pdpe(pdpe_addr, va))[pdpe_off];
  if (_predict_false(!pdpe_value.p())) throw efault(va.get());
  const page_no<arch::amd64> pdp_addr = pdpe_value.address();

  if (pdpe_value.ps())
    return std::make_tuple(pdp_addr, (1U << pdp_offset_bits), p);

  /* Resolve pdp offset. */
  const uintptr_t pdp_off = (p & pdp_mask) >> pdp_addr_offset;
  p &= ~pdp_mask;

  /* Resolve pdp. */
  const pdp_record pdp_value =
      (*map_pdp(pdp_addr, va))[pdp_off];
  if (_predict_false(!pdp_value.p())) throw efault(va.get());
  const page_no<arch::amd64> pte_addr = pdp_value.address();

  if (pdp_value.ps())
    return std::make_tuple(pte_addr, (1U << pte_offset_bits), p);

  /* Resolve pte offset. */
  const uintptr_t pte_off = (p & pte_mask) >> pte_addr_offset;
  p &= ~pte_mask;

  /* Resolve pte. */
  const pte_record pte_value =
      (*map_pte(pte_addr, va))[pte_off];
  if (_predict_false(!pte_value.p())) throw efault(va.get());
  const page_no<arch::amd64> pg = pte_value.address();

  return std::make_tuple(pg, 1, p);
}

auto pmap<arch::amd64>::reduce_permission(vpage_no<arch::amd64> va,
                                          permission perm) ->
    reduce_permission_result {
  vpage_no<arch::amd64> lo, hi;
  std::tie(lo, hi) = managed_range();
  if (va < lo || va >= hi)
    throw std::out_of_range("va outside of managed range");
  if (_predict_false(!valid_sign_extend(va))) throw efault(va.get());
  return reduce_permission_(va, perm);
}

auto pmap<arch::amd64>::map(vpage_no<arch::amd64> va,
                            page_no<arch::amd64> pa,
                            permission perm) -> void {
  vpage_no<arch::amd64> lo, hi;
  std::tie(lo, hi) = managed_range();
  if (va < lo || va >= hi)
    throw std::out_of_range("va outside of managed range");
  if (_predict_false(!valid_sign_extend(va))) throw efault(va.get());
  map_(va, pa, perm);
}

auto pmap<arch::amd64>::unmap(vpage_no<arch::amd64> va,
                              page_count<arch::amd64> npg) -> void {
  const auto va_end = va + npg;
  if (va_end < va)
    throw std::length_error("too many pages: va wraps around");

  vpage_no<arch::amd64> lo, hi;
  std::tie(lo, hi) = managed_range();
  if (va < lo || va_end > hi)
    throw std::out_of_range("va outside of managed range");
  unmap_(va, npg, true);
}

auto pmap<arch::amd64>::flush_accessed_dirty(vpage_no<arch::amd64> va)
    noexcept -> void {
  vpage_no<arch::amd64> lo, hi;
  std::tie(lo, hi) = managed_range();
  if (va < lo || va >= hi)
    throw std::out_of_range("va outside of managed range");
  return flush_accessed_dirty_(va);
}

auto pmap<arch::amd64>::reduce_permission_(vpage_no<arch::amd64> va,
                                           permission perm) noexcept ->
    reduce_permission_result {
  using namespace x86_shared;

  const auto p = vaddr<arch::amd64>(va).get();
  auto pml4_ptr = page_ptr<arch::amd64>(pml4_);
  auto mapped_pml4 = map_pml4(pml4_ptr.get(), va);

  /* Resolve pml4 offset. */
  const uintptr_t pml4_off = (p & pml4_mask) >> pml4_addr_offset;

  /* Resolve pdpe. */
  pml4_record& pml4_value = (*mapped_pml4)[pml4_off];
  if (!pml4_value.p())
    return reduce_permission_result::UNMAPPED;
  auto pdpe_ptr = page_ptr<arch::amd64>(pml4_value.address());
  auto mapped_pdpe = map_pdpe(pdpe_ptr.get(), va);

  /* Resolve pdpe offset. */
  const uintptr_t pdpe_off = (p & pdpe_mask) >> pdpe_addr_offset;

  /* Resolve pdp. */
  pdpe_record& pdpe_value = (*mapped_pdpe)[pdpe_off];
  if (!pdpe_value.p())
    return reduce_permission_result::UNMAPPED;
  if (pdpe_value.ps()) {  /* Resolve new permission. */
    permission reduced = perm & pdpe_value.get_permission();

    /* Assign page entry. */
    const auto new_pdpe_value = pdpe_value.combine(reduced);
    assert(new_pdpe_value.valid());
    pdpe_value = new_pdpe_value;

    /*
     * Note that there is no need to propagate the permission up the tree,
     * as the earlier entries should already have all the eventual permissions.
     */
    if (new_pdpe_value.p()) return reduce_permission_result::OK;

    maybe_gc(tie(pml4_ptr, mapped_pml4, pml4_value),
             tie(pdpe_ptr, mapped_pdpe, pdpe_value));
    return reduce_permission_result::UNMAPPED;
  }
  auto pdp_ptr = page_ptr<arch::amd64>(pdpe_value.address());
  auto mapped_pdp = map_pdp(pdp_ptr.get(), va);

  /* Resolve pdp offset. */
  const uintptr_t pdp_off = (p & pdp_mask) >> pdp_addr_offset;

  /* Resolve pte. */
  pdp_record& pdp_value = (*mapped_pdp)[pdp_off];
  if (!pdp_value.p())
    return reduce_permission_result::UNMAPPED;
  if (pdp_value.ps()) {  /* Resolve new permission. */
    permission reduced = perm & pdp_value.get_permission();

    /* Assign page entry. */
    const auto new_pdp_value = pdp_value.combine(reduced);
    assert(new_pdp_value.valid());
    pdp_value = new_pdp_value;

    /*
     * Note that there is no need to propagate the permission up the tree,
     * as the earlier entries should already have all the eventual permissions.
     */
    if (new_pdp_value.p()) return reduce_permission_result::OK;

    maybe_gc(tie(pml4_ptr, mapped_pml4, pml4_value),
             tie(pdpe_ptr, mapped_pdpe, pdpe_value),
             tie(pdp_ptr, mapped_pdp, pdp_value));
    return reduce_permission_result::UNMAPPED;
  }
  auto pte_ptr = page_ptr<arch::amd64>(pdp_value.address());
  auto mapped_pte = map_pte(pte_ptr.get(), va);

  /* Resolve pte offset. */
  const uintptr_t pte_off = (p & pte_mask) >> pte_addr_offset;

  /* Resolve page entry. */
  pte_record& pte_value = (*mapped_pte)[pte_off];
  if (!pte_value.p())
    return reduce_permission_result::UNMAPPED;

  /* Resolve new permissions. */
  {
    permission reduced = perm & pte_value.get_permission();

    /* Assign page entry. */
    const auto new_pte_value = pte_value.combine(reduced);
    assert(new_pte_value.valid());
    pte_value = new_pte_value;

    /*
     * Note that there is no need to propagate the permission up the tree,
     * as the earlier entries should already have all the eventual permissions.
     */
    if (new_pte_value.p()) return reduce_permission_result::OK;

    /* GC pte if new value if unmapped. */
    maybe_gc(tie(pml4_ptr, mapped_pml4, pml4_value),
             tie(pdpe_ptr, mapped_pdpe, pdpe_value),
             tie(pdp_ptr, mapped_pdp, pdp_value),
             tie(pte_ptr, mapped_pte, pte_value));
    return reduce_permission_result::UNMAPPED;
  }
}

auto pmap<arch::amd64>::map_(vpage_no<arch::amd64> va, page_no<arch::amd64> pg,
                             permission perm) -> void {
  using namespace x86_shared;

  const auto p = vaddr<arch::amd64>(va).get();
  auto pml4_ptr = (pml4_ != page_no<arch::amd64>(0) ?
                   page_ptr<arch::amd64>(pml4_) :
                   page_ptr<arch::amd64>::allocate(support_));
  auto mapped_pml4 = map_pml4(pml4_ptr.get(), va);
  /* Clear pml4 if newly allocated. */
  if (pml4_ptr.is_allocated())
    std::fill(mapped_pml4->begin(), mapped_pml4->end(), pml4_record{ 0 });

  /* Resolve pml4 offset. */
  const uintptr_t pml4_off = (p & pml4_mask) >> pml4_addr_offset;

  /* Resolve pdpe. */
  pml4_record& pml4_value = (*mapped_pml4)[pml4_off];
  auto pdpe_ptr = (pml4_value.p() ?
                   page_ptr<arch::amd64>(pml4_value.address()) :
                   page_ptr<arch::amd64>::allocate(support_));
  auto mapped_pdpe = map_pdpe(pdpe_ptr.get(), va);
  /* Clear pdpe if newly allocated. */
  if (pdpe_ptr.is_allocated())
    std::fill(mapped_pdpe->begin(), mapped_pdpe->end(), pdpe_record{ 0 });

  /* Resolve pdpe offset. */
  const uintptr_t pdpe_off = (p & pdpe_mask) >> pdpe_addr_offset;

  /* Resolve pdp. */
  pdpe_record& pdpe_value = (*mapped_pdpe)[pdpe_off];
  auto pdp_ptr = (pdpe_value.p() ?
                  page_ptr<arch::amd64>(pdpe_value.address()) :
                  page_ptr<arch::amd64>::allocate(support_));
  auto mapped_pdp = map_pdp(pdp_ptr.get(), va);
  /* Clear pdp if newly allocated. */
  if (pdp_ptr.is_allocated())
    std::fill(mapped_pdp->begin(), mapped_pdp->end(), pdp_record{ 0 });

  /* Resolve pdp offset. */
  const uintptr_t pdp_off = (p & pdp_mask) >> pdp_addr_offset;

  /* Resolve pte. */
  pdp_record& pdp_value = (*mapped_pdp)[pdp_off];
  auto pte_ptr = (pdp_value.p() ?
                  page_ptr<arch::amd64>(pdp_value.address()) :
                  page_ptr<arch::amd64>::allocate(support_));
  auto mapped_pte = map_pte(pte_ptr.get(), va);
  /* Clear pte if newly allocated. */
  if (pte_ptr.is_allocated())
    std::fill(mapped_pte->begin(), mapped_pte->end(), pte_record{ 0 });

  /* Resolve pte offset. */
  const uintptr_t pte_off = (p & pte_mask) >> pte_addr_offset;

  /* Resolve pg. */
  pte_record& pte_value = (*mapped_pte)[pte_off];

  /* Assign page entry. */
  {
    const auto new_pte_value = pte_record::create(
        pg, (userspace() ? PT_US : flags { 0 }) |
            (pte_value.flags() & PT_AVL)).combine(perm);
    assert(new_pte_value.valid());
    pte_value = new_pte_value;
  }

  /* Assign pte to pdp, iff newly allocated. */
  if (pte_ptr.is_allocated()) {
    const auto new_pdp_value = pdp_record::create(
        pte_ptr.get(), (userspace() ? PT_US : flags{ 0 }) |
                       (pdp_value.flags() & PT_AVL)).combine(perm);
    assert(new_pdp_value.valid());
    pdp_value = new_pdp_value;
    pte_ptr.release();
  } else {
    pdp_record new_pdp_value = pdp_value.combine(perm);
    assert(new_pdp_value.valid());
    pdp_value = new_pdp_value;
  }

  /* Assign pdp to pdpe, iff newly allocated. */
  if (pdp_ptr.is_allocated()) {
    const auto new_pdpe_value = pdpe_record::create(
        pdp_ptr.get(), (userspace() ? PT_US : flags{ 0 }) |
                       (pdpe_value.flags() & PT_AVL)).combine(perm);
    assert(new_pdpe_value.valid());
    pdpe_value = new_pdpe_value;
    pdp_ptr.release();
  } else {
    pdpe_record new_pdpe_value = pdpe_value.combine(perm);
    assert(new_pdpe_value.valid());
    pdpe_value = new_pdpe_value;
  }

  /* Assign pdpe to pml4, iff newly allocated. */
  if (pdpe_ptr.is_allocated()) {
    const auto new_pml4_value = pml4_record::create(
        pdpe_ptr.get(), (userspace() ? PT_US : flags{ 0 }) |
                        (pml4_value.flags() & PT_AVL)).combine(perm);
    assert(new_pml4_value.valid());
    pml4_value = new_pml4_value;
    pdpe_ptr.release();
  } else {
    pml4_record new_pml4_value = pml4_value.combine(perm);
    assert(new_pml4_value.valid());
    pml4_value = new_pml4_value;
  }

  /* Assign pml4 to this, iff newly allocated. */
  if (pml4_ptr.is_allocated())
    pml4_ = pdpe_ptr.release();
}

auto pmap<arch::amd64>::unmap_(vpage_no<arch::amd64> va,
                               page_count<arch::amd64> c,
                               bool do_deregister) noexcept -> void {
  using namespace x86_shared;
  using std::none_of;
  using std::min;
  using std::next;

  if (_predict_false(pml4_ == page_no<arch::amd64>(0))) return;
  const auto p = vaddr<arch::amd64>(va).get();

  /* Resolve pml4 offset. */
  auto pml4_off = (p & pml4_mask) >> pml4_addr_offset;
  /* Resolve pdpe offset. */
  auto pdpe_off = (p & pdpe_mask) >> pdpe_addr_offset;
  /* Resolve pdp offset. */
  auto pdp_off = (p & pdp_mask) >> pdp_addr_offset;
  /* Resolve pte offset. */
  auto pte_off = (p & pte_mask) >> pte_addr_offset;

  /* Iterate in pml4. */
  auto pml4_ptr = page_ptr<arch::amd64>(pml4_);
  auto mapped_pml4 = map_pml4(pml4_ptr.get(), va);
  for (auto pml4_iter = next(mapped_pml4->begin(), pml4_off);
       pml4_iter != mapped_pml4->end() && c > page_count<arch::amd64>(0);
       pdpe_off = 0, ++pml4_iter) {
    const auto pml4_skip =  // Number of entries covered.
        min(c, page_count<arch::amd64>(N_PTE * N_PDP * (N_PDPE - pdpe_off)));
    if (!pml4_iter->p()) {  // PML4 not present.
      c -= pml4_skip;
      va += pml4_skip;
      continue;
    }

    /* Iterate in pdpe. */
    auto pdpe_ptr = page_ptr<arch::amd64>(pml4_iter->address());
    auto mapped_pdpe = map_pdpe(pdpe_ptr.get(), va);
    for (auto pdpe_iter = next(mapped_pdpe->begin(), pdpe_off);
         pdpe_iter != mapped_pdpe->end() && c > page_count<arch::amd64>(0);
         pdp_off = 0, ++pdpe_iter) {
      const auto pdpe_skip =  // Number of entries covered.
          min(c, page_count<arch::amd64>(N_PTE * (N_PDP - pdp_off)));
      if (!pdpe_iter->p()) {  // PDPE not present.
        c -= pdpe_skip;
        va += pdpe_skip;
        continue;
      }
      if (pdpe_iter->ps()) {  // Large page.
        /*
         * Break the page up into smaller bits, iff the page is unmapped
         * partially.
         * On failure: just unmap the page and let page-fault deal with it.
         */
        bool broken_up = false;
        try {
          if (pdpe_skip != page_count<arch::amd64>(N_PTE * N_PDP)) {
            break_large_page_(*pdpe_iter, va);
            broken_up = true;
          }
        } catch (...) {
          /* SKIP: broken_up = false, handled below. */
        }

        if (!broken_up) {
          if (do_deregister) {
            const bool accessed = pdpe_iter->flags().a();
            const bool dirty = pdpe_iter->flags().d();
            deregister_from_pg_(pdpe_iter->address(), va, accessed, dirty,
                                page_count<arch::amd64>(N_PTE * N_PDP));
          }
          va += page_count<arch::amd64>(N_PTE * N_PDP);

          *pdpe_iter = pdpe_record::create(nullptr, pdpe_iter->flags());
          maybe_gc(tie(pml4_ptr, mapped_pml4, *pml4_iter),
                   tie(pdpe_ptr, mapped_pdpe, *pdpe_iter));
          continue;
        }
      }

      /* Iterate in pdp. */
      auto pdp_ptr = page_ptr<arch::amd64>(pdpe_iter->address());
      auto mapped_pdp = map_pdp(pdp_ptr.get(), va);
      for (auto pdp_iter = next(mapped_pdp->begin(), pdp_off);
           pdp_iter != mapped_pdp->end() && c > page_count<arch::amd64>(0);
           pte_off = 0, ++pdp_iter) {
        const auto pdp_skip =  // Number of entries covered.
            min(c, page_count<arch::amd64>(N_PTE - pte_off));
        if (!pdp_iter->p()) {  // PDP not present.
          c -= pdp_skip;
          va += pdp_skip;
          continue;
        }
        if (pdp_iter->ps()) {  // Large page.
          /*
           * Break the page up into smaller bits, iff the page is unmapped
           * partially.
           * On failure: just unmap the page and let page-fault deal with it.
           */
          bool broken_up = false;
          try {
            if (pdp_skip != page_count<arch::amd64>(N_PTE)) {
              break_large_page_(*pdp_iter, va);
              broken_up = true;
            }
          } catch (...) {
            /* SKIP: broken_up = false, handled below. */
          }

          if (!broken_up) {
            if (do_deregister) {
              const bool accessed = pdp_iter->flags().a();
              const bool dirty = pdp_iter->flags().d();
              deregister_from_pg_(pdp_iter->address(), va, accessed, dirty,
                                  page_count<arch::amd64>(N_PTE));
            }
            va += page_count<arch::amd64>(N_PTE);

            *pdp_iter = pdp_record::create(nullptr, pdp_iter->flags());
            maybe_gc(tie(pml4_ptr, mapped_pml4, *pml4_iter),
                     tie(pdpe_ptr, mapped_pdpe, *pdpe_iter),
                     tie(pdp_ptr, mapped_pdp, *pdp_iter));
            continue;
          }
        }

        /* Iterate in pte. */
        auto pte_ptr = page_ptr<arch::amd64>(pdp_iter->address());
        auto mapped_pte = map_pte(pte_ptr.get(), va);
        for (auto pte_iter = next(mapped_pte->begin(), pte_off);
             pte_iter != mapped_pte->end() && c > page_count<arch::amd64>(0);
             ++va, --c, ++pte_iter) {
          if (!pte_iter->p()) {  // PTE not present.
            /* Increment/decrement logic is present in loop. */
            continue;
          }

          if (pte_iter->p() && do_deregister) {
            const bool accessed = pte_iter->flags().a();
            const bool dirty = pte_iter->flags().d();
            deregister_from_pg_(pte_iter->address(), va, accessed, dirty);
          }

          *pte_iter = pte_record::create(nullptr, pte_iter->flags());
          maybe_gc(tie(pml4_ptr, mapped_pml4, *pml4_iter),
                   tie(pdpe_ptr, mapped_pdpe, *pdpe_iter),
                   tie(pdp_ptr, mapped_pdp, *pdp_iter),
                   tie(pte_ptr, mapped_pte, *pte_iter));
        }  // Iterate in pte.
      }  // Iterate in pdp.
    }  // Iterate in pdpe.
  }  // Iterate in pml4.
}

auto pmap<arch::amd64>::flush_accessed_dirty_(vpage_no<arch::amd64> va)
    noexcept -> void {
  using namespace x86_shared;

  const auto p = vaddr<arch::amd64>(va).get();
  auto pml4_ptr = page_ptr<arch::amd64>(pml4_);
  auto mapped_pml4 = map_pml4(pml4_ptr.get(), va);

  /* Resolve pml4 offset. */
  const uintptr_t pml4_off = (p & pml4_mask) >> pml4_addr_offset;

  /* Resolve pdpe. */
  pml4_record& pml4_value = (*mapped_pml4)[pml4_off];
  if (!pml4_value.p()) return;
  auto pdpe_ptr = page_ptr<arch::amd64>(pml4_value.address());
  auto mapped_pdpe = map_pdpe(pdpe_ptr.get(), va);

  /* Resolve pdpe offset. */
  const uintptr_t pdpe_off = (p & pdpe_mask) >> pdpe_addr_offset;

  /* Resolve pdp. */
  pdpe_record& pdpe_value = (*mapped_pdpe)[pdpe_off];
  if (!pdpe_value.p()) return;
  if (pdpe_value.ps()) {  /* Flush flags. */
    const auto fl = pdpe_value.clear_ad_flags();
    add_flags_to_pg_(pdpe_value.address(), fl.a(), fl.d(),
                     page_count<arch::amd64>(N_PDP * N_PTE));
    return;
  }
  auto pdp_ptr = page_ptr<arch::amd64>(pdpe_value.address());
  auto mapped_pdp = map_pdp(pdp_ptr.get(), va);

  /* Resolve pdp offset. */
  const uintptr_t pdp_off = (p & pdp_mask) >> pdp_addr_offset;

  /* Resolve pte. */
  pdp_record& pdp_value = (*mapped_pdp)[pdp_off];
  if (!pdp_value.p()) return;
  if (pdp_value.ps()) {  /* Flush flags. */
    const auto fl = pdp_value.clear_ad_flags();
    add_flags_to_pg_(pdpe_value.address(), fl.a(), fl.d(),
                     page_count<arch::amd64>(N_PTE));
    return;
  }
  auto pte_ptr = page_ptr<arch::amd64>(pdp_value.address());
  auto mapped_pte = map_pte(pte_ptr.get(), va);

  /* Resolve pte offset. */
  const uintptr_t pte_off = (p & pte_mask) >> pte_addr_offset;

  /* Resolve page entry. */
  pte_record& pte_value = (*mapped_pte)[pte_off];
  if (!pte_value.p()) return;

  /* Flush flags. */
  {
    const auto fl = pte_value.clear_ad_flags();
    add_flags_to_pg_(pdpe_value.address(), fl.a(), fl.d());
    return;
  }
}

auto pmap<arch::amd64>::map_pml4(page_no<arch::amd64> pg,
                                 vaddr<arch::amd64> va) const ->
    pmap_mapped_ptr<pml4, arch::amd64> {
  if (kva_map_self_enabled_)
    return pmap_map_page<pml4>(kva_pml4_entry(va));
  return pmap_map_page<pml4>(pg, support_);
}

auto pmap<arch::amd64>::map_pdpe(page_no<arch::amd64> pg,
                                 vaddr<arch::amd64> va) const ->
    pmap_mapped_ptr<pdpe, arch::amd64> {
  if (kva_map_self_enabled_)
    return pmap_map_page<pdpe>(kva_pdpe_entry(va));
  return pmap_map_page<pdpe>(pg, support_);
}

auto pmap<arch::amd64>::map_pdp(page_no<arch::amd64> pg,
                                vaddr<arch::amd64> va) const ->
    pmap_mapped_ptr<pdp, arch::amd64> {
  if (kva_map_self_enabled_)
    return pmap_map_page<pdp>(kva_pdp_entry(va));
  return pmap_map_page<pdp>(pg, support_);
}

auto pmap<arch::amd64>::map_pte(page_no<arch::amd64> pg,
                                vaddr<arch::amd64> va) const ->
    pmap_mapped_ptr<pte, arch::amd64> {
  if (kva_map_self_enabled_)
    return pmap_map_page<pte>(kva_pte_entry(va));
  return pmap_map_page<pte>(pg, support_);
}

auto pmap<arch::amd64>::maybe_gc(
    std::tuple<page_ptr<arch::amd64>&,
               pmap_mapped_ptr<pml4, arch::amd64>&,
               pml4_record&> pml4,
    std::tuple<page_ptr<arch::amd64>&,
               pmap_mapped_ptr<pdpe, arch::amd64>&,
               pdpe_record&> pdpe,
    std::tuple<page_ptr<arch::amd64>&,
               pmap_mapped_ptr<pdp, arch::amd64>&,
               pdp_record&> pdp,
    std::tuple<page_ptr<arch::amd64>&,
               pmap_mapped_ptr<pte, arch::amd64>&,
               pte_record&> pte)
    noexcept -> void {
  using std::get;
  using std::none_of;

  auto& parent = get<2>(pdp);
  auto& mapped = get<1>(pte);
  auto& ptr = get<0>(pte);

  if (!parent.flags().avl(AVL_CRITICAL) &&
      none_of(mapped->begin(), mapped->end(),
              [](pte_record r) {
                return r.p() || r.flags().avl(AVL_CRITICAL);
              })) {
    parent = pdp_record{ 0 };
    ptr.set_allocated(support_);
    maybe_gc(pml4, pdpe, pdp);
  }
}

auto pmap<arch::amd64>::maybe_gc(
    std::tuple<page_ptr<arch::amd64>&,
               pmap_mapped_ptr<pml4, arch::amd64>&,
               pml4_record&> pml4,
    std::tuple<page_ptr<arch::amd64>&,
               pmap_mapped_ptr<pdpe, arch::amd64>&,
               pdpe_record&> pdpe,
    std::tuple<page_ptr<arch::amd64>&,
               pmap_mapped_ptr<pdp, arch::amd64>&,
               pdp_record&> pdp)
    noexcept -> void {
  using std::get;
  using std::none_of;

  auto& parent = get<2>(pdpe);
  auto& mapped = get<1>(pdp);
  auto& ptr = get<0>(pdp);

  if (!parent.flags().avl(AVL_CRITICAL) &&
      none_of(mapped->begin(), mapped->end(),
              [](pdp_record r) {
                return r.p() || r.flags().avl(AVL_CRITICAL);
              })) {
    parent = pdpe_record{ 0 };
    ptr.set_allocated(support_);
    maybe_gc(pml4, pdpe);
  }
}

auto pmap<arch::amd64>::maybe_gc(
    std::tuple<page_ptr<arch::amd64>&,
               pmap_mapped_ptr<pml4, arch::amd64>&,
               pml4_record&> pml4,
    std::tuple<page_ptr<arch::amd64>&,
               pmap_mapped_ptr<pdpe, arch::amd64>&,
               pdpe_record&> pdpe)
    noexcept -> void {
  using std::get;
  using std::none_of;

  auto& parent = get<2>(pml4);
  auto& mapped = get<1>(pdpe);
  auto& ptr = get<0>(pdpe);

  if (!parent.flags().avl(AVL_CRITICAL) &&
      none_of(mapped->begin(), mapped->end(),
              [](pdpe_record r) {
                return r.p() || r.flags().avl(AVL_CRITICAL);
              })) {
    parent = pml4_record{ 0 };
    ptr.set_allocated(support_);
    maybe_gc(pml4);
  }
}

auto pmap<arch::amd64>::maybe_gc(
    std::tuple<page_ptr<arch::amd64>&,
               pmap_mapped_ptr<pml4, arch::amd64>&,
               pml4_record&> pml4)
    noexcept -> void {
  using std::get;
  using std::none_of;

  auto& parent = this->pml4_;
  auto& mapped = get<1>(pml4);
  auto& ptr = get<0>(pml4);

  if (userspace() &&
      none_of(mapped->begin(), mapped->end(),
              [](pml4_record r) {
                return r.p() || r.flags().avl(AVL_CRITICAL);
              })) {
    parent = page_no<arch::amd64>(0);
    ptr.set_allocated(support_);
  }
}

auto pmap<arch::amd64>::break_large_page_(pdpe_record& pdpe_value,
                                          vaddr<arch::amd64> va) -> void {
  using namespace x86_shared;

  page_no<arch::amd64> pa = pdpe_value.address();
  const auto fl = pdpe_value.flags();
  auto pdp_ptr = page_ptr<arch::amd64>::allocate(support_);
  auto mapped_pdp = map_pdp(pdp_ptr.get(), va);
  for (auto& e : *mapped_pdp) {
    e = pdp_record::create(pa, fl, true);
    pa += page_count<arch::amd64>(N_PTE);
  }
  pdpe_value = pdpe_record::create(pdp_ptr.release(), fl, false);
}

auto pmap<arch::amd64>::break_large_page_(pdp_record& pdp_value,
                                          vaddr<arch::amd64> va) -> void {
  using namespace x86_shared;

  page_no<arch::amd64> pa = pdp_value.address();
  const auto fl = pdp_value.flags();
  auto pte_ptr = page_ptr<arch::amd64>::allocate(support_);
  auto mapped_pte = map_pte(pte_ptr.get(), va);
  for (auto& e : *mapped_pte) {
    e = pte_record::create(pa, fl);
    pa += page_count<arch::amd64>(1);
  }
  pdp_value = pdp_record::create(pte_ptr.release(), fl, false);
}

#ifndef _LOADER
auto pmap<arch::amd64>::deregister_from_pg_(page_no<arch::amd64> pa,
                                            vpage_no<arch::amd64> va,
                                            bool accessed, bool dirty,
                                            page_count<arch::amd64> n)
    noexcept -> void {
  while (n > page_count<arch::amd64>(0)) {
    pmap_page& ppg = support_.lookup_pmap_page(pa);
    ppg.pmap_deregister_(*this, va);
    if (accessed && dirty)
      ppg.mark_accessed_and_dirty();
    else if (accessed)
      ppg.mark_accessed();
    else if (dirty)
      ppg.mark_dirty();

    ++va;
    ++pa;
    --n;
  }
}
#else
auto pmap<arch::amd64>::deregister_from_pg_(page_no<arch::amd64>,
                                            vpage_no<arch::amd64>,
                                            bool, bool,
                                            page_count<arch::amd64>)
    noexcept -> void
{}
#endif  // _LOADER

#ifndef _LOADER
auto pmap<arch::amd64>::add_flags_to_pg_(page_no<arch::amd64> pa,
                                         bool accessed, bool dirty,
                                         page_count<arch::amd64> n)
    noexcept -> void {
  while (n > page_count<arch::amd64>(0)) {
    pmap_page& ppg = support_.lookup_pmap_page(pa);
    if (accessed && dirty)
      ppg.mark_accessed_and_dirty();
    else if (accessed)
      ppg.mark_accessed();
    else if (dirty)
      ppg.mark_dirty();

    ++pa;
    --n;
  }
}
#else
auto pmap<arch::amd64>::add_flags_to_pg_(page_no<arch::amd64>,
                                         bool, bool,
                                         page_count<arch::amd64>)
    noexcept -> void
{}
#endif  // _LOADER


}} /* namespace ilias::pmap */
