#include <ilias/elf/elf_progheader.h>
#include <streambuf>
#include <ilias/elf/elf_error.h>

namespace ilias {
namespace elf {
namespace {


auto read_elf_progheader(streambuf& buf, const elf_header& eh) ->
    any<types::Elf32_Phdr, types::Elf64_Phdr> {
  using types::Elf32_Phdr;
  using types::Elf64_Phdr;
  using types::Elf32_Ehdr;
  using types::Elf64_Ehdr;

  static_assert(is_pod<types::Elf32_Phdr>::value &&
                is_pod<types::Elf32_Phdr>::value,
                "Memory copy operation requires elf program header to be "
                "a plain-old-data type.");

  streamsize len;
  auto rv = map(
      eh.get_underlying_type(),
      [&len, &buf, &eh](const Elf32_Ehdr&) {
        Elf32_Phdr rv;
        if (static_cast<streamsize>(sizeof(rv)) > eh.get_phsize())
          throw elf_error("elf_progheader: program header too short");
        len = buf.sgetn(reinterpret_cast<char*>(&rv), sizeof(rv));
        if (len != sizeof(rv))
          throw underflow_error("elf_progheader: insufficient data");
        return rv;
      },
      [&len, &buf, &eh](const Elf64_Ehdr&) {
        Elf64_Phdr rv;
        if (static_cast<streamsize>(sizeof(rv)) > eh.get_phsize())
          throw elf_error("elf_progheader: program header too short");
        len = buf.sgetn(reinterpret_cast<char*>(&rv), sizeof(rv));
        if (len != sizeof(rv))
          throw underflow_error("elf_progheader: insufficient data");
        return rv;
      });

  while (len < eh.get_phsize()) {
    buf.sbumpc();
    ++len;
  }

  return rv;
}


} /* namespace ilias::elf::<unnamed> */


elf_progheader::elf_progheader(streambuf& buf, const elf_header& eh)
: hdr_(read_elf_progheader(buf, eh)),
  hdr_data_(eh.get_data())
{}

auto elf_progheader::get_type() const noexcept -> uint32_t {
  using types::Elf32_Phdr;
  using types::Elf64_Phdr;

  return map_onto<uint32_t>(
      hdr_,
      [this](const Elf32_Phdr& h) {
        return endian_to_host(h.p_type);
      },
      [this](const Elf64_Phdr& h) {
        return endian_to_host(h.p_type);
      });
}

auto elf_progheader::get_flags() const noexcept -> uint32_t {
  using types::Elf32_Phdr;
  using types::Elf64_Phdr;

  return map_onto<uint32_t>(
      hdr_,
      [this](const Elf32_Phdr& h) {
        return endian_to_host(h.p_flags);
      },
      [this](const Elf64_Phdr& h) {
        return endian_to_host(h.p_flags);
      });
}

auto elf_progheader::get_offset() const noexcept -> streamoff {
  using types::Elf32_Phdr;
  using types::Elf64_Phdr;

  return map_onto<streamoff>(
      hdr_,
      [this](const Elf32_Phdr& h) {
        return endian_to_host(h.p_offset);
      },
      [this](const Elf64_Phdr& h) {
        return endian_to_host(h.p_offset);
      });
}

auto elf_progheader::get_vaddr() const noexcept ->
    any<elf32_addr, elf64_addr> {
  using types::Elf32_Phdr;
  using types::Elf64_Phdr;

  return map(
      hdr_,
      [this](const Elf32_Phdr& h) {
        return elf32_addr::from_elf_addr(endian_to_host(h.p_vaddr));
      },
      [this](const Elf64_Phdr& h) {
        return elf64_addr::from_elf_addr(endian_to_host(h.p_vaddr));
      });
}

auto elf_progheader::get_paddr() const noexcept ->
    any<elf32_addr, elf64_addr> {
  using types::Elf32_Phdr;
  using types::Elf64_Phdr;

  return map(
      hdr_,
      [this](const Elf32_Phdr& h) {
        return elf32_addr::from_elf_addr(endian_to_host(h.p_paddr));
      },
      [this](const Elf64_Phdr& h) {
        return elf64_addr::from_elf_addr(endian_to_host(h.p_paddr));
      });
}

auto elf_progheader::get_filesize() const noexcept -> streamoff {
  using types::Elf32_Phdr;
  using types::Elf64_Phdr;

  return map_onto<streamoff>(
      hdr_,
      [this](const Elf32_Phdr& h) {
        return endian_to_host(h.p_filesz);
      },
      [this](const Elf64_Phdr& h) {
        return endian_to_host(h.p_filesz);
      });
}

auto elf_progheader::get_memsize() const noexcept ->
    any<elf32_addr, elf64_addr> {
  using types::Elf32_Phdr;
  using types::Elf64_Phdr;

  return map(
      hdr_,
      [this](const Elf32_Phdr& h) {
        return elf32_addr::from_elf_addr(endian_to_host(h.p_memsz));
      },
      [this](const Elf64_Phdr& h) {
        return elf64_addr::from_elf_addr(endian_to_host(h.p_memsz));
      });
}

auto elf_progheader::get_align() const noexcept ->
    any<elf32_addr, elf64_addr> {
  using types::Elf32_Phdr;
  using types::Elf64_Phdr;

  return map(
      hdr_,
      [this](const Elf32_Phdr& h) {
        return elf32_addr::from_elf_addr(endian_to_host(h.p_align));
      },
      [this](const Elf64_Phdr& h) {
        return elf64_addr::from_elf_addr(endian_to_host(h.p_align));
      });
}


}} /* namespace ilias::elf */
