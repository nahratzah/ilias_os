#include <ilias/pmap/pmap_i386.h>
#include <algorithm>
#include <abi/ext/log2.h>
#include <ilias/pmap/page_alloc.h>
#include <ilias/pmap/page_alloc_support.h>
#include <new>
#include <tuple>

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

auto pmap<arch::i386>::map(vpage_no<arch::i386> va, page_no<arch::i386> pg) ->
    void {
  auto p = va.get();
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
  if (pte_ptr.is_allocated())
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
    assert(new_pte_value.valid());
    pte_value = new_pte_value;
  }

  /* Assign pte to pdp, iff newly allocated. */
  if (pte_ptr.is_allocated()) {
    pdp_record new_pdp_value{ 0 };
    new_pdp_value.page_no(pte_ptr.release().get());
    new_pdp_value.p(true);
    assert(new_pdp_value.valid());
    pdp_value = new_pdp_value;
  }

  /* Assign pdp to pdpe, iff newly allocated. */
  if (pdp_ptr.is_allocated()) {
    pdpe_record new_pdpe_value{ 0 };
    new_pdpe_value.page_no(pdp_ptr.release().get());
    new_pdpe_value.p(true);
    assert(new_pdpe_value.valid());
    pdpe_value = new_pdpe_value;
  }

  return;
}


}} /* namespace ilias::pmap */
