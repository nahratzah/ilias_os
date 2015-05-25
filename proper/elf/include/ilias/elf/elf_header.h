#ifndef _ILIAS_ELF_ELF_HEADER_H_
#define _ILIAS_ELF_ELF_HEADER_H_

#include <cstddef>
#include <cstdint>
#include <array>
#include <bitset>
#include <iosfwd>
#include <ios>
#include <ilias/optional.h>
#include <ilias/any.h>
#include <ilias/elf/elf_types.h>

namespace ilias {
namespace elf {


using namespace std;

class elf_header {
 public:
  /* Object types. */
  enum class objtype : types::Elf32_Half {
    ET_NONE   = 0,
    ET_REL    = 1,
    ET_EXEC   = 2,
    ET_DYN    = 3,
    ET_CORE   = 4,
    ET_LOPROC = 0xff00U,
    ET_HIPROC = 0xffffU
  };
  static constexpr objtype ET_NONE   = objtype::ET_NONE;
  static constexpr objtype ET_REL    = objtype::ET_REL;
  static constexpr objtype ET_EXEC   = objtype::ET_EXEC;
  static constexpr objtype ET_DYN    = objtype::ET_DYN;
  static constexpr objtype ET_CORE   = objtype::ET_CORE;
  static constexpr objtype ET_LOPROC = objtype::ET_LOPROC;
  static constexpr objtype ET_HIPROC = objtype::ET_HIPROC;

  /* Machine types. */
  enum class machtype : types::Elf32_Half {
    EM_NONE = 0,
    EM_M32 = 1,  // AT&T WE 32100
    EM_SPARC = 2,  // Sparc
    EM_386 = 3,  // Intel 80386
    EM_68K = 4,  // Motorola 68000
    EM_88K = 5,  // Motorola 88000
    EM_860 = 7,  // Intel 80860
    EM_MIPS = 8,  // MIPS RS3000, Big Endian
    EM_MIPS_RS4_BE = 10,  // MIPS RS4000, Big Endian
    EM_SPARC64 = 11,  // SPARC v9 64-bit
    EM_PARISC = 15,  // HPPA
    EM_SPARC32PLUS = 18,  // Enhanced instruction set Sparc
    EM_PPC = 20,  // PowerPC
    EM_ARM = 40,  // ARM 32-bit (AArch32)
    EM_ALPHA = 41,  // DEC ALPHA
    EM_SH = 42,  // Hitachi/Renesas Super-H
    EM_SPARCV9 = 43,  // Sparc v9
    EM_IA_64 = 50,  // Intel IA-64
    EM_AMD64 = 62,  // AMD64
    EM_VAX = 75,  // DEC VAX
    EM_AARCH64 = 183,  // ARM 64-bit (AArch64)
  };
  static constexpr machtype EM_NONE         = machtype::EM_NONE;
  static constexpr machtype EM_M32          = machtype::EM_M32;
  static constexpr machtype EM_SPARC        = machtype::EM_SPARC;
  static constexpr machtype EM_386          = machtype::EM_386;
  static constexpr machtype EM_68K          = machtype::EM_68K;
  static constexpr machtype EM_88K          = machtype::EM_88K;
  static constexpr machtype EM_860          = machtype::EM_860;
  static constexpr machtype EM_MIPS         = machtype::EM_MIPS;
  static constexpr machtype EM_MIPS_RS3_BE  = EM_MIPS;  // Alias
  static constexpr machtype EM_MIPS_RS4_BE  = machtype::EM_MIPS_RS4_BE;
  static constexpr machtype EM_SPARC64      = machtype::EM_SPARC64;
  static constexpr machtype EM_PARISC       = machtype::EM_PARISC;
  static constexpr machtype EM_SPARC32PLUS  = machtype::EM_SPARC32PLUS;
  static constexpr machtype EM_PPC          = machtype::EM_PPC;
  static constexpr machtype EM_ARM          = machtype::EM_ARM;
  static constexpr machtype EM_ALPHA        = machtype::EM_ALPHA;
  static constexpr machtype EM_SH           = machtype::EM_SH;
  static constexpr machtype EM_SPARCV9      = machtype::EM_SPARCV9;
  static constexpr machtype EM_IA_64        = machtype::EM_IA_64;
  static constexpr machtype EM_AMD64        = machtype::EM_AMD64;
  static constexpr machtype EM_VAX          = machtype::EM_VAX;
  static constexpr machtype EM_AARCH64      = machtype::EM_AARCH64;

  /* Version numbers. */
  static constexpr types::Elf32_Word EV_NONE = 0;  // Invalid version
  static constexpr types::Elf32_Word EV_CURRENT = 1;  // Version of ELF spec

  /* ELF classes */
  static constexpr uint8_t ELFCLASSNONE = 0;
  static constexpr uint8_t ELFCLASS32   = 1;
  static constexpr uint8_t ELFCLASS64   = 2;

  /* Data encoding */
  static constexpr uint8_t ELFDATANONE = 0;
  static constexpr uint8_t ELFDATA2LSB = 1;
  static constexpr uint8_t ELFDATA2MSB = 2;

  elf_header(const elf_header&) noexcept = default;
  elf_header& operator=(const elf_header&) noexcept = default;
  elf_header(streambuf&);

  optional<size_t> get_name() const noexcept;
  objtype get_type() const noexcept;
  machtype get_mach() const noexcept;
  uint32_t get_version() const noexcept;
  any<elf32_addr, elf64_addr> get_entry() const noexcept;
  streamoff get_phoff() const noexcept;
  streamoff get_shoff() const noexcept;
  elf_hdr_flags get_flags() const noexcept;
  streamsize get_size() const noexcept;
  streamsize get_phsize() const noexcept;
  size_t get_phcount() const noexcept;
  streamsize get_shsize() const noexcept;
  size_t get_shcount() const noexcept;
  optional<size_t> get_shstr_index() const noexcept;

  uint8_t get_class() const noexcept;
  bool is_class_32() const noexcept;  // 32-bit elf object file
  bool is_class_64() const noexcept;  // 64-bit elf object file

  uint8_t get_data() const noexcept;
  bool is_data_big_endian() const noexcept;
  bool is_data_little_endian() const noexcept;

  const array<uint8_t, types::EI_NIDENT>& get_ident() const noexcept;

  auto get_underlying_type() const noexcept ->
      const any<types::Elf32_Ehdr, types::Elf64_Ehdr>&;

 private:
  template<typename T> T endian_to_host(T) const noexcept;

  any<types::Elf32_Ehdr, types::Elf64_Ehdr> hdr_;
};


}} /* namespace ilias::elf */

#include "elf_header-inl.h"

#endif /* _ILIAS_ELF_ELF_HEADER_H_ */
