#include <ilias/pmap/pmap_i386.h>
#include <abi/ext/log2.h>
#include <ilias/pmap/page_alloc.h>
#include <ilias/pmap/page_alloc_support.h>
#include <algorithm>
#include <tuple>
#include <utility>

namespace ilias {
namespace pmap {


pmap<arch::i386>::~pmap() noexcept {
  clear();
}

auto pmap<arch::i386>::clear() noexcept -> void {
  for (pdpe_record pdpe_value : pdpe_) {
    if (!pdpe_value.p()) continue;  // Skip non-present entries.

    /* Mark this page for destruction. */
    auto pdp_ptr =
        page_ptr<arch::i386>(page_no<arch::i386>(pdpe_value.page_no()));
    pdp_ptr.set_allocated(support_);

    /* Process all mapped pdp entries. */
    auto mapped_pdp = pmap_map_page<pdp>(pdp_ptr.get(), support_);
    for (pdp_record pdp_value : *mapped_pdp) {
      if (!pdp_value.p()) continue;  // Skip non-present entries.
      if (pdp_value.ps()) continue;  // Leaf node.

      /* Mark this page for destruction. */
      auto pte_ptr =
          page_ptr<arch::i386>(page_no<arch::i386>(pdp_value.page_no()));
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
  const auto pdp_addr = page_no<arch::i386>(pdpe_value.page_no());

  /* Resolve pdp offset. */
  const uintptr_t pdp_off = (p & pdp_mask) >> pdp_addr_offset;
  p &= ~pdp_mask;

  /* Resolve pdp. */
  const pdp_record pdp_value =
      (*pmap_map_page<pdp>(pdp_addr, support_))[pdp_off];
  if (_predict_false(!pdp_value.p())) throw efault(va.get());
  const auto pte_addr = page_no<arch::i386>(pdp_value.page_no());

  if (pdp_value.ps())
    return std::make_tuple(pte_addr, (1U << (pte_offset_bits)), p);

  /* Resolve pte offet. */
  const uintptr_t pte_off = (p & pte_mask) >> pte_addr_offset;
  p &= ~pte_mask;

  /* Resolve pte. */
  const pte_record pte_value =
      (*pmap_map_page<pte>(pte_addr, support_))[pte_off];
  if (_predict_false(!pte_value.p())) throw efault(va.get());
  const auto pg = page_no<arch::i386>(pte_value.page_no());

  return std::make_tuple(pg, 1, p);
}

auto pmap<arch::i386>::map(vpage_no<arch::i386> va, page_no<arch::i386> pg,
                           permission perm) -> void {
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
    pdp_ptr = page_ptr<arch::i386>(page_no<arch::i386>(pdpe_value.page_no()));
  auto mapped_pdp = pmap_map_page<pdp>(pdp_ptr.get(), support_);
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
    pte_ptr = page_ptr<arch::i386>(page_no<arch::i386>(pdp_value.page_no()));
  auto mapped_pte = pmap_map_page<pte>(pte_ptr.get(), support_);
  /* Clear PTE if it was newly allocated. */
  if (pte_ptr.is_allocated())
    std::fill(mapped_pte->begin(), mapped_pte->end(), pte_record{ 0 });

  /* Resolve pte offset. */
  const uintptr_t pte_off = (p & pte_mask) >> pte_addr_offset;
  p &= ~pte_mask;

  /* Resolve page entry. */
  pte_record& pte_value = (*mapped_pte)[pte_off];
  if (pte_value.p()) {
    if (pte_value.page_no() == pg.get()) return;
    throw efault(va.get());
  }

  /* Assign page entry. */
  {
    pte_record new_pte_value{ 0 };
    new_pte_value.page_no(pg.get());
    new_pte_value.p(true);
    new_pte_value.us(support_.userspace);
    new_pte_value = new_pte_value.combine(perm);
    assert(new_pte_value.valid());
    pte_value = new_pte_value;
  }

  /* Assign pte to pdp, iff newly allocated. */
  if (pte_ptr.is_allocated()) {
    pdp_record new_pdp_value{ 0 };
    new_pdp_value.page_no(pte_ptr.release().get());
    new_pdp_value.p(true);
    new_pdp_value.us(support_.userspace);
    new_pdp_value = new_pdp_value.combine(perm);
    assert(new_pdp_value.valid());
    pdp_value = new_pdp_value;
  } else {
    pdp_record new_pdp_value = pdp_value.combine(perm);
    assert(new_pdp_value.valid());
    pdp_value = new_pdp_value;
  }

  /* Assign pdp to pdpe, iff newly allocated. */
  if (pdp_ptr.is_allocated()) {
    pdpe_record new_pdpe_value{ 0 };
    new_pdpe_value.page_no(pdp_ptr.release().get());
    new_pdpe_value.p(true);
    new_pdpe_value = new_pdpe_value.combine(perm);
    assert(new_pdpe_value.valid());
    pdpe_value = new_pdpe_value;
  } else {
    pdpe_record new_pdpe_value = pdpe_value.combine(perm);
    assert(new_pdpe_value.valid());
    pdpe_value = new_pdpe_value;
  }

  return;
}

auto pmap<arch::i386>::unmap(vpage_no<arch::i386> va,
                             page_count<arch::i386> c) noexcept -> void {
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
    auto pdp_ptr =
        page_ptr<arch::i386>(page_no<arch::i386>(pdpe_iter->page_no()));

    /* Map the page, to descend. */
    auto mapped_pdp = pmap_map_page<pdp>(pdp_ptr.get(), support_);

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
            auto mapped_new_pdp = pmap_map_page<pte>(new_pdp_ptr.get(),
                                                     support_);
            pte_record pg = pte_record::convert(*pdp_iter);
            std::generate(mapped_new_pdp->begin(), mapped_new_pdp->end(),
                          [&pg]() {
                            pte_record rv = pg;
                            page_no<arch::i386> pgno(pg.page_no());
                            ++pgno;
                            pg.page_no(pgno.get());
                            return rv;
                          });
            std::fill(next(mapped_new_pdp->begin(), pte_off),
                      next(mapped_new_pdp->begin(),
                           min(size_t(pte_off + c.get()), size_t(N_PDP))),
                      pte_record{ 0 });
            pdp_iter->ps(false);
            pdp_iter->page_no(
                page_no<arch::i386>(new_pdp_ptr.release()).get());
          } catch (...) {
            /* Error recovery: unmap the whole thing and
             * let the fault handler sort it out. */
            unmap_entirely = true;  // Handled by if-statement.
          }
        }

        if (unmap_entirely) {
          /* the big page is entirely removed:
           * invalidate pdp entry and continue. */
          pdp_iter->p(false);
        }
      } else { /* Descend into pte record. */
        auto pte_ptr =
            page_ptr<arch::i386>(page_no<arch::i386>(pdp_iter->page_no()));
        auto mapped_pte = pmap_map_page<pte>(pte_ptr.get(), support_);

        std::fill(next(mapped_pte->begin(), pte_off),
                  next(mapped_pte->begin(),
                       min(size_t(pte_off + c.get()), size_t(N_PTE))),
                  pte_record{ 0 });

        /* Remove mapping if page becomes empty. */
        if (none_of(mapped_pte->begin(), mapped_pte->end(),
                    [](pte_record r) { return r.p(); })) {
          pdp_iter->p(false);
          pte_ptr.set_allocated(support_);
        }
      }

      c -= page_count<arch::i386>(N_PTE - pte_off);
      p += N_PTE;
      ++pdp_iter;
    }

    /* Remove mapping if page becomes empty. */
    if (none_of(mapped_pdp->begin(), mapped_pdp->end(),
                [](pdp_record r) { return r.p(); })) {
      pdpe_iter->p(false);
      pdp_ptr.set_allocated(support_);
    }

    ++pdpe_iter;
  }
}


}} /* namespace ilias::pmap */
