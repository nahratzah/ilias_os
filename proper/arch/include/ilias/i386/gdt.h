#ifndef _ILIAS_ARCH_I386_GDT_H_
#define _ILIAS_ARCH_I386_GDT_H_

#include <cstdint>
#include <array>

namespace ilias {
namespace i386 {


/*
 * GDT flags.
 */
constexpr uint16_t seg_type_system = 1 << 0x4;

/* Segment privilege level. */
constexpr uint16_t seg_priv(uint16_t lvl) {
  return (lvl & 0x3) << 0x5;
}

constexpr uint16_t seg_present     = 1 << 0x7;
constexpr uint16_t seg_sys_avl     = 1 << 0xc;
constexpr uint16_t seg_longmode    = 1 << 0xd;
constexpr uint16_t seg_32bit_size  = 1 << 0xe;
constexpr uint16_t seg_page_gran   = 1 << 0xf;


/*
 * GDT code/data segment bits.
 */
constexpr uint8_t seg_cd_accessed  = 0x01;
constexpr uint8_t seg_d_write      = 0x02;
constexpr uint8_t seg_d_exp_down   = 0x04;
constexpr uint8_t seg_cd_code      = 0x08;  // Indicates code segment.
constexpr uint8_t seg_c_read       = 0x02;
constexpr uint8_t seg_c_conforming = 0x04;


/* Code/data flags for PL0, PL3. */
constexpr uint16_t gdt_code_flags(uint16_t lvl) noexcept {
  return seg_type_system | seg_present |
         seg_32bit_size | seg_page_gran |
         seg_priv(lvl) | (seg_cd_code | seg_c_read);
}
constexpr uint16_t gdt_data_flags(uint16_t lvl) noexcept {
  return seg_type_system | seg_present |
         seg_32bit_size | seg_page_gran |
         seg_priv(lvl) | (seg_d_write);
}


/* Create a single descriptor entry. */
constexpr uint64_t gdt_descriptor(uint32_t base, uint32_t lim, uint16_t fl) {
  return uint64_t(lim          & 0x000f0000) << 32 |
         uint64_t((fl << 8)    & 0x00f0ff00) << 32 |
         uint64_t((base >> 16) & 0x000000ff) << 32 |
         uint64_t(base         & 0xff000000) << 32 |
                 ((base << 16) & 0xffff0000) |
                 (lim          & 0x0000ffff);
}


/*
 * Descriptor indices.
 */
enum class gdt_idx : uint16_t {
  nil = 0,  // Null selector (keeps emulators happy).
  kernel_code = 4,
  kernel_data,
  kernel_tls,
  tss,
  uspace_code = 8,
  uspace_data,
  uspace_tls,
  uspace_wine_tls,
  ldt,
  _count  // Must be last entry in the list!
};

using gdt_entries_t = std::array<uint64_t, uint16_t(gdt_idx::_count)>;

struct __attribute__((packed)) gdt_descriptor_t {
  uint16_t size;
  uint32_t offset;
};

struct alignas(64) gdt_t {
  gdt_entries_t gdt;
  const gdt_descriptor_t desc;

  const void* get_gdt_ptr() const noexcept { return &desc; }
};

extern gdt_t gdt;


}} /* namespace ilias::i386 */

#endif /* _ILIAS_ARCH_I386_GDT_H_ */
