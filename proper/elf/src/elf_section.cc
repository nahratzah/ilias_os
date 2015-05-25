#include <ilias/elf/elf_section.h>
#include <streambuf>
#include <ilias/elf/elf_error.h>

namespace ilias {
namespace elf {
namespace {


auto read_elf_section(streambuf& buf, const elf_header& eh) ->
    any<types::Elf32_Shdr, types::Elf64_Shdr> {
  using types::Elf32_Shdr;
  using types::Elf64_Shdr;
  using types::Elf32_Ehdr;
  using types::Elf64_Ehdr;

  static_assert(is_pod<types::Elf32_Shdr>::value &&
                is_pod<types::Elf64_Shdr>::value,
                "Memmory copy operation requires elf header to be "
                "a plain-old-data type.");

  streamsize len;
  auto rv = map(
      eh.get_underlying_type(),
      [&len, &buf, &eh](const Elf32_Ehdr&) {
        Elf32_Shdr rv;
        if (static_cast<streamsize>(sizeof(rv)) > eh.get_shsize())
          throw elf_error("elf_section: section header too short");
        len = buf.sgetn(reinterpret_cast<char*>(&rv), sizeof(rv));
        if (len != sizeof(rv))
          throw underflow_error("elf_section: insufficient data");
        return rv;
      },
      [&len, &buf, &eh](const Elf64_Ehdr&) {
        Elf64_Shdr rv;
        if (static_cast<streamsize>(sizeof(rv)) > eh.get_shsize())
          throw elf_error("elf_section: section header too short");
        len = buf.sgetn(reinterpret_cast<char*>(&rv), sizeof(rv));
        if (len != sizeof(rv))
          throw underflow_error("elf_section: insufficient data");
        return rv;
      });

  while (len < eh.get_shsize()) {
    buf.sbumpc();
    ++len;
  }

  return rv;
}


} /* namespace ilias::elf::<unnamed> */


elf_section::elf_section(streambuf& buf, const elf_header& eh)
: hdr_(read_elf_section(buf, eh)),
  hdr_data_(eh.get_data())
{}

auto elf_section::get_name() const noexcept -> optional<size_t> {
  using types::Elf32_Shdr;
  using types::Elf64_Shdr;

  const size_t idx = map_onto<size_t>(
      hdr_,
      [this](const Elf32_Shdr& h) {
        return endian_to_host(h.sh_name);
      },
      [this](const Elf64_Shdr& h) {
        return endian_to_host(h.sh_name);
      });
  if (idx != 0) return idx;
  return optional<size_t>();
}

auto elf_section::get_flags() const noexcept -> flags_type {
  using types::Elf32_Shdr;
  using types::Elf64_Shdr;

  return map_onto<flags_type>(
      hdr_,
      [this](const Elf32_Shdr& h) {
        return flags_type(endian_to_host(h.sh_flags));
      },
      [this](const Elf64_Shdr& h) {
        return flags_type(endian_to_host(h.sh_flags));
      });
}

auto elf_section::get_type() const noexcept -> sectiontype {
  using types::Elf32_Shdr;
  using types::Elf64_Shdr;

  return map_onto<sectiontype>(
      hdr_,
      [this](const Elf32_Shdr& h) {
        return static_cast<sectiontype>(endian_to_host(h.sh_type));
      },
      [this](const Elf64_Shdr& h) {
        return static_cast<sectiontype>(endian_to_host(h.sh_type));
      });
}

auto elf_section::get_offset() const noexcept -> streamoff {
  using types::Elf32_Shdr;
  using types::Elf64_Shdr;

  return map_onto<streamoff>(
      hdr_,
      [this](const Elf32_Shdr& h) {
        return endian_to_host(h.sh_offset);
      },
      [this](const Elf64_Shdr& h) {
        return endian_to_host(h.sh_offset);
      });
}

auto elf_section::get_size() const noexcept -> streamsize {
  using types::Elf32_Shdr;
  using types::Elf64_Shdr;

  return map_onto<streamsize>(
      hdr_,
      [this](const Elf32_Shdr& h) {
        return endian_to_host(h.sh_size);
      },
      [this](const Elf64_Shdr& h) {
        return endian_to_host(h.sh_size);
      });
}

auto elf_section::get_link() const noexcept -> optional<size_t> {
  using types::Elf32_Shdr;
  using types::Elf64_Shdr;

  switch (get_type()) {
  default:
    return optional<size_t>();
  case SHT_DYNAMIC:
  case SHT_HASH:
  case SHT_REL:
  case SHT_RELA:
  case SHT_SYMTAB:
  case SHT_DYNSYM:
    return map_onto<size_t>(
        hdr_,
        [this](const Elf32_Shdr& h) {
          return endian_to_host(h.sh_link);
        },
        [this](const Elf64_Shdr& h) {
          return endian_to_host(h.sh_link);
        });
  }
}

auto elf_section::get_info() const noexcept -> optional<uint32_t> {
  using types::Elf32_Shdr;
  using types::Elf64_Shdr;

  switch (get_type()) {
  default:
    return optional<uint32_t>();
  case SHT_REL:
  case SHT_RELA:
  case SHT_SYMTAB:
  case SHT_DYNSYM:
    return map_onto<uint32_t>(
        hdr_,
        [this](const Elf32_Shdr& h) {
          return endian_to_host(h.sh_info);
        },
        [this](const Elf64_Shdr& h) {
          return endian_to_host(h.sh_info);
        });
  }
}

auto elf_section::get_align() const noexcept -> any<elf32_addr, elf64_addr> {
  using types::Elf32_Shdr;
  using types::Elf64_Shdr;

  return map(
      hdr_,
      [this](const Elf32_Shdr& h) {
        return elf32_addr::from_elf_addr(endian_to_host(h.sh_addralign));
      },
      [this](const Elf64_Shdr& h) {
        return elf64_addr::from_elf_addr(endian_to_host(h.sh_addralign));
      });
}

auto elf_section::get_entsize() const noexcept -> optional<streamsize> {
  using types::Elf32_Shdr;
  using types::Elf64_Shdr;

  size_t entsize = map_onto<streamsize>(
      hdr_,
      [this](const Elf32_Shdr& h) {
        return endian_to_host(h.sh_entsize);
      },
      [this](const Elf64_Shdr& h) {
        return endian_to_host(h.sh_entsize);
      });
  if (entsize == 0) return optional<streamsize>();
  return entsize;
}


constexpr elf_section::flags_type elf_section::SHF_WRITE;
constexpr elf_section::flags_type elf_section::SHF_ALLOC;
constexpr elf_section::flags_type elf_section::SHF_EXECINSTR;
constexpr elf_section::flags_type elf_section::SHF_MASKPROC;

constexpr elf_section::sectiontype elf_section::SHT_NONE;
constexpr elf_section::sectiontype elf_section::SHT_PROGBITS;
constexpr elf_section::sectiontype elf_section::SHT_SYMTAB;
constexpr elf_section::sectiontype elf_section::SHT_STRTAB;
constexpr elf_section::sectiontype elf_section::SHT_RELA;
constexpr elf_section::sectiontype elf_section::SHT_HASH;
constexpr elf_section::sectiontype elf_section::SHT_DYNAMIC;
constexpr elf_section::sectiontype elf_section::SHT_NOTE;
constexpr elf_section::sectiontype elf_section::SHT_NOBITS;
constexpr elf_section::sectiontype elf_section::SHT_REL;
constexpr elf_section::sectiontype elf_section::SHT_SHLIB;
constexpr elf_section::sectiontype elf_section::SHT_DYNSYM;
constexpr elf_section::sectiontype elf_section::SHT_LOPROC;
constexpr elf_section::sectiontype elf_section::SHT_HIPROC;
constexpr elf_section::sectiontype elf_section::SHT_LOUSER;
constexpr elf_section::sectiontype elf_section::SHT_HIUSER;

constexpr size_t elf_section::SHN_UNDEF;
constexpr size_t elf_section::SHN_LORESERVE;
constexpr size_t elf_section::SHN_LOPROC;
constexpr size_t elf_section::SHN_HIPROC;
constexpr size_t elf_section::SHN_ABS;
constexpr size_t elf_section::SHN_COMMON;
constexpr size_t elf_section::SHN_HIRESERVE;

const string_ref elf_section::sname_bss      = string_ref(".bss", 4);
const string_ref elf_section::sname_comment  = string_ref(".comment", 8);
const string_ref elf_section::sname_data     = string_ref(".data", 5);
const string_ref elf_section::sname_data1    = string_ref(".data1", 6);
const string_ref elf_section::sname_debug    = string_ref(".debug", 6);
const string_ref elf_section::sname_dynamic  = string_ref(".dynamic", 8);
const string_ref elf_section::sname_hash     = string_ref(".hash", 5);
const string_ref elf_section::sname_line     = string_ref(".line", 5);
const string_ref elf_section::sname_note     = string_ref(".note", 5);
const string_ref elf_section::sname_rodata   = string_ref(".rodata", 7);
const string_ref elf_section::sname_rodata1  = string_ref(".rodata1" ,8);
const string_ref elf_section::sname_shstrtab = string_ref(".shstrtab", 9);
const string_ref elf_section::sname_strtab   = string_ref(".strtab", 7);
const string_ref elf_section::sname_symtab   = string_ref(".symtab", 7);
const string_ref elf_section::sname_text     = string_ref(".text", 5);


}} /* namespace ilias::elf */
