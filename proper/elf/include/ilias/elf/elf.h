#ifndef _ILIAS_ELF_ELF_H_
#define _ILIAS_ELF_ELF_H_

#include <cstddef>
#include <cstdint>
#include <array>
#include <bitset>
#include <streambuf>
#include <string>
#include <ilias/optional.h>
#include <ilias/any.h>
#include <sys/endian.h>
#include <ilias/elf/elf_types.h>

namespace ilias {
namespace elf {


using namespace std;

class elf_header {
 public:
  /* Object types. */
  enum class objtype : types::Elf32_Half {
    ET_NONE   = 0,
    ET_REL    = 1,
    ET_EXEC   = 2,
    ET_DYN    = 3,
    ET_CORE   = 4,
    ET_LOPROC = 0xff00U,
    ET_HIPROC = 0xffffU
  };
  static constexpr objtype ET_NONE   = objtype::ET_NONE;
  static constexpr objtype ET_REL    = objtype::ET_REL;
  static constexpr objtype ET_EXEC   = objtype::ET_EXEC;
  static constexpr objtype ET_DYN    = objtype::ET_DYN;
  static constexpr objtype ET_CORE   = objtype::ET_CORE;
  static constexpr objtype ET_LOPROC = objtype::ET_LOPROC;
  static constexpr objtype ET_HIPROC = objtype::ET_HIPROC;

  /* Machine types. */
  enum class machtype : types::Elf32_Half {
    EM_NONE = 0,
    EM_M32 = 1,  // AT&T WE 32100
    EM_SPARC = 2,  // Sparc
    EM_386 = 3,  // Intel 80386
    EM_68K = 4,  // Motorola 68000
    EM_88K = 5,  // Motorola 88000
    EM_860 = 7,  // Intel 80860
    EM_MIPS = 8,  // MIPS RS3000, Big Endian
    EM_MIPS_RS4_BE = 10,  // MIPS RS4000, Big Endian
    EM_SPARC64 = 11,  // SPARC v9 64-bit
    EM_PARISC = 15,  // HPPA
    EM_SPARC32PLUS = 18,  // Enhanced instruction set Sparc
    EM_PPC = 20,  // PowerPC
    EM_ARM = 40,  // ARM 32-bit (AArch32)
    EM_ALPHA = 41,  // DEC ALPHA
    EM_SH = 42,  // Hitachi/Renesas Super-H
    EM_SPARCV9 = 43,  // Sparc v9
    EM_IA_64 = 50,  // Intel IA-64
    EM_AMD64 = 62,  // AMD64
    EM_VAX = 75,  // DEC VAX
    EM_AARCH64 = 183,  // ARM 64-bit (AArch64)
  };
  static constexpr machtype EM_NONE         = machtype::EM_NONE;
  static constexpr machtype EM_M32          = machtype::EM_M32;
  static constexpr machtype EM_SPARC        = machtype::EM_SPARC;
  static constexpr machtype EM_386          = machtype::EM_386;
  static constexpr machtype EM_68K          = machtype::EM_68K;
  static constexpr machtype EM_88K          = machtype::EM_88K;
  static constexpr machtype EM_860          = machtype::EM_860;
  static constexpr machtype EM_MIPS         = machtype::EM_MIPS;
  static constexpr machtype EM_MIPS_RS3_BE  = EM_MIPS;  // Alias
  static constexpr machtype EM_MIPS_RS4_BE  = machtype::EM_MIPS_RS4_BE;
  static constexpr machtype EM_SPARC64      = machtype::EM_SPARC64;
  static constexpr machtype EM_PARISC       = machtype::EM_PARISC;
  static constexpr machtype EM_SPARC32PLUS  = machtype::EM_SPARC32PLUS;
  static constexpr machtype EM_PPC          = machtype::EM_PPC;
  static constexpr machtype EM_ARM          = machtype::EM_ARM;
  static constexpr machtype EM_ALPHA        = machtype::EM_ALPHA;
  static constexpr machtype EM_SH           = machtype::EM_SH;
  static constexpr machtype EM_SPARCV9      = machtype::EM_SPARCV9;
  static constexpr machtype EM_IA_64        = machtype::EM_IA_64;
  static constexpr machtype EM_AMD64        = machtype::EM_AMD64;
  static constexpr machtype EM_VAX          = machtype::EM_VAX;
  static constexpr machtype EM_AARCH64      = machtype::EM_AARCH64;

  /* Version numbers. */
  static constexpr types::Elf32_Word EV_NONE = 0;  // Invalid version
  static constexpr types::Elf32_Word EV_CURRENT = 1;  // Version of ELF spec

  /* ELF classes */
  static constexpr uint8_t ELFCLASSNONE = 0;
  static constexpr uint8_t ELFCLASS32   = 1;
  static constexpr uint8_t ELFCLASS64   = 2;

  /* Data encoding */
  static constexpr uint8_t ELFDATANONE = 0;
  static constexpr uint8_t ELFDATA2LSB = 1;
  static constexpr uint8_t ELFDATA2MSB = 2;

  elf_header(const elf_header&) noexcept = default;
  elf_header& operator=(const elf_header&) noexcept = default;
  elf_header(streambuf&);

  optional<size_t> get_name() const noexcept;
  objtype get_type() const noexcept;
  machtype get_mach() const noexcept;
  uint32_t get_version() const noexcept;
  any<elf32_addr, elf64_addr> get_entry() const noexcept;
  any<elf32_off, elf64_off> get_phoff() const noexcept;
  any<elf32_off, elf64_off> get_shoff() const noexcept;
  elf_hdr_flags get_flags() const noexcept;
  size_t get_size() const noexcept;
  size_t get_phsize() const noexcept;
  size_t get_phcount() const noexcept;
  size_t get_shsize() const noexcept;
  size_t get_shcount() const noexcept;
  optional<size_t> get_shstr_index() const noexcept;

  uint8_t get_class() const noexcept;
  bool is_class_32() const noexcept;  // 32-bit elf object file
  bool is_class_64() const noexcept;  // 64-bit elf object file

  uint8_t get_data() const noexcept;
  bool is_data_big_endian() const noexcept;
  bool is_data_little_endian() const noexcept;

  const array<uint8_t, types::EI_NIDENT>& get_ident() const noexcept;

 private:
  template<typename T> T endian_to_host(T) const noexcept;

  any<types::Elf32_Ehdr, types::Elf64_Ehdr> hdr_;
};

class elf_section {
 public:
  /* Flags. */
  using flags_type = bitset<32>;
  static constexpr flags_type SHF_WRITE = flags_type(0x1);
  static constexpr flags_type SHF_ALLOC = flags_type(0x2);
  static constexpr flags_type SHF_EXECINSTR = flags_type(0x4);
  static constexpr flags_type SHF_MASKPROC = flags_type(0xf0000000);

  /* Section types. */
  enum class sectiontype : types::Elf32_Word {
    SHT_NONE = 0,
    SHT_PROGBITS = 1,
    SHT_SYMTAB = 2,
    SHT_STRTAB = 3,
    SHT_RELA = 4,
    SHT_HASH = 5,
    SHT_DYNAMIC = 6,
    SHT_NOTE = 7,
    SHT_NOBITS = 8,
    SHT_REL = 9,
    SHT_SHLIB = 10,
    SHT_DYNSYM = 11,
    SHT_LOPROC = 0x70000000,
    SHT_HIPROC = 0x7fffffff,
    SHT_LOUSER = 0x80000000,
    SHT_HIUSER = 0xffffffff,
  };
  static constexpr sectiontype SHT_NONE = sectiontype::SHT_NONE;
  static constexpr sectiontype SHT_PROGBITS = sectiontype::SHT_PROGBITS;
  static constexpr sectiontype SHT_SYMTAB = sectiontype::SHT_SYMTAB;
  static constexpr sectiontype SHT_STRTAB = sectiontype::SHT_STRTAB;
  static constexpr sectiontype SHT_RELA = sectiontype::SHT_RELA;
  static constexpr sectiontype SHT_HASH = sectiontype::SHT_HASH;
  static constexpr sectiontype SHT_DYNAMIC = sectiontype::SHT_DYNAMIC;
  static constexpr sectiontype SHT_NOTE = sectiontype::SHT_NOTE;
  static constexpr sectiontype SHT_NOBITS = sectiontype::SHT_NOBITS;
  static constexpr sectiontype SHT_REL = sectiontype::SHT_REL;
  static constexpr sectiontype SHT_SHLIB = sectiontype::SHT_SHLIB;
  static constexpr sectiontype SHT_DYNSYM = sectiontype::SHT_DYNSYM;
  static constexpr sectiontype SHT_LOPROC = sectiontype::SHT_LOPROC;
  static constexpr sectiontype SHT_HIPROC = sectiontype::SHT_HIPROC;
  static constexpr sectiontype SHT_LOUSER = sectiontype::SHT_LOUSER;
  static constexpr sectiontype SHT_HIUSER = sectiontype::SHT_HIUSER;

  /* Special section indices. */
  static constexpr size_t SHN_UNDEF = 0;
  static constexpr size_t SHN_LORESERVE = 0xff00;
  static constexpr size_t SHN_LOPROC = 0xff00;
  static constexpr size_t SHN_HIPROC = 0xff1f;
  static constexpr size_t SHN_ABS = 0xfff1;
  static constexpr size_t SHN_COMMON = 0xfff2;
  static constexpr size_t SHN_HIRESERVE = 0xffff;

  /* Special section names. */
  static const string_ref sname_bss;
  static const string_ref sname_comment;
  static const string_ref sname_data;
  static const string_ref sname_data1;
  static const string_ref sname_debug;
  static const string_ref sname_dynamic;
  static const string_ref sname_hash;
  static const string_ref sname_line;
  static const string_ref sname_note;
  static const string_ref sname_rodata;
  static const string_ref sname_rodata1;
  static const string_ref sname_shstrtab;
  static const string_ref sname_strtab;
  static const string_ref sname_symtab;
  static const string_ref sname_text;

  elf_section() noexcept = default;
  elf_section(const elf_section&) noexcept = default;
  elf_section& operator=(const elf_section&) noexcept = default;
  elf_section(streambuf&, uint8_t, size_t);
  elf_section(streambuf&, const elf_header&);

  optional<size_t> get_name() const noexcept;
  flags_type get_flags() const noexcept;
  sectiontype get_type() const noexcept;
  streambuf::off_type get_offset() const noexcept;
  streamsize get_size() const noexcept;
  optional<size_t> get_link() const noexcept;
  optional<uint32_t> get_info() const noexcept;
  size_t get_align() const noexcept;
  optional<size_t> get_entsize() const noexcept;

  bool is_data_big_endian() const noexcept;
  bool is_data_little_endian() const noexcept;

 private:
  template<typename T> T endian_to_host(T) const noexcept;

  any<types::Elf32_Shdr, types::Elf64_Shdr> hdr_;
  uint8_t hdr_data_;
};


}} /* namespace ilias::elf */

#include "elf-inl.h"

#endif /* _ILIAS_ELF_ELF_H_ */
