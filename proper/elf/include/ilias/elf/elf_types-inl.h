#ifndef _ILIAS_ELF_ELF_TYPES_INL_H_
#define _ILIAS_ELF_ELF_TYPES_INL_H_

#include "elf_types.h"

namespace ilias {
namespace elf {


inline auto elf32_addr::from_elf_addr(uint32_t addr) noexcept -> elf32_addr {
  elf32_addr rv;
  rv.addr_ = addr;
  return rv;
}

inline auto elf64_addr::from_elf_addr(uint64_t addr) noexcept -> elf64_addr {
  elf64_addr rv;
  rv.addr_ = addr;
  return rv;
}


inline auto elf32_off::from_elf_off(uint32_t off) noexcept -> elf32_off {
  elf32_off rv;
  rv.off_ = off;
  return rv;
}

inline auto elf64_off::from_elf_off(uint64_t off) noexcept -> elf64_off {
  elf64_off rv;
  rv.off_ = off;
  return rv;
}


inline auto elf_hdr_flags::from_elf_flags(uint32_t fl) noexcept ->
    elf_hdr_flags {
  elf_hdr_flags rv;
  rv.fl_ = fl;
  return rv;
}


}} /* namespace ilias::elf */

#endif /* _ILIAS_ELF_ELF_TYPES_INL_H_ */
