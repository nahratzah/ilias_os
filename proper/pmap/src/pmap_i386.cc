#include <ilias/pmap/pmap_i386.h>
#include <ilias/pmap/page_alloc.h>
#include <ilias/pmap/page_alloc_support.h>
#include <ilias/pmap/pmap_page.h>
#include <algorithm>
#include <tuple>
#include <utility>

namespace ilias {
namespace pmap {


constexpr std::array<size_t, 2> pmap<arch::i386>::N_PAGES;

constexpr uint64_t pmap<arch::i386>::lp_pdp_pgno_align;

constexpr unsigned int pmap<arch::i386>::offset_bits;
constexpr unsigned int pmap<arch::i386>::pte_offset_bits;
constexpr unsigned int pmap<arch::i386>::pdp_offset_bits;
constexpr unsigned int pmap<arch::i386>::pdpe_offset_bits;

constexpr unsigned int pmap<arch::i386>::pte_addr_offset;
constexpr unsigned int pmap<arch::i386>::pdp_addr_offset;
constexpr unsigned int pmap<arch::i386>::pdpe_addr_offset;

constexpr uint32_t pmap<arch::i386>::pdpe_mask;
constexpr uint32_t pmap<arch::i386>::pdp_mask;
constexpr uint32_t pmap<arch::i386>::pte_mask;

constexpr unsigned int pmap<arch::i386>::N_PDPE;
constexpr unsigned int pmap<arch::i386>::N_PDP;
constexpr unsigned int pmap<arch::i386>::N_PTE;

constexpr page_count<arch::i386> pmap<arch::i386>::worst_case_npages;
constexpr vpage_no<arch::i386> pmap<arch::i386>::kva_map_self;
constexpr vpage_no<arch::i386> pmap<arch::i386>::kva_map_self_pdp;
constexpr vpage_no<arch::i386> pmap<arch::i386>::kva_map_self_pte;

constexpr unsigned int pmap_map<pmap<arch::i386>>::N_PDPE;
constexpr unsigned int pmap_map<pmap<arch::i386>>::N_PDP;
constexpr unsigned int pmap_map<pmap<arch::i386>>::N_PTE;

constexpr unsigned int pmap_map<pmap<arch::i386>>::pdpe_addr_offset;
constexpr unsigned int pmap_map<pmap<arch::i386>>::pdp_addr_offset;
constexpr unsigned int pmap_map<pmap<arch::i386>>::pte_addr_offset;
constexpr uint32_t pmap_map<pmap<arch::i386>>::pdpe_mask;
constexpr uint32_t pmap_map<pmap<arch::i386>>::pdp_mask;
constexpr uint32_t pmap_map<pmap<arch::i386>>::pte_mask;


pmap<arch::i386>::~pmap() noexcept {
  clear();
}

auto pmap<arch::i386>::clear() noexcept -> void {
  auto va = vpage_no<arch::i386>(0);

  for (pdpe_record pdpe_value : pdpe_) {
    constexpr auto pdpe_skip = page_count<arch::i386>(N_PTE * N_PDP);
    if (!pdpe_value.p()) {
      va += pdpe_skip;
      continue;  // Skip non-present entries.
    }
    /* pdpe_value has no page-select bit. */

    /* Mark this page for destruction. */
    auto pdp_ptr = page_ptr<arch::i386>(pdpe_value.address());
    pdp_ptr.set_allocated(support_);

    /* Process all mapped pdp entries. */
    auto mapped_pdp = pmap_map_page<pdp>(pdp_ptr.get(), support_);
    for (pdp_record pdp_value : *mapped_pdp) {
      constexpr auto pdp_skip = page_count<arch::i386>(N_PTE);
      if (!pdp_value.p()) {
        va += pdp_skip;
        continue;  // Skip non-present entries.
      }
      if (pdp_value.ps()) {
        deregister_from_pg_(pdp_value.address(), va, pdp_skip);
        continue;  // Leaf node.
      }

      /* Mark this page for destruction. */
      auto pte_ptr = page_ptr<arch::i386>(pdp_value.address());
      pte_ptr.set_allocated(support_);

      /* Process all mapped pte entries. */
      auto mapped_pte = pmap_map_page<pte>(pte_ptr.get(), support_);
      for (pte_record pte_value : *mapped_pte) {
        /* Release the page. */
        if (pte_value.p())
          deregister_from_pg_(pte_value.address(), va);
        va += page_count<arch::i386>(1);
      }
    }
  }
}

auto pmap<arch::i386>::virt_to_phys(vaddr<arch::i386> p) const ->
    phys_addr<arch::i386> {
  phys_addr<arch::i386> paddr;
  uintptr_t off;

  std::tie(paddr, std::ignore, off) = virt_to_page(p);
  return phys_addr<arch::i386>(paddr.get() + off);
}

auto pmap<arch::i386>::virt_to_page(vaddr<arch::i386> va) const ->
    std::tuple<page_no<arch::i386>, size_t, uintptr_t> {
  auto p = va.get();

  /* Resolve pdpe offset. */
  const uintptr_t pdpe_off = (p & pdpe_mask) >> pdpe_addr_offset;
  p &= ~pdpe_mask;

  /* Resolve pdpe. */
  const pdpe_record pdpe_value = pdpe_[pdpe_off];
  if (_predict_false(!pdpe_value.p())) throw efault(va.get());
  const page_no<arch::i386> pdp_addr = pdpe_value.address();

  /* Resolve pdp offset. */
  const uintptr_t pdp_off = (p & pdp_mask) >> pdp_addr_offset;
  p &= ~pdp_mask;

  /* Resolve pdp. */
  const pdp_record pdp_value =
      (*map_pdp(pdp_addr, va))[pdp_off];
  if (_predict_false(!pdp_value.p())) throw efault(va.get());
  const page_no<arch::i386> pte_addr = pdp_value.address();

  if (pdp_value.ps())
    return std::make_tuple(pte_addr, (1U << pte_offset_bits), p);

  /* Resolve pte offet. */
  const uintptr_t pte_off = (p & pte_mask) >> pte_addr_offset;
  p &= ~pte_mask;

  /* Resolve pte. */
  const pte_record pte_value =
      (*map_pte(pte_addr, va))[pte_off];
  if (_predict_false(!pte_value.p())) throw efault(va.get());
  const page_no<arch::i386> pg = pte_value.address();

  return std::make_tuple(pg, 1, p);
}

auto pmap<arch::i386>::reduce_permission(vpage_no<arch::i386> va,
                                         permission perm) ->
    reduce_permission_result {
  vpage_no<arch::i386> lo, hi;
  std::tie(lo, hi) = managed_range();
  if (va < lo || va >= hi)
    throw std::out_of_range("va outside of managed range");
  return reduce_permission_(va, perm);
}

auto pmap<arch::i386>::map(vpage_no<arch::i386> va,
                           page_no<arch::i386> pa,
                           permission perm) -> void {
  vpage_no<arch::i386> lo, hi;
  std::tie(lo, hi) = managed_range();
  if (va < lo || va >= hi)
    throw std::out_of_range("va outside of managed range");
  map_(va, pa, perm);
}

auto pmap<arch::i386>::unmap(vpage_no<arch::i386> va,
                             page_count<arch::i386> npg) -> void {
  const auto va_end = va + npg;
  if (va_end < va)
    throw std::length_error("too many pages: va wraps around");

  vpage_no<arch::i386> lo, hi;
  std::tie(lo, hi) = managed_range();
  if (va < lo || va_end > hi)
    throw std::out_of_range("va outside of managed range");
  unmap_(va, npg, true);
}

auto pmap<arch::i386>::reduce_permission_(vpage_no<arch::i386> va,
                                          permission perm) noexcept ->
    reduce_permission_result {
  using namespace x86_shared;

  const auto p = vaddr<arch::i386>(va).get();

  /* Resolve pdpe offset. */
  const uint32_t pdpe_off = (p & pdpe_mask) >> pdpe_addr_offset;

  /* Resolve pde. */
  pdpe_record& pdpe_value = pdpe_[pdpe_off];
  if (!pdpe_value.p())
    return reduce_permission_result::UNMAPPED;
  auto pdp_ptr = page_ptr<arch::i386>(pdpe_value.address());
  auto mapped_pdp = map_pdp(pdp_ptr.get(), va);

  /* Resolve pde offset. */
  const uintptr_t pdp_off = (p & pdp_mask) >> pdp_addr_offset;

  /* Resolve pte. */
  pdp_record& pdp_value = (*mapped_pdp)[pdp_off];
  if (!pdp_value.p())
    return reduce_permission_result::UNMAPPED;
  if (pdp_value.ps()) {  /* Resolve new permissions. */
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

    maybe_gc(pdpe_[pdpe_off],
             tie(pdp_ptr, mapped_pdp, pdp_value));
    return reduce_permission_result::UNMAPPED;
  }
  auto pte_ptr = page_ptr<arch::i386>(pdp_value.address());
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

    maybe_gc(pdpe_[pdpe_off],
             tie(pdp_ptr, mapped_pdp, pdp_value),
             tie(pte_ptr, mapped_pte, pte_value));
    return reduce_permission_result::UNMAPPED;
  }
}

auto pmap<arch::i386>::map_(vpage_no<arch::i386> va, page_no<arch::i386> pg,
                            permission perm) -> void {
  using namespace x86_shared;

  auto p = vaddr<arch::i386>(va).get();
  page_ptr<arch::i386> pdp_ptr;
  page_ptr<arch::i386> pte_ptr;

  /* Resolve pdpe offset. */
  const uint32_t pdpe_off = (p & pdpe_mask) >> pdpe_addr_offset;
  p &= ~pdpe_mask;

  /* Resolve pde. */
  pdpe_record& pdpe_value = pdpe_[pdpe_off];
  if (!pdpe_value.p())
    pdp_ptr = page_ptr<arch::i386>::allocate(support_);
  else
    pdp_ptr = page_ptr<arch::i386>(pdpe_value.address());
  auto mapped_pdp = map_pdp(pdp_ptr.get(), va);
  /* Clear PDP if it was newly allocated. */
  if (pdp_ptr.is_allocated())
    std::fill(mapped_pdp->begin(), mapped_pdp->end(), pdp_record{ 0 });

  /* Resolve pde offset. */
  const uintptr_t pdp_off = (p & pdp_mask) >> pdp_addr_offset;
  p &= ~pdp_mask;

  /* Resolve pte. */
  pdp_record& pdp_value = (*mapped_pdp)[pdp_off];
  if (!pdp_value.p())
    pte_ptr = page_ptr<arch::i386>::allocate(support_);
  else
    pte_ptr = page_ptr<arch::i386>(pdp_value.address());
  auto mapped_pte = map_pte(pte_ptr.get(), va);
  /* Clear PTE if it was newly allocated. */
  if (pte_ptr.is_allocated())
    std::fill(mapped_pte->begin(), mapped_pte->end(), pte_record{ 0 });

  /* Resolve pte offset. */
  const uintptr_t pte_off = (p & pte_mask) >> pte_addr_offset;
  p &= ~pte_mask;

  /* Resolve page entry. */
  pte_record& pte_value = (*mapped_pte)[pte_off];

  /* Assign page entry. */
  {
    const auto new_pte_value = pte_record::create(
        pg, (support_.userspace ? PT_US : flags{ 0 }) |
            (pte_value.flags() & PT_AVL)).combine(perm);
    assert(new_pte_value.valid());
    pte_value = new_pte_value;
  }

  /* Assign pte to pdp, iff newly allocated. */
  if (pte_ptr.is_allocated()) {
    const auto new_pdp_value = pdp_record::create(
        pte_ptr.get(), (support_.userspace ? PT_US : flags{ 0 }) |
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
        pdp_ptr.get(), (support_.userspace ? PT_US : flags{ 0 }) |
                       (pdpe_value.flags() & PT_AVL)).combine(perm);
    assert(new_pdpe_value.valid());
    pdpe_value = new_pdpe_value;
    pdp_ptr.release();
  } else {
    pdpe_record new_pdpe_value = pdpe_value.combine(perm);
    assert(new_pdpe_value.valid());
    pdpe_value = new_pdpe_value;
  }

  return;
}

auto pmap<arch::i386>::unmap_(vpage_no<arch::i386> va,
                              page_count<arch::i386> c,
                              bool do_deregister) noexcept -> void {
  using namespace x86_shared;
  using std::none_of;
  using std::min;
  using std::next;

  auto p = vaddr<arch::i386>(va).get();
  /* Resolve pdpe offset. */
  auto pdpe_off = (p & pdpe_mask) >> pdpe_addr_offset;
  /* Resolve pdp offset. */
  auto pdp_off = (p & pdp_mask) >> pdp_addr_offset;
  /* Resolve pte offset. */
  auto pte_off = (p & pte_mask) >> pte_addr_offset;

  /* Iterate in pdpe. */
  for (auto pdpe_iter = next(pdpe_.begin(), pdpe_off);
       pdpe_iter != pdpe_.end() && c > page_count<arch::i386>(0);
       pdp_off = 0, ++pdpe_iter) {
    const auto pdpe_skip =  // Number of entries covered.
	min(c, page_count<arch::i386>(N_PTE * N_PDP * (N_PDPE - pdpe_off)));
    if (!pdpe_iter->p()) {  // PDPE not present.
      c -= pdpe_skip;
      va += pdpe_skip;
      continue;
    }

    /* Iterate in pdp. */
    auto pdp_ptr = page_ptr<arch::i386>(pdpe_iter->address());
    auto mapped_pdp = map_pdp(pdp_ptr.get(), va);
    for (auto pdp_iter = next(mapped_pdp->begin(), pdp_off);
         pdp_iter != mapped_pdp->end() && c > page_count<arch::i386>(0);
	 pdp_off = 0, ++pdp_iter) {
      const auto pdp_skip =  // Number of entries covered.
	  min(c, page_count<arch::i386>(N_PTE - pte_off));
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
	  if (pdp_skip != page_count<arch::i386>(N_PTE)) {
	    break_large_page_(*pdp_iter, va);
	    broken_up = true;
	  }
	} catch (...) {
	  /* SKIP: broken_up = false, handled below. */
	}

	if (!broken_up) {
	  if (do_deregister) {
	    deregister_from_pg_(pdp_iter->address(), va,
				page_count<arch::i386>(N_PTE));
	  }
	  va += page_count<arch::i386>(N_PTE);

	  *pdp_iter = pdp_record::create(nullptr, pdp_iter->flags());
	  maybe_gc(*pdpe_iter,
	           std::tie(pdp_ptr, mapped_pdp, *pdp_iter));
	  continue;
	}
      }

      /* Iterate in pte. */
      auto pte_ptr = page_ptr<arch::i386>(pdp_iter->address());
      auto mapped_pte = map_pte(pte_ptr.get(), va);
      for (auto pte_iter = next(mapped_pte->begin(), pte_off);
	   pte_iter != mapped_pte->end() && c > page_count<arch::i386>(0);
	   ++va, --c, ++pte_iter) {
	if (!pte_iter->p()) {  // PTE not present.
	  /* Increment/decrement logic is present in loop. */
	  continue;
	}

	if (pte_iter->p() && do_deregister)
	  deregister_from_pg_(pte_iter->address(), va);

	*pte_iter = pte_record::create(nullptr, pte_iter->flags());
	maybe_gc(*pdpe_iter,
	         std::tie(pdp_ptr, mapped_pdp, *pdp_iter),
	         std::tie(pte_ptr, mapped_pte, *pte_iter));
      }  // Iterate in pte.
    }  // Iterate in pdp.
  }  // Iterate in pdpe.
}

auto pmap<arch::i386>::map_pdp(page_no<arch::i386> pg, vaddr<arch::i386> va)
    const -> pmap_mapped_ptr<pdp, arch::i386> {
  if (kva_map_self_enabled_)
    return pmap_map_page<pdp>(kva_pdp_entry(va));
  return pmap_map_page<pdp>(pg, support_);
}

auto pmap<arch::i386>::map_pte(page_no<arch::i386> pg, vaddr<arch::i386> va)
    const -> pmap_mapped_ptr<pte, arch::i386> {
  if (kva_map_self_enabled_)
    return pmap_map_page<pte>(kva_pte_entry(va));
  return pmap_map_page<pte>(pg, support_);
}

auto pmap<arch::i386>::map_pdp(page_no<arch::i386> pg, unsigned int pdpe_idx)
    const -> pmap_mapped_ptr<pdp, arch::i386> {
  if (kva_map_self_enabled_)
    return pmap_map_page<pdp>(kva_pdp_entry(pdpe_idx));
  return pmap_map_page<pdp>(pg, support_);
}

auto pmap<arch::i386>::map_pte(page_no<arch::i386> pg,
                               unsigned int pdpe_idx, unsigned int pdp_idx)
    const -> pmap_mapped_ptr<pte, arch::i386> {
  if (kva_map_self_enabled_)
    return pmap_map_page<pte>(kva_pte_entry(pdpe_idx, pdp_idx));
  return pmap_map_page<pte>(pg, support_);
}

auto pmap<arch::i386>::maybe_gc(
    pdpe_record& pdpe,
    std::tuple<page_ptr<arch::i386>&,
               pmap_mapped_ptr<pdp, arch::i386>&,
               pdp_record&> pdp,
    std::tuple<page_ptr<arch::i386>&,
               pmap_mapped_ptr<pte, arch::i386>&,
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
    maybe_gc(pdpe, pdp);
  }
}

auto pmap<arch::i386>::maybe_gc(
    pdpe_record& pdpe,
    std::tuple<page_ptr<arch::i386>&,
               pmap_mapped_ptr<pdp, arch::i386>&,
               pdp_record&> pdp)
    noexcept -> void {
  using std::get;
  using std::none_of;

  auto& parent = pdpe;
  auto& mapped = get<1>(pdp);
  auto& ptr = get<0>(pdp);

  if (!parent.flags().avl(AVL_CRITICAL) &&
      none_of(mapped->begin(), mapped->end(),
              [](pdp_record r) {
                return r.p() || r.flags().avl(AVL_CRITICAL);
              })) {
    parent = pdpe_record{ 0 };
    ptr.set_allocated(support_);
    maybe_gc(pdpe);
  }
}

auto pmap<arch::i386>::break_large_page_(pdp_record& pdp_value,
                                         vaddr<arch::i386> va) -> void {
  using namespace x86_shared;

  page_no<arch::i386> pa = pdp_value.address();
  const auto fl = pdp_value.flags();
  auto pte_ptr = page_ptr<arch::i386>::allocate(support_);
  auto mapped_pte = map_pte(pte_ptr.get(), va);
  for (auto& e : *mapped_pte) {
    e = pte_record::create(pa, fl);
    pa += page_count<arch::i386>(1);
  }
  pdp_value = pdp_record::create(pte_ptr.release(), fl, false);
}

#ifndef _LOADER
auto pmap<arch::i386>::deregister_from_pg_(page_no<arch::i386> pa,
                                           vpage_no<arch::i386> va,
                                           page_count<arch::i386> n)
    noexcept -> void {
  while (n > page_count<arch::i386>(0)) {
    pmap_page& ppg = support_.lookup_pmap_page(pa);
    ppg.pmap_deregister_(*this, va);

    ++va;
    ++pa;
    --n;
  }
}
#else
auto pmap<arch::i386>::deregister_from_pg_(page_no<arch::i386>,
                                           vpage_no<arch::i386>,
                                           page_count<arch::i386>)
    noexcept -> void
{}
#endif


auto pmap_map<pmap<arch::i386>>::push_back(page_no<arch::i386> pg,
                                           permission perm,
                                           page_count<arch::i386> npg) ->
    void {
  using x86_shared::PT_AVL;

  auto pdpe_idx = (vaddr<arch::i386>(va_).get() & pdpe_mask) >>
                  pdpe_addr_offset;
  auto pdp_idx = (vaddr<arch::i386>(va_).get() & pdp_mask) >>
                 pdp_addr_offset;
  auto pte_idx = (vaddr<arch::i386>(va_).get() & pte_mask) >>
                 pte_addr_offset;

  if (npg < page_count<arch::i386>(0))
    throw std::invalid_argument("cannot map negative page count");
  if (va_end_ - va_ < npg) throw std::range_error("too many pages");

  while (npg > page_count<arch::i386>(0)) {
    pmap_->pdpe_[pdpe_idx] = pmap_->pdpe_[pdpe_idx].combine(perm);

    /*
     * We can map this in as a large page if:
     * - the vaddr is large-page-aligned (pte_idx == 0)
     * - the number of pages is sufficient (npg >= N_PTE)
     * - the physical page is large-page-aligned (pg % align == 0)
     * - there is no PDP (!pdpe.p()), or
     *   there is no PTE (pte_is_lp_convertible()), or
     *   the AVL bits on all the entries in the PTE are equal
     *   (pte_is_lp_convirtible()).
     */
    if (pte_idx == 0 && npg >= page_count<arch::i386>(N_PTE) &&
        pg.get() % pmap<arch::i386>::lp_pdp_pgno_align == 0 &&
        (!pmap_->pdpe_[pdpe_idx].p() ||
         pte_is_lp_convertible(pdpe_idx, pdp_idx))) {
      if (!pdp_ptr_) load_pdp_ptr_(pdpe_idx);

      page_ptr<arch::i386> old;
      if ((*pdp_ptr_)[pdp_idx].p() && !(*pdp_ptr_)[pdp_idx].ps())
        old = page_ptr<arch::i386>((*pdp_ptr_)[pdp_idx].address());
      (*pdp_ptr_)[pdp_idx] = pdp_record::create(
          pg, (*pdp_ptr_)[pdp_idx].flags() & PT_AVL).combine(perm);
      if (old)
        old.set_allocated(pmap_->support_);

      va_ += page_count<arch::i386>(N_PTE);
      pte_ptr_ = nullptr;
      if (++pdp_idx == N_PDP) {
        pdp_idx = 0;
        ++pdpe_idx;
        pdp_ptr_ = nullptr;
        npg -= page_count<arch::i386>(N_PTE);
        pg += page_count<arch::i386>(N_PTE);
      }
    } else {
      if (!pdp_ptr_) load_pdp_ptr_(pdpe_idx);
      (*pdp_ptr_)[pdp_idx] = (*pdp_ptr_)[pdp_idx].combine(perm);

      if (!pte_ptr_) load_pte_ptr_(pdpe_idx, pdp_idx);
      do {
        (*pte_ptr_)[pte_idx] = pte_record::create(
            pg, (*pte_ptr_)[pte_idx].flags() & PT_AVL).combine(perm);
        npg -= page_count<arch::i386>(1);
        pg += page_count<arch::i386>(1);
      } while (++pte_idx != N_PTE && npg > page_count<arch::i386>(0));

      if (pte_idx == N_PTE) {
        pte_idx = 0;
        if (++pdp_idx == N_PDP) {
          pdp_idx = 0;
          ++pdpe_idx;
          pdp_ptr_ = nullptr;
        }
        pte_ptr_ = nullptr;
      }
    }
  }
}

bool pmap_map<pmap<arch::i386>>::pte_is_lp_convertible(size_t pdpe_idx,
                                                       size_t pdp_idx)
    const {
  using x86_shared::PT_AVL;
  using std::bind;
  using std::placeholders::_1;
  using std::next;
  using std::all_of;

  if (!pdp_ptr_) load_pdp_ptr_(pdpe_idx);

  if (!(*pdp_ptr_)[pdp_idx].p()) return true;
  return all_of(next(pte_ptr_->begin()), pte_ptr_->end(),
                bind([](const pte_record& x, const pte_record& y) {
                       return (x.flags() & PT_AVL) == (y.flags() & PT_AVL);
                     },
                     _1, *pte_ptr_->begin()));
}

auto pmap_map<pmap<arch::i386>>::load_pdp_ptr_(size_t pdpe_idx) const -> void {
  using x86_shared::PT_AVL;

  assert(pdpe_idx < N_PDPE);

  page_ptr<arch::i386> pdp_pg;
  pdp_ptr_ = nullptr;
  pdp_pg = (pmap_->pdpe_[pdpe_idx].p() ?
            page_ptr<arch::i386>(pmap_->pdpe_[pdpe_idx].address()) :
            page_ptr<arch::i386>::allocate(pmap_->support_));
  pdp_ptr_ = pmap_->map_pdp(pdp_pg.get(), pdpe_idx);
  if (pdp_pg.is_allocated()) {
    std::fill(pdp_ptr_->begin(), pdp_ptr_->end(),
              pdp_record::create(nullptr,
                                 pmap_->pdpe_[pdpe_idx].flags() & PT_AVL));
    pmap_->pdpe_[pdpe_idx] =
        pdpe_record::create(pdp_pg.get(),
                            pmap_->pdpe_[pdpe_idx].flags() & PT_AVL);
    pdp_pg.release();
  }
}

auto pmap_map<pmap<arch::i386>>::load_pte_ptr_(size_t pdpe_idx, size_t pdp_idx)
    const -> void {
  using std::bind;
  using x86_shared::PT_AVL;

  assert(pdpe_idx < N_PDPE);
  assert(pdp_idx < N_PDP);

  page_ptr<arch::i386> pte_pg;
  pte_ptr_ = nullptr;
  if (!pdp_ptr_) load_pdp_ptr_(pdpe_idx);
  pte_pg = ((*pdp_ptr_)[pdp_idx].p() ?
            page_ptr<arch::i386>((*pdp_ptr_)[pdp_idx].address()) :
            page_ptr<arch::i386>::allocate(pmap_->support_));
  pte_ptr_ = pmap_->map_pte(pte_pg.get(), pdpe_idx, pdp_idx);
  if (pte_pg.is_allocated()) {
    /*
     * If the PDP already holds a page, split it up into smaller pages.
     */
    if ((*pdp_ptr_)[pdp_idx].p()) {
      page_no<arch::i386> pgi = (*pdp_ptr_)[pdp_idx].address();
      std::generate(pte_ptr_->begin(), pte_ptr_->end(),
                    bind([&pgi](const x86_shared::flags& fl) {
                           return pte_record::create(pgi++, fl);
                         },
                         (*pdp_ptr_)[pdp_idx].flags()));
    } else {
      std::fill(pte_ptr_->begin(), pte_ptr_->end(),
                pte_record::create(nullptr,
                                   (*pdp_ptr_)[pdp_idx].flags() & PT_AVL));
    }

    (*pdp_ptr_)[pdp_idx] =
        pdp_record::create(pte_pg.get(),
                           (*pdp_ptr_)[pdp_idx].flags() & PT_AVL);
    pte_pg.release();
  }
}


}} /* namespace ilias::pmap */
