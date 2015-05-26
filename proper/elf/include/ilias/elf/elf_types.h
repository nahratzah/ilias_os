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


struct Elf32_Sym {
  Elf32_Word    st_name;
  Elf32_Addr    st_value;
  Elf32_Word    st_size;
  Elf32_Byte    st_info;
  Elf32_Byte    st_other;
  Elf32_Section st_shndx;
};

struct Elf64_Sym {
  Elf64_Word    st_name;
  Elf64_Byte    st_info;
  Elf64_Byte    st_other;
  Elf64_Section st_shndx;
  Elf64_Addr    st_value;
  Elf64_Xword   st_size;
};

/* Symbol binding. */
constexpr uint8_t ELF32_ST_BIND(uint8_t i) noexcept {
  return i >> 4;
}
constexpr uint8_t ELF32_ST_BIND(const Elf32_Sym& s) noexcept {
  return ELF32_ST_BIND(s.st_info);
}
constexpr uint8_t ELF32_ST_BIND(const Elf64_Sym& s) noexcept {
  return ELF32_ST_BIND(s.st_info);
}

constexpr uint8_t STB_LOCAL = 0;
constexpr uint8_t STB_GLOBAL = 1;
constexpr uint8_t STB_WEAK = 2;
constexpr uint8_t STB_LOPROC = 13;
constexpr uint8_t STB_HIPROC = 15;

/* Symbol types. */
constexpr uint8_t ELF32_ST_TYPE(uint8_t i) noexcept {
  return i & 0xfU;
}
constexpr uint8_t ELF32_ST_TYPE(const Elf32_Sym& s) noexcept {
  return ELF32_ST_TYPE(s.st_info);
}
constexpr uint8_t ELF32_ST_TYPE(const Elf64_Sym& s) noexcept {
  return ELF32_ST_TYPE(s.st_info);
}

constexpr uint8_t STT_NOTYPE = 0;
constexpr uint8_t STT_OBJECT = 1;
constexpr uint8_t STT_FUNC = 2;
constexpr uint8_t STT_SECTION = 3;
constexpr uint8_t STT_FILE = 4;
constexpr uint8_t STT_LOPROC = 13;
constexpr uint8_t STT_HIPROC = 15;


struct Elf32_Rel {
  Elf32_Addr  r_offset;
  Elf32_Word  r_info;
};

struct Elf64_Rel {
  Elf64_Addr  r_offset;
  Elf64_Xword r_sym;
};

struct Elf32_Rela {
  Elf32_Addr  r_offset;
  Elf32_Word  r_info;
  Elf32_Word  r_addend;
};

struct Elf64_Rela {
  Elf64_Addr  r_offset;
  Elf64_Xword r_sym;
  Elf64_Xword r_addend;
};

constexpr uint32_t ELF64_R_SYM(uint64_t info) noexcept {
  return info >> 32;
}
constexpr uint32_t ELF64_R_TYPE(uint64_t info) noexcept {
  return info & 0xffffffffU;
}


struct Elf32_Phdr {
  Elf32_Word  p_type;
  Elf32_Off   p_offset;
  Elf32_Addr  p_vaddr;
  Elf32_Addr  p_paddr;
  Elf32_Word  p_filesz;
  Elf32_Word  p_memsz;
  Elf32_Word  p_flags;
  Elf32_Word  p_align;
};

struct Elf64_Phdr {
  Elf64_Word  p_type;
  Elf64_Word  p_flags;
  Elf64_Off   p_offset;
  Elf64_Addr  p_vaddr;
  Elf64_Addr  p_paddr;
  Elf64_Xword p_filesz;
  Elf64_Xword p_memsz;
  Elf64_Xword p_align;
};

constexpr uint32_t PT_NULL = 0;
constexpr uint32_t PT_LOAD = 1;
constexpr uint32_t PT_DYNAMIC = 2;
constexpr uint32_t PT_INTERP = 3;
constexpr uint32_t PT_NOTE = 4;
constexpr uint32_t PT_SHLIB = 5;
constexpr uint32_t PT_PHDR = 6;
constexpr uint32_t PT_TLS = 7;
constexpr uint32_t PT_LOOS = 0x60000000;
constexpr uint32_t PT_HIOS = 0x6fffffff;
constexpr uint32_t PT_LOPROC = 0x70000000;
constexpr uint32_t PT_HIPROC = 0x7fffffff;

constexpr uint32_t PF_X = 0x1;
constexpr uint32_t PF_W = 0x2;
constexpr uint32_t PF_R = 0x4;
constexpr uint32_t PF_MASKPROC = 0xf0000000;


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
