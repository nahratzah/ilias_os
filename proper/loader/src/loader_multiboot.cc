#include <loader/multiboot.h>
#include <cdecl.h>
#include <cstdint>
#include <string>
#include <vector>

namespace loader {
namespace multiboot {

_cdecl_begin

/* Multiboot provided data is stored here by loader.s */
uint32_t mb_magic;
uint32_t mb_data;

_cdecl_end


bool valid() noexcept {
  return mb_magic == 0x2badB002;
}


struct multiboot_header {
  uint32_t magic;
  uint32_t flags;
  uint32_t checksum;
  uint32_t header_addr;
  uint32_t load_addr;
  uint32_t load_end_addr;
  uint32_t bss_end_addr;
  uint32_t entry_addr;
};

struct aout_symbol_table {
  uint32_t tabsize;
  uint32_t strsize;
  uint32_t addr;
  uint32_t reserved;
};

struct elf_section_header_table {
  uint32_t num;
  uint32_t size;
  uint32_t addr;
  uint32_t shndx;
};

struct multiboot_info {
  uint32_t flags;
  uint32_t mem_lower;
  uint32_t mem_upper;
  uint32_t boot_device;
  uint32_t cmdline;
  uint32_t mods_count;
  uint32_t mod_addr;

  union {
    aout_symbol_table aout_sym;
    elf_section_header_table elf_sec;
  } u;

  uint32_t mmap_length;
  uint32_t mmap_addr;
};

struct module {
  uint32_t mod_start;
  uint32_t mod_end;
  uint32_t string;
  uint32_t reserved;
};

struct memory_map {
  uint32_t size;
  uint32_t base_addr_low;
  uint32_t base_addr_high;
  uint32_t length_low;
  uint32_t length_high;
  uint32_t type;
};


std::vector<memory_line> memory_map() {
  assert(valid());

  std::vector<memory_line> result;

  multiboot_info* info = reinterpret_cast<multiboot_info*>(mb_data);
  if (info->flags & (1U << 6)) {
    /* memory map is valid. */
    for (struct memory_map* mm =
             reinterpret_cast<struct memory_map*>(info->mmap_addr);
         mm < reinterpret_cast<void*>(info->mmap_addr + info->mmap_length);
         mm = reinterpret_cast<struct memory_map*>(
             reinterpret_cast<uint32_t>(mm) +
             mm->size + sizeof(unsigned int))) {
      uint64_t base = (uint64_t(mm->base_addr_high) << 32) |
                      uint64_t(mm->base_addr_low);
      uint64_t len = (uint64_t(mm->length_high) << 32) |
                     uint64_t(mm->length_low);
      bool usable = (mm->type == 1);

      result.push_back({ base, len, usable });
    }
  }

  return result;
}

std::string_ref command_line() {
  assert(valid());

  std::string_ref result;
  multiboot_info* info = reinterpret_cast<multiboot_info*>(mb_data);
  if (info->flags & (1U << 2)) {
    const char* cmd_ptr = reinterpret_cast<const char*>(info->cmdline);
    if (cmd_ptr) result = cmd_ptr;
  }
  return result;
}


}} /* namespace loader::multiboot */
