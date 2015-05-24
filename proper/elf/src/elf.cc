#include <ilias/elf/elf.h>
#include <streambuf>

namespace ilias {
namespace elf {
namespace {


inline auto read_nident(streambuf& buf) -> array<uint8_t, types::EI_NIDENT> {
  array<uint8_t, types::EI_NIDENT> rv;
  auto len = buf.sgetn(reinterpret_cast<char*>(rv.data()), rv.size());
  if (len < 0 || len != rv.size())
    throw underflow_error("elf_header: wrong size read");

  return rv;
}

auto read_elf_header(streambuf& buf) ->
    any<types::Elf32_Ehdr, types::Elf64_Ehdr> {
  using types::Elf32_Ehdr;
  using types::Elf64_Ehdr;
  using types::EI_CLASS;
  using types::EI_DATA;
  using types::EI_NIDENT;
  using types::ELFMAG;
  using basic_ident = array<uint8_t, EI_NIDENT>;
  using ident_type = any<basic_ident, basic_ident>;

  static_assert(is_pod<types::Elf32_Ehdr>::value &&
                is_pod<types::Elf64_Ehdr>::value,
                "Memmory copy operation requires elf header to be "
                "a plain-old-data type.");

  const auto nident = read_nident(buf);
  if (!equal(ELFMAG.begin(), ELFMAG.end(), nident.begin()))
    throw runtime_error("elf_header: magic identifier mismatch");

  ident_type ident = ident_type::create<0>(basic_ident());
  switch (nident[EI_CLASS]) {
  case elf_header::ELFCLASS32:
    ident = ident_type::create<0>(nident);
    break;
  case elf_header::ELFCLASS64:
    ident = ident_type::create<1>(nident);
    break;
  case elf_header::ELFCLASSNONE:
    throw runtime_error("elf_header: no class");
  default:
    throw runtime_error("elf_header: unrecognized elf class");
  }

  bool is_big_endian = false;
  bool is_little_endian = false;
  switch (nident[EI_DATA]) {
  default:
    throw runtime_error("elf_header: endian format not recognized");
  case elf_header::ELFDATA2MSB:
    is_big_endian = true;
    break;
  case elf_header::ELFDATA2LSB:
    is_little_endian = true;
    break;
  }
  assert(is_big_endian || is_little_endian);

  streamsize len;
  auto hdr =
      map(move(ident),
          [&len, &buf](basic_ident ident) {
            Elf32_Ehdr h;
            h.e_ident = ident;
            len = buf.sgetn(reinterpret_cast<char*>(&h) + EI_NIDENT,
                            sizeof(h) - EI_NIDENT);
            if (len != sizeof(h) - EI_NIDENT)
              throw underflow_error("elf_header: insufficient data");
            len += EI_NIDENT;
            return h;
          },
          [&len, &buf](basic_ident ident) {
            Elf64_Ehdr h;
            h.e_ident = ident;
            len = buf.sgetn(reinterpret_cast<char*>(&h) + EI_NIDENT,
                            sizeof(h) - EI_NIDENT);
            if (len != sizeof(h) - EI_NIDENT)
              throw underflow_error("elf_header: insufficient data");
            len += EI_NIDENT;
            return h;
          });

  auto hdrlen = map_onto<streamsize>(
      hdr,
      [is_big_endian, is_little_endian](const Elf32_Ehdr& h) {
        return (is_big_endian ? betoh(h.e_ehsize) : letoh(h.e_ehsize));
      },
      [is_big_endian, is_little_endian](const Elf64_Ehdr& h) {
        return (is_big_endian ? betoh(h.e_ehsize) : letoh(h.e_ehsize));
      });

  if (hdrlen < len)
    throw runtime_error("elf_header: header too short");

  while (len < hdrlen) {
    buf.sbumpc();
    ++len;
  }

  return hdr;
}


} /* namespace ilias::elf::<unnamed> */


elf_header::elf_header(streambuf& buf)
: hdr_(read_elf_header(buf))
{
  if (get_version() != map_onto<uint8_t>(hdr_,
                                         [](const types::Elf32_Ehdr& h) {
                                           return h.e_ident[types::EI_VERSION];
                                         },
                                         [](const types::Elf64_Ehdr& h) {
                                           return h.e_ident[types::EI_VERSION];
                                         })) {
    throw runtime_error("elf_header: version validation failed");
  } else if (get_version() != EV_CURRENT) {
    throw runtime_error("elf_header: invalid version");
  }
}

auto elf_header::get_type() const noexcept -> objtype {
  return map_onto<objtype>(
      hdr_,
      [this](const types::Elf32_Ehdr& h) {
        return static_cast<objtype>(endian_to_host(h.e_type));
      },
      [this](const types::Elf64_Ehdr& h) {
        return static_cast<objtype>(endian_to_host(h.e_type));
      });
}

auto elf_header::get_mach() const noexcept -> machtype {
  return map_onto<machtype>(
      hdr_,
      [this](const types::Elf32_Ehdr& h) {
        return static_cast<machtype>(endian_to_host(h.e_machine));
      },
      [this](const types::Elf64_Ehdr& h) {
        return static_cast<machtype>(endian_to_host(h.e_machine));
      });
}

auto elf_header::get_version() const noexcept -> uint32_t {
  return map_onto<uint32_t>(
      hdr_,
      [this](const types::Elf32_Ehdr& h) {
        return endian_to_host(h.e_version);
      },
      [this](const types::Elf64_Ehdr& h) {
        return endian_to_host(h.e_version);
      });
}

auto elf_header::get_entry() const noexcept ->
    any<elf32_addr, elf64_addr> {
  using types::Elf32_Ehdr;
  using types::Elf64_Ehdr;

  return map(
      hdr_,
      [this](const Elf32_Ehdr& h) {
        return elf32_addr::from_elf_addr(endian_to_host(h.e_entry));
      },
      [this](const Elf64_Ehdr& h) {
        return elf64_addr::from_elf_addr(endian_to_host(h.e_entry));
      });
}

auto elf_header::get_phoff() const noexcept ->
    any<elf32_off, elf64_off> {
  using types::Elf32_Ehdr;
  using types::Elf64_Ehdr;

  return map(
      hdr_,
      [this](const Elf32_Ehdr& h) {
        return elf32_off::from_elf_off(endian_to_host(h.e_phoff));
      },
      [this](const Elf64_Ehdr& h) {
        return elf64_off::from_elf_off(endian_to_host(h.e_phoff));
      });
}

auto elf_header::get_shoff() const noexcept ->
    any<elf32_off, elf64_off> {
  using types::Elf32_Ehdr;
  using types::Elf64_Ehdr;

  return map(
      hdr_,
      [this](const Elf32_Ehdr& h) {
        return elf32_off::from_elf_off(endian_to_host(h.e_shoff));
      },
      [this](const Elf64_Ehdr& h) {
        return elf64_off::from_elf_off(endian_to_host(h.e_shoff));
      });
}

auto elf_header::get_flags() const noexcept -> elf_hdr_flags {
  using types::Elf32_Ehdr;
  using types::Elf64_Ehdr;

  return map_onto<elf_hdr_flags>(
      hdr_,
      [this](const Elf32_Ehdr& h) {
        return elf_hdr_flags::from_elf_flags(endian_to_host(h.e_flags));
      },
      [this](const Elf64_Ehdr& h) {
        return elf_hdr_flags::from_elf_flags(endian_to_host(h.e_flags));
      });
}

auto elf_header::get_size() const noexcept -> size_t {
  using types::Elf32_Ehdr;
  using types::Elf64_Ehdr;

  return map_onto<size_t>(
      hdr_,
      [this](const Elf32_Ehdr& h) {
        return endian_to_host(h.e_ehsize);
      },
      [this](const Elf64_Ehdr& h) {
        return endian_to_host(h.e_ehsize);
      });
}

auto elf_header::get_phsize() const noexcept -> size_t {
  using types::Elf32_Ehdr;
  using types::Elf64_Ehdr;

  return map_onto<size_t>(
      hdr_,
      [this](const Elf32_Ehdr& h) {
        return endian_to_host(h.e_phentsize);
      },
      [this](const Elf64_Ehdr& h) {
        return endian_to_host(h.e_phentsize);
      });
}

auto elf_header::get_phcount() const noexcept -> size_t {
  using types::Elf32_Ehdr;
  using types::Elf64_Ehdr;

  return map_onto<size_t>(
      hdr_,
      [this](const Elf32_Ehdr& h) {
        return endian_to_host(h.e_phnum);
      },
      [this](const Elf64_Ehdr& h) {
        return endian_to_host(h.e_phnum);
      });
}

auto elf_header::get_shsize() const noexcept -> size_t {
  using types::Elf32_Ehdr;
  using types::Elf64_Ehdr;

  return map_onto<size_t>(
      hdr_,
      [this](const Elf32_Ehdr& h) {
        return endian_to_host(h.e_shentsize);
      },
      [this](const Elf64_Ehdr& h) {
        return endian_to_host(h.e_shentsize);
      });
}

auto elf_header::get_shcount() const noexcept -> size_t {
  using types::Elf32_Ehdr;
  using types::Elf64_Ehdr;

  return map_onto<size_t>(
      hdr_,
      [this](const Elf32_Ehdr& h) {
        return endian_to_host(h.e_shnum);
      },
      [this](const Elf64_Ehdr& h) {
        return endian_to_host(h.e_shnum);
      });
}

auto elf_header::get_shstr_index() const noexcept -> optional<size_t> {
  using types::Elf32_Ehdr;
  using types::Elf64_Ehdr;

  const size_t shstrndx = map_onto<size_t>(
      hdr_,
      [this](const Elf32_Ehdr& h) {
        return endian_to_host(h.e_shstrndx);
      },
      [this](const Elf64_Ehdr& h) {
        return endian_to_host(h.e_shstrndx);
      });

  if (shstrndx == types::SHN_UNDEF)
    return optional<size_t>();
  return shstrndx;
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
