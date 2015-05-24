#include <ilias/elf/elf.h>
#include <streambuf>

namespace ilias {
namespace elf {


elf_header::elf_header(streambuf& buf) {
  using unsigned_len = make_unsigned_t<streamsize>;

  static_assert(is_pod<types::Elf32_Ehdr>::value,
                "Memmory copy operation requires elf header to be "
                "a plain-old-data type.");

  unsigned_len len = buf.sgetn(reinterpret_cast<char*>(&hdr_), sizeof(hdr_));
  if (_predict_false(len != sizeof(hdr_)))
    throw underflow_error("elf_header: wrong size read");

  if (valid_header()) {
    if (get_size() < sizeof(hdr_))
      throw invalid_argument("elf_header: too short");

    while (len < get_size()) {
      buf.sbumpc();
      ++len;
    }
  }
}


elf_section::elf_section(streambuf& buf, uint8_t hdr_data, size_t slen)
: hdr_data_(hdr_data)
{
  using unsigned_len = make_unsigned_t<streamsize>;

  static_assert(is_pod<types::Elf32_Shdr>::value,
                "Memmory copy operation requires elf header to be "
                "a plain-old-data type.");

  if (slen < sizeof(hdr_))
    throw invalid_argument("elf_section: too short");

  unsigned_len len = buf.sgetn(reinterpret_cast<char*>(&hdr_), sizeof(hdr_));
  if (_predict_false(len != sizeof(hdr_)))
    throw underflow_error("elf_section: wrong size read");

  while (len < slen) {
    buf.sbumpc();
    ++len;
  }
}

elf_section::elf_section(streambuf& buf, const elf_header& eh)
: elf_section(buf, eh.get_data(), eh.get_shsize())
{}


constexpr elf_header::objtype elf_header::ET_NONE;
constexpr elf_header::objtype elf_header::ET_REL;
constexpr elf_header::objtype elf_header::ET_EXEC;
constexpr elf_header::objtype elf_header::ET_DYN;
constexpr elf_header::objtype elf_header::ET_CORE;
constexpr elf_header::objtype elf_header::ET_LOPROC;
constexpr elf_header::objtype elf_header::ET_HIPROC;

constexpr elf_header::machtype elf_header::EM_NONE;
constexpr elf_header::machtype elf_header::EM_M32;
constexpr elf_header::machtype elf_header::EM_SPARC;
constexpr elf_header::machtype elf_header::EM_386;
constexpr elf_header::machtype elf_header::EM_68K;
constexpr elf_header::machtype elf_header::EM_88K;
constexpr elf_header::machtype elf_header::EM_860;
constexpr elf_header::machtype elf_header::EM_MIPS;
constexpr elf_header::machtype elf_header::EM_MIPS_RS3_BE;
constexpr elf_header::machtype elf_header::EM_MIPS_RS4_BE;
constexpr elf_header::machtype elf_header::EM_SPARC64;
constexpr elf_header::machtype elf_header::EM_PARISC;
constexpr elf_header::machtype elf_header::EM_SPARC32PLUS;
constexpr elf_header::machtype elf_header::EM_PPC;
constexpr elf_header::machtype elf_header::EM_ARM;
constexpr elf_header::machtype elf_header::EM_ALPHA;
constexpr elf_header::machtype elf_header::EM_SH;
constexpr elf_header::machtype elf_header::EM_SPARCV9;
constexpr elf_header::machtype elf_header::EM_IA_64;
constexpr elf_header::machtype elf_header::EM_AMD64;
constexpr elf_header::machtype elf_header::EM_VAX;
constexpr elf_header::machtype elf_header::EM_AARCH64;

constexpr types::Elf32_Word elf_header::EV_NONE;
constexpr types::Elf32_Word elf_header::EV_CURRENT;

constexpr uint8_t elf_header::ELFCLASSNONE;
constexpr uint8_t elf_header::ELFCLASS32;
constexpr uint8_t elf_header::ELFCLASS64;

constexpr uint8_t elf_header::ELFDATANONE;
constexpr uint8_t elf_header::ELFDATA2LSB;
constexpr uint8_t elf_header::ELFDATA2MSB;


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
