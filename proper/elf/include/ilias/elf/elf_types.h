#ifndef _ILIAS_ELF_ELF_TYPES_H_
#define _ILIAS_ELF_ELF_TYPES_H_

#include <cstddef>
#include <cstdint>
#include <array>

namespace ilias {
namespace elf {

using namespace std;

namespace types {

using Elf32_Addr = uint32_t;
using Elf32_Half = uint16_t;
using Elf32_Off = uint32_t;
using Elf32_Sword = int32_t;
using Elf32_Word = uint32_t;
using Elf32_Byte = uint8_t;
using Elf32_Section = uint16_t;

using Elf64_Addr = uint64_t;
using Elf64_Half = uint16_t;
using Elf64_Off = uint64_t;
using Elf64_Sword = uint32_t;
using Elf64_Word = uint32_t;
using Elf64_Sxword = uint64_t;
using Elf64_Xword = uint64_t;
using Elf64_Byte = uint8_t;
using Elf64_Section = uint16_t;

/* Indices in Elf32_Ehdr.e_ident */
constexpr size_t EI_MAG0    =  0;
constexpr size_t EI_MAG1    =  1;
constexpr size_t EI_MAG2    =  2;
constexpr size_t EI_MAG3    =  3;
constexpr size_t EI_CLASS   =  4;
constexpr size_t EI_DATA    =  5;
constexpr size_t EI_VERSION =  6;
constexpr size_t EI_PAD     =  7;
constexpr size_t EI_NIDENT  = 16;

struct Elf32_Ehdr {
  static_assert(sizeof(array<Elf32_Byte, EI_NIDENT>) == EI_NIDENT,
                "std::array is broken");

  array<Elf32_Byte, EI_NIDENT> e_ident;
  Elf32_Half e_type;
  Elf32_Half e_machine;
  Elf32_Word e_version;
  Elf32_Addr e_entry;
  Elf32_Off  e_phoff;
  Elf32_Off  e_shoff;
  Elf32_Word e_flags;
  Elf32_Half e_ehsize;
  Elf32_Half e_phentsize;
  Elf32_Half e_phnum;
  Elf32_Half e_shentsize;
  Elf32_Half e_shnum;
  Elf32_Half e_shstrndx;
};

struct Elf64_Ehdr {
  static_assert(sizeof(array<Elf64_Byte, EI_NIDENT>) == EI_NIDENT,
                "std::array is broken");

  array<Elf64_Byte, EI_NIDENT> e_ident;
  Elf64_Half e_type;
  Elf64_Half e_machine;
  Elf64_Word e_version;
  Elf64_Addr e_entry;
  Elf64_Off  e_phoff;
  Elf64_Off  e_shoff;
  Elf64_Word e_flags;
  Elf64_Half e_ehsize;
  Elf64_Half e_phentsize;
  Elf64_Half e_phnum;
  Elf64_Half e_shentsize;
  Elf64_Half e_shnum;
  Elf64_Half e_shstrndx;
};

/* Special section indices */
constexpr Elf32_Half SHN_UNDEF = 0;
constexpr Elf32_Half SHN_LORESERVE = 0xff00;
constexpr Elf32_Half SHN_LOPROC = 0xff00;
constexpr Elf32_Half SHN_HIPROC = 0xff1f;
constexpr Elf32_Half SHN_ABS = 0xfff1;
constexpr Elf32_Half SHN_COMMON = 0xfff2;
constexpr Elf32_Half SHN_HIRESERVE = 0xffff;

/* ELF magic values */
constexpr array<unsigned char, 4> ELFMAG{{
    static_cast<unsigned char>(0x7fU),
    static_cast<unsigned char>('E'),
    static_cast<unsigned char>('L'),
    static_cast<unsigned char>('F')
}};

struct Elf32_Shdr {
  Elf32_Word  sh_name;
  Elf32_Word  sh_type;
  Elf32_Word  sh_flags;
  Elf32_Addr  sh_addr;
  Elf32_Off   sh_offset;
  Elf32_Word  sh_size;
  Elf32_Word  sh_link;
  Elf32_Word  sh_info;
  Elf32_Word  sh_addralign;
  Elf32_Word  sh_entsize;
};

struct Elf64_Shdr {
  Elf64_Word  sh_name;
  Elf64_Word  sh_type;
  Elf64_Xword sh_flags;
  Elf64_Addr  sh_addr;
  Elf64_Off   sh_offset;
  Elf64_Xword sh_size;
  Elf64_Word  sh_link;
  Elf64_Word  sh_info;
  Elf64_Xword sh_addralign;
  Elf64_Xword sh_entsize;
};

} /* namespace ilias::elf::types */


class elf32_addr {  // XXX
 public:
  static elf32_addr from_elf_addr(uint32_t) noexcept;

  uint32_t get() const noexcept { return addr_; }

 private:
  uint32_t addr_;
};
class elf64_addr {  // XXX
 public:
  static elf64_addr from_elf_addr(uint64_t) noexcept;

  uint64_t get() const noexcept { return addr_; }

 private:
  uint64_t addr_;
};

class elf32_off {  // XXX
 public:
  static elf32_off from_elf_off(uint32_t) noexcept;

  uint32_t get() const noexcept { return off_; }

 private:
  uint32_t off_;
};
class elf64_off {  // XXX
 public:
  static elf64_off from_elf_off(uint64_t) noexcept;

  uint64_t get() const noexcept { return off_; }

 private:
  uint64_t off_;
};

class elf_hdr_flags {  // XXX
 public:
  static elf_hdr_flags from_elf_flags(uint32_t) noexcept;

  uint32_t get() const noexcept { return fl_; }

 private:
  uint32_t fl_;
};


}} /* namespace ilias::elf */

#include "elf_types-inl.h"

#endif /* _ILIAS_ELF_ELF_TYPES_H_ */
