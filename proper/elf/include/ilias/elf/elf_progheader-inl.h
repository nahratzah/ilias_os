#ifndef _ILIAS_ELF_ELF_PROGHEADER_INL_H_
#define _ILIAS_ELF_ELF_PROGHEADER_INL_H_

#include "elf_progheader.h"
#include <sys/endian.h>

namespace ilias {
namespace elf {


inline auto elf_progheader::get_underlying_type() const noexcept ->
    const any<types::Elf32_Phdr, types::Elf64_Phdr>& {
  return hdr_;
}

inline auto elf_progheader::is_data_big_endian() const noexcept -> bool {
  return hdr_data_ == elf_header::ELFDATA2MSB;
}

inline auto elf_progheader::is_data_little_endian() const noexcept -> bool {
  return hdr_data_ == elf_header::ELFDATA2LSB;
}

template<typename T>
inline auto elf_progheader::endian_to_host(T v) const noexcept -> T {
  using ::betoh;
  using ::letoh;

  return (is_data_big_endian() ? betoh(v) :
                                 (is_data_little_endian() ? letoh(v) :
                                                            v));
}


}} /* namespace ilias::elf */

#endif /* _ILIAS_ELF_ELF_PROGHEADER_INL_H_ */
