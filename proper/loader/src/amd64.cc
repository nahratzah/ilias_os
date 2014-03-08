#include <loader/amd64.h>
#include <loader/x86_video.h>
#include <cstdint>
#include <utility>

namespace loader {


struct bios_memmap_line {
  uint64_t base;  /* Start of region. */
  uint64_t len;  /* Length of region. */
  uint32_t region_type;  /* Region type index. */
  uint32_t acpi30_ext_attr;  /* ACPI flag bits. */

  static const uint32_t region_usable = 1;
  static const uint32_t region_reserved = 2;
  static const uint32_t region_acpi_reclaimable = 3;
  static const uint32_t region_acpi_nvs = 4;
  static const uint32_t region_bad = 5;  /* Broken memory. */

  static const uint32_t acpi30_ignore = 0x0001;  /* Ignore this entry. */
  static const uint32_t acpi30_nonvolatile = 0x0002;  /* Non-volatile memory. */

  /* Test if memory is free/available. */
  bool free() const noexcept {
    return (region_type == region_usable) &&
           (acpi30_ext_attr == 0);
  }

  /*
   * Create an entry that has page-aligned addresses.
   * Free memory is shrunk inward, while all other memory is grown outward.
   *
   * Align must be a power-of-2.
   */
  bios_memmap_line align(abi::uint64_t align) const noexcept {
    abi::uint64_t mask = align - 1;
    abi::uint64_t start = this->base;
    abi::uint64_t end = this->base + this->len;

    if (this->free()) {
      start += mask;
      start &= ~mask;
      end &= ~mask;
    } else {
      start &= ~mask;
      end += mask;
      end &= ~mask;
    }

    bios_memmap_line rv = *this;
    rv.base = start;
    if (end < start || this->len == 0)
      rv.len = 0;
    else
      rv.len = end - start;
    return rv;
  }

  /* Read the first memory map line from bios. */
  template<typename Fn>
  static bool visit(Fn&& fn)
      noexcept(noexcept(fn(std::declval<bios_memmap_line>()))) {
    static const abi::uint32_t magic = 0x534d4150;

    bios_memmap_line line;
    abi::uint32_t contid = 0;
    abi::uint32_t signature;
    abi::uint32_t bytes;

    do {
      line.acpi30_ext_attr = 1;  /* Ask for valid acpi entry. */
      asm volatile (
        "int $0x15"
      : "=a" (signature),
        "=b" (contid),
        "=c" (bytes)
      : "a" (0xe820),
        "b" (contid),
        "c" (sizeof(line)),
        "d" (magic),
        "D" (&line)
      : "cc", "memory"  /* Condition code, memory. */
      );
      if (signature != magic)
        return false;
      if (bytes < 20)
        line.acpi30_ext_attr = 0;  /* BIOS does not fill in acpi field. */
      fn(std::move(line));
    } while (contid);
    return true;
  }

  operator memorymap::range() const noexcept {
    return memorymap::range{ base, len };
  }
};

memorymap amd64_memorymap() {
  bios_put_str("Loading memory map");
  memorymap rv;
  const bool valid = bios_memmap_line::visit(
    [&rv](bios_memmap_line l) {
      if (l.len > 0 && l.free()) rv.push_back(l.align(4096));
      bios_put_char('.');
    });
  if (!valid) throw std::runtime_error("bios map invalid");

  rv.shrink_to_fit();
  return rv;
}


} /* namespace loader */
