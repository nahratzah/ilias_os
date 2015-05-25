#include <ilias/elf/elf_header.h>
#include <streambuf>
#include <ilias/elf/elf_error.h>

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
    throw elf_error("elf_header: magic identifier mismatch");

  ident_type ident = ident_type::create<0>(basic_ident());
  switch (nident[EI_CLASS]) {
  case elf_header::ELFCLASS32:
    ident = ident_type::create<0>(nident);
    break;
  case elf_header::ELFCLASS64:
    ident = ident_type::create<1>(nident);
    break;
  case elf_header::ELFCLASSNONE:
    throw elf_error("elf_header: no class");
  default:
    throw elf_error("elf_header: unrecognized elf class");
  }

  bool is_big_endian = false;
  bool is_little_endian = false;
  switch (nident[EI_DATA]) {
  default:
    throw elf_error("elf_header: endian format not recognized");
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
          [&len, &buf](const basic_ident& ident) {
            Elf32_Ehdr h;
            h.e_ident = ident;
            len = buf.sgetn(reinterpret_cast<char*>(&h) + EI_NIDENT,
                            sizeof(h) - EI_NIDENT);
            if (len != sizeof(h) - EI_NIDENT)
              throw underflow_error("elf_header: insufficient data");
            len += EI_NIDENT;
            return h;
          },
          [&len, &buf](const basic_ident& ident) {
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
    throw elf_error("elf_header: header too short");

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
    throw elf_error("elf_header: version validation failed");
  } else if (get_version() != EV_CURRENT) {
    throw elf_error("elf_header: invalid version");
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
    streamoff {
  using types::Elf32_Ehdr;
  using types::Elf64_Ehdr;

  return map_onto<streamoff>(
      hdr_,
      [this](const Elf32_Ehdr& h) {
        return endian_to_host(h.e_phoff);
      },
      [this](const Elf64_Ehdr& h) {
        return endian_to_host(h.e_phoff);
      });
}

auto elf_header::get_shoff() const noexcept ->
    streamoff {
  using types::Elf32_Ehdr;
  using types::Elf64_Ehdr;

  return map_onto<streamoff>(
      hdr_,
      [this](const Elf32_Ehdr& h) {
        return endian_to_host(h.e_shoff);
      },
      [this](const Elf64_Ehdr& h) {
        return endian_to_host(h.e_shoff);
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

auto elf_header::get_size() const noexcept -> streamsize {
  using types::Elf32_Ehdr;
  using types::Elf64_Ehdr;

  return map_onto<streamsize>(
      hdr_,
      [this](const Elf32_Ehdr& h) {
        return endian_to_host(h.e_ehsize);
      },
      [this](const Elf64_Ehdr& h) {
        return endian_to_host(h.e_ehsize);
      });
}

auto elf_header::get_phsize() const noexcept -> streamsize {
  using types::Elf32_Ehdr;
  using types::Elf64_Ehdr;

  return map_onto<streamsize>(
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

auto elf_header::get_shsize() const noexcept -> streamsize {
  using types::Elf32_Ehdr;
  using types::Elf64_Ehdr;

  return map_onto<streamsize>(
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


}} /* namespace ilias::elf */
