#ifndef _ILIAS_PMAP_X86_SHARED_INL_H_
#define _ILIAS_PMAP_X86_SHARED_INL_H_

#include <ilias/pmap/x86_shared.h>
#include <cassert>
#include <cstdint>
#include <stdexcept>

namespace ilias {
namespace pmap {
namespace x86_shared {


constexpr page_no_proxy::page_no_proxy(page_no<arch::i386> pg) noexcept
: page_no_proxy(pg.get())
{}

constexpr page_no_proxy::page_no_proxy(page_no<arch::amd64> pg) noexcept
: page_no_proxy(pg.get())
{}

constexpr page_no_proxy::operator page_no<arch::i386>() const noexcept {
  return page_no<arch::i386>(pgno_);
}

constexpr page_no_proxy::operator page_no<arch::amd64>() const noexcept {
  return page_no<arch::amd64>(pgno_);
}

constexpr bool page_no_proxy::operator==(const page_no_proxy& o)
    const noexcept {
  return pgno_ == o.pgno_;
}

constexpr bool page_no_proxy::operator!=(const page_no_proxy& o)
    const noexcept {
  return pgno_ != o.pgno_;
}

constexpr bool page_no_proxy::operator<(const page_no_proxy& o)
    const noexcept {
  return pgno_ < o.pgno_;
}

constexpr bool page_no_proxy::operator<=(const page_no_proxy& o)
    const noexcept {
  return pgno_ <= o.pgno_;
}

constexpr bool page_no_proxy::operator>(const page_no_proxy& o)
    const noexcept {
  return pgno_ > o.pgno_;
}

constexpr bool page_no_proxy::operator>=(const page_no_proxy& o)
    const noexcept {
  return pgno_ >= o.pgno_;
}


constexpr flags PT_AVL_mask(unsigned int idx) {
  if (idx >= flags::AVL_COUNT)
    throw std::out_of_range("avl index");
  return flags(PT_AVL0.get() << idx);
}


constexpr uint64_t flags::get() const noexcept {
  return (*this & PT_ALL_FLAGS).fl_;
}

constexpr bool flags::rw() const noexcept {
  return bool(*this & PT_RW);
}
constexpr bool flags::us() const noexcept {
  return bool(*this & PT_US);
}
constexpr bool flags::pwt() const noexcept {
  return bool(*this & PT_PWT);
}
constexpr bool flags::pcd() const noexcept {
  return bool(*this & PT_PCD);
}
constexpr bool flags::a() const noexcept {
  return bool(*this & PT_A);
}
constexpr bool flags::d() const noexcept {
  return bool(*this & PT_D);
}
constexpr bool flags::g() const noexcept {
  return bool(*this & PT_G);
}
constexpr bool flags::avl(unsigned int idx) const {
  return bool(*this & PT_AVL_mask(idx));
}
constexpr bool flags::pat() const noexcept {
  return bool(*this & PT_PAT);
}
constexpr bool flags::nx() const noexcept {
  return bool(*this & PT_NX);
}

constexpr auto flags::apply(const permission& perm, bool leaf)
    const noexcept -> flags {
  /* Convenience define,
   * to avoid having to write x86_shared::flags() all the time. */
  constexpr x86_shared::flags nil{};

  /* NX bit: set if this is the leaf and perm.exec == false.
   *       : clear if perm.exec == true. */
  auto set_nx = (leaf && !perm.exec ? PT_NX : nil);
  auto clear_nx = (perm.exec ? PT_NX : nil);

  /* WR bit: set if perm.write == true.
   *       : clear if this is the leaf and perm.write == false. */
  auto set_wr = (perm.write ? PT_RW : nil);
  auto clear_wr = (leaf && !perm.write ? PT_RW : nil);

  /* PWT bit: set if this is the leaf and perm.no_cache_write == true.
   *        : clear if perm.no_cache_write == false. */
  auto set_pwt = (leaf && perm.no_cache_write ? PT_PWT : nil);
  auto clear_pwt = (!perm.no_cache_write ? PT_PWT : nil);

  /* PCD bit: set if this is the leaf and perm.no_cache_read == true.
   *        : clear if perm.no_cache_read == false. */
  auto set_pcd = (leaf && perm.no_cache_read ? PT_PCD : nil);
  auto clear_pcd = (!perm.no_cache_read ? PT_PCD : nil);

  /* G bit: set if this is the leaf and perm.global == true.
   *      : clear if perm.global == false. */
  auto set_g = (leaf && perm.global ? PT_G : nil);
  auto clear_g = (!perm.global ? PT_G : nil);

  /* All bits that need to be cleared. */
  auto clear = clear_nx | clear_wr | clear_pwt | clear_pcd | clear_g;
  /* All bits that need to be set. */
  auto set = set_nx | set_wr | set_pwt | set_pcd | set_g;

  /* Calculate final result on flags. */
  return (flags() & ~clear) | set;
}

constexpr auto flags::get_permission() const noexcept -> permission {
  /* Convenience define,
   * to avoid having to write permission() all the time. */
  constexpr permission nil{};

  return permission::READ() |
         (rw() ? permission::WRITE() : nil) |
         (!nx() ? permission::EXEC() : nil) |
         (g() ? permission::GLOBAL() : nil) |
         (pcd() ? permission::NO_CACHE_READ() : nil) |
         (pwt() ? permission::NO_CACHE_WRITE() : nil);
}


constexpr auto pdpe_record::create(page_no_proxy pg, x86_shared::flags fl) ->
    pdpe_record {
  if ((pg.pgno_ & (PAGE_MASK >> PAGE_SHIFT)) != pg.pgno_)
    throw std::out_of_range("page number out of range");
  return pdpe_record{ (pg.pgno_ << PAGE_SHIFT) |
                      (fl & FLAGS_MASK).get() | PT_P };
}
constexpr auto pdpe_record::create(std::nullptr_t, x86_shared::flags fl)
    noexcept -> pdpe_record {
  return pdpe_record{ (fl & FLAGS_MASK).get() };
}

constexpr auto pdpe_record::address() const noexcept -> page_no_proxy {
  return page_no_proxy((v_ & PAGE_MASK) >> PAGE_SHIFT);
}
constexpr auto pdpe_record::flags() const noexcept -> x86_shared::flags {
  return x86_shared::flags(v_) & FLAGS_MASK;
}

constexpr auto pdpe_record::valid() const noexcept -> bool {
  return *this == (p() ?
                   create(address(), flags()) :
                   create(nullptr, flags()));
}

constexpr auto pdpe_record::combine(const permission& perm) const noexcept ->
    pdpe_record {
  return (p() ? create(address(), flags().apply(perm, false)) :
                create(nullptr, flags().apply(perm, false)));
}

constexpr auto pdpe_record::get_permission() const noexcept -> permission {
  return permission();
}


constexpr auto pdp_record::create(page_no_proxy pg, x86_shared::flags fl,
                                  bool leaf) -> pdp_record {
  auto PAGE_MASK = (leaf ? PAGE_MASK_PS : PAGE_MASK_NPS);
  if ((pg.pgno_ & (PAGE_MASK_NPS >> PAGE_SHIFT)) != pg.pgno_)
    throw std::out_of_range("page number out of range");
  if ((pg.pgno_ & (PAGE_MASK >> PAGE_SHIFT)) != pg.pgno_)
    throw std::out_of_range("page number insufficiently aligned");

  /* PT_PAT flag requires special handling. */
  auto FLAGS_MASK = (leaf ?
                     FLAGS_MASK_PS & ~PT_PAT :
                     FLAGS_MASK_NPS);
  auto pt_pat = (leaf && fl.pat() ? PT_PAT_ : 0U);

  return pdp_record{ (pg.pgno_ << PAGE_SHIFT) |
                     (fl & FLAGS_MASK).get() |
                     pt_pat | PT_P | (leaf ? PT_PS : 0U) };
}
constexpr auto pdp_record::create(std::nullptr_t, x86_shared::flags fl,
                                  bool leaf) noexcept -> pdp_record {
  /* PT_PAT flag requires special handling. */
  auto FLAGS_MASK = (leaf ?
                     FLAGS_MASK_PS & ~PT_PAT :
                     FLAGS_MASK_NPS);
  auto pt_pat = (leaf && fl.pat() ? PT_PAT_ : 0U);

  return pdp_record{ (fl & FLAGS_MASK).get() |
                     pt_pat | (leaf ? PT_PS : 0U) };
}

constexpr auto pdp_record::address() const noexcept -> page_no_proxy {
  auto sel = (ps() ? PAGE_MASK_PS : PAGE_MASK_NPS);
  return page_no_proxy((v_ & sel) >> PAGE_SHIFT);
}
constexpr auto pdp_record::flags() const noexcept -> x86_shared::flags {
  auto pt_pat = (ps() && (v_ & PT_PAT_) ?
                 PT_PAT :
                 x86_shared::flags());
  auto sel = (ps() ? FLAGS_MASK_PS & ~PT_PAT : FLAGS_MASK_NPS);
  return pt_pat | (x86_shared::flags(v_) & sel);
}

constexpr auto pdp_record::valid() const noexcept -> bool {
  return *this == (p() ?
                   create(address(), flags(), ps()) :
                   create(nullptr, flags(), ps()));
}

constexpr auto pdp_record::combine(const permission& perm) const noexcept ->
    pdp_record {
  /* Present bit: clear if this is the leaf and
   * !perm.read && !perm.write && !perm.exec. */
  return (!p() || (ps() && !perm.read && !perm.write && !perm.exec) ?
          create(nullptr, flags().apply(perm, ps())) :
          create(address(), flags().apply(perm, ps())));
}

constexpr auto pdp_record::get_permission() const noexcept -> permission {
  return (p() && ps() ? flags().get_permission() : permission());
}


constexpr auto pte_record::create(page_no_proxy pg, x86_shared::flags fl) ->
    pte_record {
  if ((pg.pgno_ & (PAGE_MASK >> PAGE_SHIFT)) != pg.pgno_)
    throw std::out_of_range("page number out of range");
  return pte_record{ (pg.pgno_ << PAGE_SHIFT) |
                     (fl & FLAGS_MASK).get() | PT_P };
}
constexpr auto pte_record::create(std::nullptr_t, x86_shared::flags fl)
    noexcept -> pte_record {
  return pte_record{ (fl & FLAGS_MASK).get() };
}

constexpr auto pte_record::address() const noexcept -> page_no_proxy {
  return page_no_proxy((v_ & PAGE_MASK) >> PAGE_SHIFT);
}
constexpr auto pte_record::flags() const noexcept -> x86_shared::flags {
  return x86_shared::flags(v_) & FLAGS_MASK;
}

constexpr auto pte_record::valid() const noexcept -> bool {
  return *this == (p() ?
                   create(address(), flags()) :
                   create(nullptr, flags()));
}

constexpr auto pte_record::combine(const permission& perm) const noexcept ->
    pte_record {
  /* Present bit: clear if !perm.read && !perm.write && !perm.exec. */
  return (!p() || (!perm.read && !perm.write && !perm.exec) ?
          create(nullptr, flags().apply(perm, true)) :
          create(address(), flags().apply(perm, true)));
}

constexpr auto pte_record::get_permission() const noexcept -> permission {
  return (p() ? flags().get_permission() : permission());
}


constexpr bool operator==(pdpe_record x, pdpe_record y) noexcept {
  return x.v_ == y.v_;
}
constexpr bool operator!=(pdpe_record x, pdpe_record y) noexcept {
  return x.v_ != y.v_;
}
constexpr bool operator==(pdp_record x, pdp_record y) noexcept {
  return x.v_ == y.v_;
}
constexpr bool operator!=(pdp_record x, pdp_record y) noexcept {
  return x.v_ != y.v_;
}
constexpr bool operator==(pte_record x, pte_record y) noexcept {
  return x.v_ == y.v_;
}
constexpr bool operator!=(pte_record x, pte_record y) noexcept {
  return x.v_ != y.v_;
}


}}} /* namespace ilias::pmap::x86_shared */

#endif /* _ILIAS_PMAP_X86_SHARED_INL_H_ */
