#ifndef _ILIAS_PMAP_X86_SHARED_H_
#define _ILIAS_PMAP_X86_SHARED_H_

#include <cstdint>
#include <type_traits>

namespace ilias {
namespace pmap {
namespace x86_shared {


struct pdpe_record;
struct pdp_record;
struct pte_record;


struct pdpe_record {
  uint64_t v_;

  /* Read/set the address. */
  auto page_no() const noexcept -> uint64_t;
  auto page_no(uint64_t) -> void;

  /* P (presence) bit. */
  auto p() const noexcept -> bool;
  auto p(bool) noexcept -> void;

  /* PWT (Page Writethrough) bit. */
  auto pwt() const noexcept -> bool;
  auto pwt(bool) noexcept -> void;

  /* PCD (Page-level cache disable) bit. */
  auto pcd() const noexcept -> bool;
  auto pcd(bool) noexcept -> void;

  /* AVL (Available to software) bits. */
  static constexpr unsigned int AVL_COUNT = 3;
  auto avl(unsigned int) const noexcept -> bool;
  auto avl(bool, unsigned int) noexcept -> void;

  /* Check if the entry is valid. */
  auto valid() const noexcept -> bool;
  auto combine(const permission&) const noexcept -> pdpe_record;
};

static_assert(sizeof(pdpe_record) == 8,
              "PDPE entry has wrong size.");
static_assert(std::is_pod<pdpe_record>::value,
              "PDPE record must be a plain-old-data type.");


struct pdp_record {
  uint64_t v_;

  /* Read/set the address. */
  auto page_no() const noexcept -> uint64_t;
  auto page_no(uint64_t) -> void;

  /* P (presence) bit. */
  auto p() const noexcept -> bool;
  auto p(bool) noexcept -> void;

  /* R/W (read/write) bit. */
  auto rw() const noexcept -> bool;
  auto rw(bool) noexcept -> void;

  /* U/S (user/supervisor) bit. */
  auto us() const noexcept -> bool;
  auto us(bool) noexcept -> void;

  /* PWT (page writethrough) bit. */
  auto pwt() const noexcept -> bool;
  auto pwt(bool) noexcept -> void;

  /* PCD (Page-level cache disable) bit. */
  auto pcd() const noexcept -> bool;
  auto pcd(bool) noexcept -> void;

  /* A (accessed) bit. */
  auto a() const noexcept -> bool;
  auto a(bool) noexcept -> void;

  /* D (dirty) bit. */
  auto d() const noexcept -> bool;
  auto d(bool) noexcept -> void;

  /* PS (page selector) bit. */
  auto ps() const noexcept -> bool;
  auto ps(bool) noexcept -> void;

  /* G (global) bit. */
  auto g() const noexcept -> bool;
  auto g(bool) noexcept -> void;

  /* AVL (Available to software) bits. */
  static constexpr unsigned int AVL_COUNT = 3;
  auto avl(unsigned int) const noexcept -> bool;
  auto avl(bool, unsigned int) noexcept -> void;

  /* NX (no execute) bit. */
  auto nx() const noexcept -> bool;
  auto nx(bool) noexcept -> void;

  /* PAT (Page-Attribute Table) bit. */
  auto pat() const noexcept -> bool;
  auto pat(bool) noexcept -> void;

  /* Check if the entry is valid. */
  auto valid() const noexcept -> bool;

  /* Conversion. */
  static auto convert(const pte_record&) noexcept -> pdp_record;
  auto combine(const permission&) const noexcept -> pdp_record;
};

static_assert(sizeof(pdp_record) == 8,
              "PDP entry has wrong size.");
static_assert(std::is_pod<pdp_record>::value,
              "PDP record must be a plain-old-data type.");


struct pte_record {
  uint64_t v_;

  /* Read/set the address. */
  auto page_no() const noexcept -> uint64_t;
  auto page_no(uint64_t) -> void;

  /* P (presence) bit. */
  auto p() const noexcept -> bool;
  auto p(bool) noexcept -> void;

  /* R/W (read/write) bit. */
  auto rw() const noexcept -> bool;
  auto rw(bool) noexcept -> void;

  /* U/S (user/supervisor) bit. */
  auto us() const noexcept -> bool;
  auto us(bool) noexcept -> void;

  /* PWT (page writethrough) bit. */
  auto pwt() const noexcept -> bool;
  auto pwt(bool) noexcept -> void;

  /* PCD (Page-level cache disable) bit. */
  auto pcd() const noexcept -> bool;
  auto pcd(bool) noexcept -> void;

  /* A (accessed) bit. */
  auto a() const noexcept -> bool;
  auto a(bool) noexcept -> void;

  /* D (dirty) bit. */
  auto d() const noexcept -> bool;
  auto d(bool) noexcept -> void;

  /* PAT (Page-Attribute Table) bit. */
  auto pat() const noexcept -> bool;
  auto pat(bool) noexcept -> void;

  /* G (global) bit. */
  auto g() const noexcept -> bool;
  auto g(bool) noexcept -> void;

  /* AVL (Available to software) bits. */
  static constexpr unsigned int AVL_COUNT = 3;
  auto avl(unsigned int) const noexcept -> bool;
  auto avl(bool, unsigned int) noexcept -> void;

  /* NX (no execute) bit. */
  auto nx() const noexcept -> bool;
  auto nx(bool) noexcept -> void;

  /* Check if the entry is valid. */
  auto valid() const noexcept -> bool;

  /* Conversion. */
  static auto convert(const pdp_record&) noexcept -> pte_record;
  auto combine(const permission&) const noexcept -> pte_record;
};

static_assert(sizeof(pte_record) == 8,
              "PTE entry has wrong size.");
static_assert(std::is_pod<pte_record>::value,
              "PTE record must be a plain-old-data type.");


}}} /* namespace ilias::pmap::x86_shared */

#include <ilias/pmap/x86_shared-inl.h>

#endif /* _ILIAS_PMAP_X86_SHARED_H_ */
