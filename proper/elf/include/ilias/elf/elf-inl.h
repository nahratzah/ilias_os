#ifndef _ILIAS_ELF_ELF_INL_H_
#define _ILIAS_ELF_ELF_INL_H_

#include "elf.h"

namespace ilias {
namespace elf {


inline auto elf_header::get_class() const noexcept -> uint8_t {
  return get_ident()[types::EI_CLASS];
}

inline auto elf_header::is_class_32() const noexcept -> bool {
  return get_class() == ELFCLASS32;
}

inline auto elf_header::is_class_64() const noexcept -> bool {
  return get_class() == ELFCLASS64;
}

inline auto elf_header::get_data() const noexcept -> uint8_t {
  return get_ident()[types::EI_DATA];
}

inline auto elf_header::is_data_big_endian() const noexcept -> bool {
  return get_data() == ELFDATA2MSB;
}

inline auto elf_header::is_data_little_endian() const noexcept -> bool {
  return get_data() == ELFDATA2LSB;
}

inline auto elf_header::get_ident() const noexcept ->
    const array<uint8_t, types::EI_NIDENT>& {
  using types::Elf32_Ehdr;
  using types::Elf64_Ehdr;

  return map_onto<const array<uint8_t, types::EI_NIDENT>&>(
      hdr_,
      [](const Elf32_Ehdr& h) -> const array<uint8_t, types::EI_NIDENT>& {
        return h.e_ident;
      },
      [](const Elf64_Ehdr& h) -> const array<uint8_t, types::EI_NIDENT>& {
        return h.e_ident;
      });
}

template<typename T>
inline auto elf_header::endian_to_host(T v) const noexcept -> T {
  using ::betoh;
  using ::letoh;

  return (is_data_big_endian() ? betoh(v) :
                                 (is_data_little_endian() ? letoh(v) :
                                                            v));
}


inline auto elf_section::get_name() const noexcept -> optional<size_t> {
  const size_t idx = endian_to_host(hdr_.sh_name);
  if (idx != 0) return idx;
  return optional<size_t>();
}

inline auto elf_section::get_flags() const noexcept -> flags_type {
  return flags_type(endian_to_host(hdr_.sh_flags));
}

inline auto elf_section::get_type() const noexcept -> sectiontype {
  return static_cast<sectiontype>(endian_to_host(hdr_.sh_type));
}

inline auto elf_section::get_offset() const noexcept -> streambuf::off_type {
  return hdr_.sh_offset;
}

inline auto elf_section::get_size() const noexcept -> streamsize {
  return hdr_.sh_size;
}

inline auto elf_section::get_link() const noexcept -> optional<size_t> {
  switch (get_type()) {
  default:
    return optional<size_t>();
  case SHT_DYNAMIC:
  case SHT_HASH:
  case SHT_REL:
  case SHT_RELA:
  case SHT_SYMTAB:
  case SHT_DYNSYM:
    return endian_to_host(hdr_.sh_link);
  }
}

inline auto elf_section::get_info() const noexcept -> optional<uint32_t> {
  switch (get_type()) {
  default:
    return optional<uint32_t>();
  case SHT_REL:
  case SHT_RELA:
  case SHT_SYMTAB:
  case SHT_DYNSYM:
    return endian_to_host(hdr_.sh_info);
  }
}

inline auto elf_section::get_align() const noexcept -> size_t {
  return hdr_.sh_addralign;
}

inline auto elf_section::get_entsize() const noexcept -> optional<size_t> {
  if (hdr_.sh_entsize == 0) return optional<size_t>();
  return hdr_.sh_entsize;
}

inline auto elf_section::is_data_big_endian() const noexcept -> bool {
  return hdr_data_ == elf_header::ELFDATA2MSB;
}

inline auto elf_section::is_data_little_endian() const noexcept -> bool {
  return hdr_data_ == elf_header::ELFDATA2LSB;
}

template<typename T>
inline auto elf_section::endian_to_host(T v) const noexcept -> T {
  using ::betoh;
  using ::letoh;

  return (is_data_big_endian() ? betoh(v) :
                                 (is_data_little_endian() ? letoh(v) :
                                                            v));
}


}} /* namespace ilias::elf */

#endif /* _ILIAS_ELF_ELF_INL_H_ */
