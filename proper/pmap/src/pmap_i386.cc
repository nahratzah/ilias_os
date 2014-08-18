#include <ilias/pmap/pmap_i386.h>
#include <abi/ext/log2.h>
#include <ilias/pmap/page_alloc.h>
#include <ilias/pmap/page_alloc_support.h>
#include <algorithm>
#include <tuple>
#include <utility>

namespace ilias {
namespace pmap {


const vpage_no<arch::i386> pmap<arch::i386>::kva_map_self =
    vpage_no<arch::i386>(0xffffffff >> page_shift(arch::i386)) -
    worst_case_npages;


pmap<arch::i386>::~pmap() noexcept {
  clear();
}

auto pmap<arch::i386>::clear() noexcept -> void {
  for (pdpe_record pdpe_value : pdpe_) {
    if (!pdpe_value.p()) continue;  // Skip non-present entries.

    /* Mark this page for destruction. */
    auto pdp_ptr = page_ptr<arch::i386>(pdpe_value.address());
    pdp_ptr.set_allocated(support_);

    /* Process all mapped pdp entries. */
    auto mapped_pdp = pmap_map_page<pdp>(pdp_ptr.get(), support_);
    for (pdp_record pdp_value : *mapped_pdp) {
      if (!pdp_value.p()) continue;  // Skip non-present entries.
      if (pdp_value.ps()) continue;  // Leaf node.

      /* Mark this page for destruction. */
      auto pte_ptr = page_ptr<arch::i386>(pdp_value.address());
      pte_ptr.set_allocated(support_);
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
    return std::make_tuple(pte_addr, (1U << (pte_offset_bits)), p);

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
  unmap_(va, npg);
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
                              page_count<arch::i386> c) noexcept -> void {
  using namespace x86_shared;
  using std::none_of;
  using std::min;
  using std::next;

  auto p = vaddr<arch::i386>(va).get();
  const uint32_t pdpe_off = (p & pdpe_mask) >> pdpe_addr_offset;
  p &= ~pdpe_mask;

  auto pdpe_iter = next(pdpe_.begin(), pdpe_off);
  while (pdpe_iter != pdpe_.end() && c > page_count<arch::i386>(0)) {
    /* Resolve pde offset. */
    const uintptr_t pdp_off = (p & pdp_mask) >> pdp_addr_offset;
    p &= ~pdp_mask;

    if (!pdpe_iter->p()) {
      // 512 * 512 == pdp::size() * pte::size()
      c -= page_count<arch::i386>((N_PDP - pdp_off) * N_PTE);
      continue;
    }

    /*
     * Maybe mark this page for destruction
     * (decided at the end of the loop).
     */
    auto pdp_ptr = page_ptr<arch::i386>(pdpe_iter->address());

    /* Map the page, to descend. */
    auto mapped_pdp = map_pdp(pdp_ptr.get(), pdpe_off);

    auto pdp_iter = next(mapped_pdp->begin(), pdp_off);
    while (pdp_iter != mapped_pdp->end() && c > page_count<arch::i386>(0)) {
      /* Resolve pte offset. */
      const uintptr_t pte_off = (p & pte_mask) >> pte_addr_offset;
      p &= ~pte_mask;

      if (!pdp_iter->p()) {
        // 512 == pte::size()
        c -= page_count<arch::i386>(N_PTE - pte_off);
        p += N_PTE;  // pte_mask was subtracted above.
        continue;
      }

      /* If this is a big page... */
      if (pdp_iter->ps()) {
        bool unmap_entirely = (pte_off == 0 &&
                               c >= page_count<arch::i386>(N_PTE));

        if (!unmap_entirely) {
          /* part of the big page remains in existence:
           * transfer the remaining part to a new page level. */
          try {
            auto new_pdp_ptr = page_ptr<arch::i386>::allocate(support_);
            auto mapped_new_pdp = map_pte(new_pdp_ptr.get(),
                                          pdpe_off, pdp_off);
	    page_no<arch::i386> pg = pdp_iter->address();
            std::generate(mapped_new_pdp->begin(), mapped_new_pdp->end(),
                          [&pg, pdp_iter]() {
                            return pte_record::create(pg++, pdp_iter->flags());
                          });
            std::fill(next(mapped_new_pdp->begin(), pte_off),
                      next(mapped_new_pdp->begin(),
                           min(size_t(pte_off + c.get()), size_t(N_PDP))),
                      pte_record::create(nullptr, pdp_iter->flags() & PT_AVL));
            *pdp_iter = pdp_record::create(new_pdp_ptr.get(),
                                           pdp_iter->flags());
            new_pdp_ptr.release();
          } catch (...) {
            /* Error recovery: unmap the whole thing and
             * let the fault handler sort it out. */
            unmap_entirely = true;  // Handled by if-statement.
          }
        }

        if (unmap_entirely) {
          /* the big page is entirely removed:
           * invalidate pdp entry and continue. */
          *pdp_iter = pdp_record::create(nullptr, pdp_iter->flags(), true);
        }
      } else { /* Descend into pte record. */
        auto pte_ptr = page_ptr<arch::i386>(pdp_iter->address());
        auto mapped_pte = map_pte(pte_ptr.get(), pdp_off, pte_off);

        std::for_each(next(mapped_pte->begin(), pte_off),
                      next(mapped_pte->begin(),
                           min(size_t(pte_off + c.get()), size_t(N_PTE))),
                      [](pte_record& r) {
                        r = pte_record::create(nullptr, r.flags());
                      });

        /* Remove mapping if page becomes empty. */
        if (none_of(mapped_pte->begin(), mapped_pte->end(),
                    [](pte_record r) {
                      return r.p() || r.flags().avl(AVL_CRITICAL);
                    })) {
          *pdp_iter = pdp_record::create(nullptr, pdp_iter->flags());
          pte_ptr.set_allocated(support_);
        }
      }

      c -= page_count<arch::i386>(N_PTE - pte_off);
      p += N_PTE;
      ++pdp_iter;
    }

    /* Remove mapping if page becomes empty. */
    if (none_of(mapped_pdp->begin(), mapped_pdp->end(),
                [](pdp_record r) {
                  return r.p() || r.flags().avl(AVL_CRITICAL);
                })) {
      *pdpe_iter = pdpe_record::create(nullptr, pdpe_iter->flags());
      pdp_ptr.set_allocated(support_);
    }

    ++pdpe_iter;
  }
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
