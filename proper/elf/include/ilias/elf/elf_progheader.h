#ifndef _ILIAS_ELF_ELF_PROGHEADER_H_
#define _ILIAS_ELF_ELF_PROGHEADER_H_

#include <cstddef>
#include <cstdint>
#include <iosfwd>
#include <ios>
#include <ilias/any.h>
#include <ilias/elf/elf_types.h>
#include <ilias/elf/elf_header.h>

namespace ilias {
namespace elf {


using namespace std;

class elf_progheader {
 public:
  static constexpr uint32_t PT_NULL    = types::PT_NULL;
  static constexpr uint32_t PT_LOAD    = types::PT_LOAD;
  static constexpr uint32_t PT_DYNAMIC = types::PT_DYNAMIC;
  static constexpr uint32_t PT_INTERP  = types::PT_INTERP;
  static constexpr uint32_t PT_NOTE    = types::PT_NOTE;
  static constexpr uint32_t PT_SHLIB   = types::PT_SHLIB;
  static constexpr uint32_t PT_PHDR    = types::PT_PHDR;
  static constexpr uint32_t PT_TLS     = types::PT_TLS;
  static constexpr uint32_t PT_LOOS    = types::PT_LOOS;
  static constexpr uint32_t PT_HIOS    = types::PT_HIOS;
  static constexpr uint32_t PT_LOPROC  = types::PT_LOPROC;
  static constexpr uint32_t PT_HIPROC  = types::PT_HIPROC;

  static constexpr uint32_t PF_X        = types::PF_X;
  static constexpr uint32_t PF_W        = types::PF_W;
  static constexpr uint32_t PF_R        = types::PF_R;
  static constexpr uint32_t PF_MASKPROC = types::PF_MASKPROC;

  elf_progheader(const elf_progheader&) noexcept = default;
  elf_progheader& operator=(const elf_progheader&) noexcept = default;
  elf_progheader(streambuf&, const elf_header&);

  uint32_t get_type() const noexcept;
  uint32_t get_flags() const noexcept;
  streamoff get_offset() const noexcept;
  any<elf32_addr, elf64_addr> get_vaddr() const noexcept;
  any<elf32_addr, elf64_addr> get_paddr() const noexcept;
  streamsize get_filesize() const noexcept;
  any<elf32_addr, elf64_addr> get_memsize() const noexcept;
  any<elf32_addr, elf64_addr> get_align() const noexcept;

  auto get_underlying_type() const noexcept ->
      const any<types::Elf32_Phdr, types::Elf64_Phdr>&;

  bool is_data_big_endian() const noexcept;
  bool is_data_little_endian() const noexcept;

 private:
  template<typename T> T endian_to_host(T) const noexcept;

  any<types::Elf32_Phdr, types::Elf64_Phdr> hdr_;
  uint8_t hdr_data_;
};


}} /* namespace ilias::elf */

#include "elf_progheader-inl.h"

#endif /* _ILIAS_ELF_ELF_PROGHEADER_H_ */
