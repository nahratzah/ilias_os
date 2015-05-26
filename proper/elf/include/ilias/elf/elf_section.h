#ifndef _ILIAS_ELF_ELF_SECTION_H_
#define _ILIAS_ELF_ELF_SECTION_H_

#include <cstddef>
#include <cstdint>
#include <array>
#include <bitset>
#include <iosfwd>
#include <ios>
#include <string>
#include <ilias/optional.h>
#include <ilias/any.h>
#include <ilias/elf/elf_types.h>
#include <ilias/elf/elf_header.h>

namespace ilias {
namespace elf {


using namespace std;

class elf_section {
 public:
  /* Flags. */
  using flags_type = bitset<64>;
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
  static constexpr size_t SHN_UNDEF = types::SHN_UNDEF;
  static constexpr size_t SHN_LORESERVE = types::SHN_LORESERVE;
  static constexpr size_t SHN_LOPROC = types::SHN_LOPROC;
  static constexpr size_t SHN_HIPROC = types::SHN_HIPROC;
  static constexpr size_t SHN_ABS = SHN_ABS;
  static constexpr size_t SHN_COMMON = SHN_COMMON;
  static constexpr size_t SHN_HIRESERVE = SHN_HIRESERVE;

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

  elf_section(const elf_section&) noexcept = default;
  elf_section& operator=(const elf_section&) noexcept = default;
  elf_section(streambuf&, const elf_header&);

  optional<size_t> get_name() const noexcept;
  flags_type get_flags() const noexcept;
  sectiontype get_type() const noexcept;
  streamoff get_offset() const noexcept;
  streamsize get_size() const noexcept;
  optional<size_t> get_link() const noexcept;
  optional<uint32_t> get_info() const noexcept;
  any<elf32_addr, elf64_addr> get_align() const noexcept;
  optional<streamsize> get_entsize() const noexcept;

  auto get_underlying_type() const noexcept ->
      const any<types::Elf32_Shdr, types::Elf64_Shdr>&;

  bool is_data_big_endian() const noexcept;
  bool is_data_little_endian() const noexcept;

 private:
  template<typename T> T endian_to_host(T) const noexcept;

  any<types::Elf32_Shdr, types::Elf64_Shdr> hdr_;
  uint8_t hdr_data_;
};


}} /* namespace ilias::elf */

#include "elf_section-inl.h"

#endif /* _ILIAS_ELF_ELF_SECTION_H_ */
