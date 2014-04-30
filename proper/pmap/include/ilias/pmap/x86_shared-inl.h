#ifndef _ILIAS_PMAP_X86_SHARED_INL_H_
#define _ILIAS_PMAP_X86_SHARED_INL_H_

#include <ilias/pmap/x86_shared.h>
#include <cassert>
#include <cstdint>
#include <stdexcept>

namespace ilias {
namespace pmap {
namespace x86_shared {


inline auto pdpe_record::page_no() const noexcept -> uint64_t {
  constexpr auto pg_shift = page_shift(arch::i386);
  constexpr uint64_t entry_mask = 0x8000000000000000ULL - (1ULL << pg_shift);
  return (v_ & entry_mask) >> pg_shift;
}
inline auto pdpe_record::page_no(uint64_t v) -> void {
  constexpr auto pg_shift = page_shift(arch::i386);
  constexpr uint64_t entry_mask = 0x8000000000000000ULL - (1ULL << pg_shift);

  if (_predict_false((v & (entry_mask >> pg_shift)) != v))
    throw std::invalid_argument("Page number too large.");
  v_ &= ~entry_mask;
  v_ |= (v << pg_shift);
}

inline auto pdpe_record::p() const noexcept -> bool {
  constexpr uint64_t mask = (1ULL << 0);
  return (v_ & mask);
}
inline auto pdpe_record::p(bool set) noexcept -> void {
  constexpr uint64_t mask = (1ULL << 0);
  if (set)
    v_ |= mask;
  else
    v_ &= ~mask;
}

inline auto pdpe_record::pwt() const noexcept -> bool {
  constexpr uint64_t mask = (1ULL << 3);
  return (v_ & mask);
}
inline auto pdpe_record::pwt(bool set) noexcept -> void {
  constexpr uint64_t mask = (1ULL << 3);
  if (set)
    v_ |= mask;
  else
    v_ &= ~mask;
}

inline auto pdpe_record::pcd() const noexcept -> bool {
  constexpr uint64_t mask = (1ULL << 4);
  return (v_ & mask);
}
inline auto pdpe_record::pcd(bool set) noexcept -> void {
  constexpr uint64_t mask = (1ULL << 4);
  if (set)
    v_ |= mask;
  else
    v_ &= ~mask;
}

inline auto pdpe_record::avl(unsigned int idx) const noexcept -> bool {
  assert(idx < AVL_COUNT);
  constexpr uint64_t mask = (1ULL << 9);
  return (v_ & (mask << idx));
}
inline auto pdpe_record::avl(bool set, unsigned int idx) noexcept -> void {
  assert(idx < AVL_COUNT);
  constexpr uint64_t mask = (1ULL << 9);
  if (set)
    v_ |= (mask << idx);
  else
    v_ &= ~(mask << idx);
}

inline auto pdpe_record::valid() const noexcept -> bool {
  constexpr auto pg_shift = page_shift(arch::i386);
  constexpr uint64_t entry_mask = 0x8000000000000000ULL - (1ULL << pg_shift);
  constexpr uint64_t flag_mask = (1ULL << 0 |
                                  1ULL << 3 |
                                  1ULL << 4 |
                                  ((1ULL << AVL_COUNT) - 1U) << 9);
  constexpr uint64_t mask = entry_mask | flag_mask;

  return (v_ & mask) == v_;
}


inline auto pdp_record::page_no() const noexcept -> uint64_t {
  constexpr auto pg_shift = page_shift(arch::i386);
  uint64_t entry_mask = 0x8000000000000000ULL - (1ULL << pg_shift);
  if (ps()) entry_mask &= ~uint64_t(1ULL << 12);  // Mask PAT bit.

  return (v_ & entry_mask) >> pg_shift;
}
inline auto pdp_record::page_no(uint64_t v) -> void {
  constexpr auto pg_shift = page_shift(arch::i386);
  uint64_t entry_mask = 0x8000000000000000ULL - (1ULL << pg_shift);
  if (ps()) entry_mask &= ~uint64_t(1ULL << 12);  // Mask PAT bit.

  if (_predict_false((v & (entry_mask >> pg_shift)) != v))
    throw std::invalid_argument("Page number too large.");
  v_ &= ~entry_mask;
  v_ |= (v << pg_shift);
}

inline auto pdp_record::p() const noexcept -> bool {
  constexpr uint64_t mask = (1ULL << 0);
  return (v_ & mask);
}
inline auto pdp_record::p(bool set) noexcept -> void {
  constexpr uint64_t mask = (1ULL << 0);
  if (set)
    v_ |= mask;
  else
    v_ &= ~mask;
}

inline auto pdp_record::rw() const noexcept -> bool {
  constexpr uint64_t mask = (1ULL << 1);
  return (v_ & mask);
}
inline auto pdp_record::rw(bool set) noexcept -> void {
  constexpr uint64_t mask = (1ULL << 1);
  if (set)
    v_ |= mask;
  else
    v_ &= ~mask;
}

inline auto pdp_record::us() const noexcept -> bool {
  constexpr uint64_t mask = (1ULL << 2);
  return (v_ & mask);
}
inline auto pdp_record::us(bool set) noexcept -> void {
  constexpr uint64_t mask = (1ULL << 2);
  if (set)
    v_ |= mask;
  else
    v_ &= ~mask;
}

inline auto pdp_record::pwt() const noexcept -> bool {
  constexpr uint64_t mask = (1ULL << 3);
  return (v_ & mask);
}
inline auto pdp_record::pwt(bool set) noexcept -> void {
  constexpr uint64_t mask = (1ULL << 3);
  if (set)
    v_ |= mask;
  else
    v_ &= ~mask;
}

inline auto pdp_record::pcd() const noexcept -> bool {
  constexpr uint64_t mask = (1ULL << 4);
  return (v_ & mask);
}
inline auto pdp_record::pcd(bool set) noexcept -> void {
  constexpr uint64_t mask = (1ULL << 4);
  if (set)
    v_ |= mask;
  else
    v_ &= ~mask;
}

inline auto pdp_record::a() const noexcept -> bool {
  constexpr uint64_t mask = (1ULL << 5);
  return (v_ & mask);
}
inline auto pdp_record::a(bool set) noexcept -> void {
  constexpr uint64_t mask = (1ULL << 5);
  if (set)
    v_ |= mask;
  else
    v_ &= ~mask;
}

inline auto pdp_record::d() const noexcept -> bool {
  constexpr uint64_t mask = (1ULL << 6);
  return ps() && (v_ & mask);
}
inline auto pdp_record::d(bool set) noexcept -> void {
  constexpr uint64_t mask = (1ULL << 6);
  if (set)
    v_ |= mask;
  else
    v_ &= ~mask;
}

inline auto pdp_record::ps() const noexcept -> bool {
  constexpr uint64_t mask = (1ULL << 7);
  return (v_ & mask);
}
inline auto pdp_record::ps(bool set) noexcept -> void {
  constexpr uint64_t mask = (1ULL << 7);
  if (set)
    v_ |= mask;
  else
    v_ &= ~mask;
}

inline auto pdp_record::g() const noexcept -> bool {
  constexpr uint64_t mask = (1ULL << 8);
  return ps() && (v_ & mask);
}
inline auto pdp_record::g(bool set) noexcept -> void {
  constexpr uint64_t mask = (1ULL << 8);
  if (set)
    v_ |= mask;
  else
    v_ &= ~mask;
}

inline auto pdp_record::avl(unsigned int idx) const noexcept -> bool {
  assert(idx < AVL_COUNT);
  constexpr uint64_t mask = (1ULL << 9);
  return (v_ & (mask << idx));
}
inline auto pdp_record::avl(bool set, unsigned int idx) noexcept -> void {
  assert(idx < AVL_COUNT);
  constexpr uint64_t mask = (1ULL << 9);
  if (set)
    v_ |= (mask << idx);
  else
    v_ &= ~(mask << idx);
}

inline auto pdp_record::nx() const noexcept -> bool {
  constexpr uint64_t mask = (1ULL << 63);
  return (v_ & mask);
}
inline auto pdp_record::nx(bool set) noexcept -> void {
  constexpr uint64_t mask = (1ULL << 63);
  if (set)
    v_ |= mask;
  else
    v_ &= ~mask;
}

inline auto pdp_record::pat() const noexcept -> bool {
  constexpr uint64_t mask = (1ULL << 12);
  return ps() && (v_ & mask);
}
inline auto pdp_record::pat(bool set) noexcept -> void {
  constexpr uint64_t mask = (1ULL << 12);
  if (set)
    v_ |= mask;
  else
    v_ &= ~mask;
}

inline auto pdp_record::valid() const noexcept -> bool {
  constexpr auto pg_shift = page_shift(arch::i386);
  constexpr uint64_t entry_mask = 0x8000000000000000ULL - (1ULL << pg_shift);
  constexpr uint64_t flag_mask = (1ULL << 0 |
                                  1ULL << 1 |
                                  1ULL << 2 |
                                  1ULL << 3 |
                                  1ULL << 4 |
                                  1ULL << 5 |
                                  1ULL << 6 |
                                  1ULL << 7 |
                                  1ULL << 8 |
                                  ((1ULL << AVL_COUNT) - 1U) << 9 |
                                  1ULL << 63);
  constexpr uint64_t mask = entry_mask | flag_mask;

  return (v_ & mask) == v_ &&
         (!ps() || (page_no() & ((0x1ULL << 9) - 1U)) == page_no());
}

inline auto pdp_record::convert(const pte_record& r) noexcept ->
    pdp_record {
  pdp_record rv{ 0 };
  rv.page_no(r.page_no());
  rv.p(r.p());
  rv.rw(r.rw());
  rv.us(r.us());
  rv.pwt(r.pwt());
  rv.pcd(r.pcd());
  rv.a(r.a());
  rv.d(r.d());
  rv.ps(true);
  rv.g(r.g());
  for (unsigned int i = 0; i < AVL_COUNT && i < pte_record::AVL_COUNT; ++i)
    rv.avl(r.avl(i), i);
  rv.nx(r.nx());
  rv.pat(r.pat());
  return rv;
}


inline auto pte_record::page_no() const noexcept -> uint64_t {
  constexpr auto pg_shift = page_shift(arch::i386);
  constexpr uint64_t entry_mask = 0x8000000000000000ULL - (1ULL << pg_shift);
  return (v_ & entry_mask) >> pg_shift;
}
inline auto pte_record::page_no(uint64_t v) -> void {
  constexpr auto pg_shift = page_shift(arch::i386);
  constexpr uint64_t entry_mask = 0x8000000000000000ULL - (1ULL << pg_shift);

  if (_predict_false((v & (entry_mask >> pg_shift)) != v))
    throw std::invalid_argument("Page number too large.");
  v_ &= ~entry_mask;
  v_ |= (v << pg_shift);
}

inline auto pte_record::p() const noexcept -> bool {
  constexpr uint64_t mask = (1ULL << 0);
  return (v_ & mask);
}
inline auto pte_record::p(bool set) noexcept -> void {
  constexpr uint64_t mask = (1ULL << 0);
  if (set)
    v_ |= mask;
  else
    v_ &= ~mask;
}

inline auto pte_record::rw() const noexcept -> bool {
  constexpr uint64_t mask = (1ULL << 1);
  return (v_ & mask);
}
inline auto pte_record::rw(bool set) noexcept -> void {
  constexpr uint64_t mask = (1ULL << 1);
  if (set)
    v_ |= mask;
  else
    v_ &= ~mask;
}

inline auto pte_record::us() const noexcept -> bool {
  constexpr uint64_t mask = (1ULL << 2);
  return (v_ & mask);
}
inline auto pte_record::us(bool set) noexcept -> void {
  constexpr uint64_t mask = (1ULL << 2);
  if (set)
    v_ |= mask;
  else
    v_ &= ~mask;
}

inline auto pte_record::pwt() const noexcept -> bool {
  constexpr uint64_t mask = (1ULL << 3);
  return (v_ & mask);
}
inline auto pte_record::pwt(bool set) noexcept -> void {
  constexpr uint64_t mask = (1ULL << 3);
  if (set)
    v_ |= mask;
  else
    v_ &= ~mask;
}

inline auto pte_record::pcd() const noexcept -> bool {
  constexpr uint64_t mask = (1ULL << 4);
  return (v_ & mask);
}
inline auto pte_record::pcd(bool set) noexcept -> void {
  constexpr uint64_t mask = (1ULL << 4);
  if (set)
    v_ |= mask;
  else
    v_ &= ~mask;
}

inline auto pte_record::a() const noexcept -> bool {
  constexpr uint64_t mask = (1ULL << 5);
  return (v_ & mask);
}
inline auto pte_record::a(bool set) noexcept -> void {
  constexpr uint64_t mask = (1ULL << 5);
  if (set)
    v_ |= mask;
  else
    v_ &= ~mask;
}

inline auto pte_record::d() const noexcept -> bool {
  constexpr uint64_t mask = (1ULL << 6);
  return (v_ & mask);
}
inline auto pte_record::d(bool set) noexcept -> void {
  constexpr uint64_t mask = (1ULL << 6);
  if (set)
    v_ |= mask;
  else
    v_ &= ~mask;
}

inline auto pte_record::pat() const noexcept -> bool {
  constexpr uint64_t mask = (1ULL << 7);
  return (v_ & mask);
}
inline auto pte_record::pat(bool set) noexcept -> void {
  constexpr uint64_t mask = (1ULL << 7);
  if (set)
    v_ |= mask;
  else
    v_ &= ~mask;
}

inline auto pte_record::g() const noexcept -> bool {
  constexpr uint64_t mask = (1ULL << 8);
  return (v_ & mask);
}
inline auto pte_record::g(bool set) noexcept -> void {
  constexpr uint64_t mask = (1ULL << 8);
  if (set)
    v_ |= mask;
  else
    v_ &= ~mask;
}

inline auto pte_record::avl(unsigned int idx) const noexcept -> bool {
  assert(idx < AVL_COUNT);
  constexpr uint64_t mask = (1ULL << 9);
  return (v_ & (mask << idx));
}
inline auto pte_record::avl(bool set, unsigned int idx) noexcept -> void {
  assert(idx < AVL_COUNT);
  constexpr uint64_t mask = (1ULL << 9);
  if (set)
    v_ |= (mask << idx);
  else
    v_ &= ~(mask << idx);
}

inline auto pte_record::nx() const noexcept -> bool {
  constexpr uint64_t mask = (1ULL << 63);
  return (v_ & mask);
}
inline auto pte_record::nx(bool set) noexcept -> void {
  constexpr uint64_t mask = (1ULL << 63);
  if (set)
    v_ |= mask;
  else
    v_ &= ~mask;
}

inline auto pte_record::valid() const noexcept -> bool {
  constexpr auto pg_shift = page_shift(arch::i386);
  constexpr uint64_t entry_mask = 0x8000000000000000ULL - (1ULL << pg_shift);
  constexpr uint64_t flag_mask = (1ULL << 0 |
                                  1ULL << 1 |
                                  1ULL << 2 |
                                  1ULL << 3 |
                                  1ULL << 4 |
                                  1ULL << 5 |
                                  1ULL << 6 |
                                  1ULL << 7 |
                                  1ULL << 8 |
                                  ((1ULL << AVL_COUNT) - 1U) << 9 |
                                  1ULL << 63);
  constexpr uint64_t mask = entry_mask | flag_mask;

  return (v_ & mask) == v_;
}

inline auto pte_record::convert(const pdp_record& r) noexcept ->
    pte_record {
  assert_msg(r.ps(), "Cannot convert non-page PDP to PTE.");

  pte_record rv{ 0 };
  rv.page_no(r.page_no());
  rv.p(r.p());
  rv.rw(r.rw());
  rv.us(r.us());
  rv.pwt(r.pwt());
  rv.pcd(r.pcd());
  rv.a(r.a());
  rv.d(r.d());
  rv.pat(r.pat());
  rv.g(r.g());
  for (unsigned int i = 0; i < AVL_COUNT && i < pdp_record::AVL_COUNT; ++i)
    rv.avl(r.avl(i), i);
  rv.nx(r.nx());
  return rv;
}


}}} /* namespace ilias::pmap::x86_shared */

#endif /* _ILIAS_PMAP_X86_SHARED_INL_H_ */
