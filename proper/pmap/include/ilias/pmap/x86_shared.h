#ifndef _ILIAS_PMAP_X86_SHARED_H_
#define _ILIAS_PMAP_X86_SHARED_H_

#include <cstdint>
#include <type_traits>
#include <ilias/pmap/pmap.h>
#include <ilias/pmap/page.h>

namespace ilias {
namespace pmap {
namespace x86_shared {


class flags;
struct pdpe_record;
struct pdp_record;
struct pte_record;


class page_no_proxy {
  friend pdpe_record;
  friend pdp_record;
  friend pte_record;

 private:
  constexpr page_no_proxy(uint64_t pg) noexcept : pgno_(pg) {}

 public:
  constexpr page_no_proxy(const page_no_proxy&) noexcept = default;
  page_no_proxy& operator=(const page_no_proxy&) noexcept = default;
  constexpr page_no_proxy(page_no<arch::i386>) noexcept;
  constexpr page_no_proxy(page_no<arch::amd64>) noexcept;

  constexpr operator page_no<arch::i386>() const noexcept;
  constexpr operator page_no<arch::amd64>() const noexcept;

  constexpr bool operator==(const page_no_proxy&) const noexcept;
  constexpr bool operator!=(const page_no_proxy&) const noexcept;
  constexpr bool operator<(const page_no_proxy&) const noexcept;
  constexpr bool operator<=(const page_no_proxy&) const noexcept;
  constexpr bool operator>(const page_no_proxy&) const noexcept;
  constexpr bool operator>=(const page_no_proxy&) const noexcept;

 private:
  uint64_t pgno_;
};


/* Page table flags. */
class flags {
 public:
  static constexpr unsigned int AVL_COUNT = 3;

  explicit constexpr flags(uint64_t fl) noexcept : fl_(fl) {}
  constexpr flags() noexcept = default;
  constexpr flags(const flags&) noexcept = default;
  flags& operator=(const flags&) noexcept = default;

  constexpr uint64_t get() const noexcept;
  constexpr explicit operator bool() const noexcept { return get() != 0; }

  friend constexpr flags operator&(flags x, flags y) noexcept
  { return flags(x.fl_ & y.fl_); }
  friend constexpr flags operator|(flags x, flags y) noexcept
  { return flags(x.fl_ | y.fl_); }
  friend constexpr flags operator^(flags x, flags y) noexcept
  { return flags(x.fl_ ^ y.fl_); }
  friend constexpr flags operator~(flags x) noexcept
  { return flags(~x.fl_); }

  friend constexpr flags PT_AVL_mask(unsigned int idx);

  constexpr bool operator==(const flags& o) const noexcept
  { return fl_ == o.fl_; }
  constexpr bool operator!=(const flags& o) const noexcept
  { return fl_ != o.fl_; }

  constexpr bool rw() const noexcept;
  constexpr bool us() const noexcept;
  constexpr bool pwt() const noexcept;
  constexpr bool pcd() const noexcept;
  constexpr bool a() const noexcept;
  constexpr bool d() const noexcept;
  constexpr bool g() const noexcept;
  constexpr bool avl(unsigned int) const;
  constexpr bool pat() const noexcept;
  constexpr bool nx() const noexcept;

  constexpr flags apply(const permission&, bool leaf) const noexcept;
  constexpr permission get_permission() const noexcept;

 private:
  uint64_t fl_ = 0;
};

/*
 * Page Table flags.
 *
 * Note that PT_P (present, bit 1) and PT_PS (page select, bit 7) are omitted:
 * they are handled in the pdp_record and pte_record.
 */
constexpr flags PT_RW   = flags(uint64_t(1) << 1);
constexpr flags PT_US   = flags(uint64_t(1) << 2);
constexpr flags PT_PWT  = flags(uint64_t(1) << 3);
constexpr flags PT_PCD  = flags(uint64_t(1) << 4);
constexpr flags PT_A    = flags(uint64_t(1) << 5);
constexpr flags PT_D    = flags(uint64_t(1) << 6);
constexpr flags PT_PAT  = flags(uint64_t(1) << 7);  // 7 in PTE, 12 in others
constexpr flags PT_G    = flags(uint64_t(1) << 8);
constexpr flags PT_AVL0 = flags(uint64_t(1) << 9);
constexpr flags PT_AVL  = flags(((uint64_t(1) << flags::AVL_COUNT) - 1U) << 4);
constexpr flags PT_NX   = flags(uint64_t(1) << 63);
constexpr flags PT_ALL_FLAGS = PT_RW | PT_US | PT_PWT | PT_PCD | PT_A | PT_D |
                               PT_PAT | PT_G | PT_AVL | PT_NX;


struct pdpe_record {
  uint64_t v_;

  static constexpr x86_shared::flags FLAGS_MASK = PT_PWT | PT_PCD | PT_AVL;
  static constexpr unsigned int PAGE_SHIFT = 12;
  static constexpr uint64_t PAGE_MASK =
      (uint64_t(1) << 63) - (uint64_t(1) << 12);
  static constexpr uint64_t PT_P = uint64_t(1) << 0;

  static constexpr auto create(page_no_proxy,
                               x86_shared::flags = x86_shared::flags{ 0 }) ->
      pdpe_record;
  static constexpr auto create(std::nullptr_t,
                               x86_shared::flags = x86_shared::flags{ 0 })
      noexcept -> pdpe_record;

  constexpr page_no_proxy address() const noexcept;
  constexpr x86_shared::flags flags() const noexcept;

  constexpr bool p() const noexcept { return v_ & PT_P; }
  explicit constexpr operator bool() const noexcept { return p(); }
  constexpr bool valid() const noexcept;

  constexpr auto combine(const permission&) const noexcept ->
      pdpe_record;
  constexpr auto get_permission() const noexcept -> permission;
};

struct pdp_record {
  uint64_t v_;

  static constexpr x86_shared::flags FLAGS_MASK_NPS =
      PT_RW | PT_US | PT_PWT | PT_PCD | PT_A | PT_AVL | PT_NX;
  static constexpr x86_shared::flags FLAGS_MASK_PS =
      PT_RW | PT_US | PT_PWT | PT_PCD | PT_A | PT_D | PT_G | PT_AVL |
      PT_PAT | PT_NX;
  static constexpr unsigned int PAGE_SHIFT = 12;
  static constexpr uint64_t PAGE_MASK_NPS =
      (uint64_t(1) << 63) - (uint64_t(1) << 12);
  static constexpr uint64_t PAGE_MASK_PS =
      (uint64_t(1) << 63) - (uint64_t(1) << 21);
  static constexpr uint64_t PT_P = uint64_t(1) << 0;
  static constexpr uint64_t PT_PS = uint64_t(1) << 7;
  static constexpr uint64_t PT_PAT_ = uint64_t(1) << 12;  // Special case.

  static constexpr auto create(page_no_proxy,
                               x86_shared::flags = x86_shared::flags{ 0 },
                               bool = false) ->
      pdp_record;
  static constexpr auto create(std::nullptr_t,
                               x86_shared::flags = x86_shared::flags{ 0 },
                               bool = false) noexcept ->
      pdp_record;

  constexpr page_no_proxy address() const noexcept;
  constexpr x86_shared::flags flags() const noexcept;

  constexpr bool p() const noexcept { return v_ & PT_P; }
  explicit constexpr operator bool() const noexcept { return p(); }
  constexpr bool ps() const noexcept { return v_ & PT_PS; }
  constexpr bool valid() const noexcept;

  constexpr auto combine(const permission&) const noexcept ->
      pdp_record;
  constexpr auto get_permission() const noexcept -> permission;
};

struct pte_record {
  uint64_t v_;

  static constexpr x86_shared::flags FLAGS_MASK =
      PT_RW | PT_US | PT_PWT | PT_PCD | PT_A | PT_D | PT_G | PT_AVL |
      PT_PAT | PT_NX;
  static constexpr unsigned int PAGE_SHIFT = 12;
  static constexpr uint64_t PAGE_MASK =
      (uint64_t(1) << 63) - (uint64_t(1) << 12);
  static constexpr uint64_t PT_P = uint64_t(1) << 0;

  static constexpr auto create(page_no_proxy,
                               x86_shared::flags = x86_shared::flags{ 0 }) ->
      pte_record;
  static constexpr auto create(std::nullptr_t,
                               x86_shared::flags = x86_shared::flags{ 0 })
      noexcept -> pte_record;

  constexpr page_no_proxy address() const noexcept;
  constexpr x86_shared::flags flags() const noexcept;

  constexpr bool p() const noexcept { return v_ & PT_P; }
  explicit constexpr operator bool() const noexcept { return p(); }
  constexpr bool valid() const noexcept;

  constexpr auto combine(const permission&) const noexcept ->
      pte_record;
  constexpr auto get_permission() const noexcept -> permission;

  static_assert(PT_PAT == x86_shared::flags(uint64_t(1) << 7),
                "This code assumes no translation of PAT bit is required.");
};


constexpr bool operator==(pdpe_record, pdpe_record) noexcept;
constexpr bool operator!=(pdpe_record, pdpe_record) noexcept;
constexpr bool operator==(pdp_record, pdp_record) noexcept;
constexpr bool operator!=(pdp_record, pdp_record) noexcept;
constexpr bool operator==(pte_record, pte_record) noexcept;
constexpr bool operator!=(pte_record, pte_record) noexcept;


/* Validate that record are ABI compliant. */
static_assert(sizeof(pdpe_record) == 8,
              "PDPE record must be 8 bytes");
static_assert(sizeof(pdp_record) == 8,
              "PDP record must be 8 bytes");
static_assert(sizeof(pte_record) == 8,
              "PTE record must be 8 bytes");
static_assert(std::is_pod<pdpe_record>::value,
              "PDPE record must be a plain-old-data type.");
static_assert(std::is_pod<pdp_record>::value,
              "PDP record must be a plain-old-data type.");
static_assert(std::is_pod<pte_record>::value,
              "PTE record must be a plain-old-data type.");


}}} /* namespace ilias::pmap::x86_shared */

#include <ilias/pmap/x86_shared-inl.h>

#endif /* _ILIAS_PMAP_X86_SHARED_H_ */
