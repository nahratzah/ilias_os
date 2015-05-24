#ifndef _ILIAS_ELF_ELF_HEADER_INL_H_
#define _ILIAS_ELF_ELF_HEADER_INL_H_

#include "elf_header.h"
#include <sys/endian.h>

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

inline auto elf_header::get_underlying_type() const noexcept ->
    const any<types::Elf32_Ehdr, types::Elf64_Ehdr>& {
  return hdr_;
}

template<typename T>
inline auto elf_header::endian_to_host(T v) const noexcept -> T {
  using ::betoh;
  using ::letoh;

  return (is_data_big_endian() ? betoh(v) :
                                 (is_data_little_endian() ? letoh(v) :
                                                            v));
}


}} /* namespace ilias::elf */

#endif /* _ILIAS_ELF_ELF_HEADER_INL_H_ */
