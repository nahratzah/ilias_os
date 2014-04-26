#ifndef _ILIAS_PMAP_CONSTS_INL_H_
#define _ILIAS_PMAP_CONSTS_INL_H_

#include <cstddef>
#include <cstdint>
#include <ilias/arch.h>
#include <abi/ext/log2.h>

namespace ilias {
namespace pmap {

/* Sizeof(void*) for a given architecture. */
constexpr size_t pointer_size(arch a) noexcept {
  switch (a) {
  case arch::i386:
    return 4;
  case arch::amd64:
    return 8;
  }
}

/* Size of page, for a given architecture. */
constexpr size_t page_size(arch a) noexcept {
  switch (a) {
  case arch::i386:
    return 0x1000;  // 4 kb pages on i386
  case arch::amd64:
    return 0x1000;  // 4 kb pages on amd64
  }
}

/* Page mask. */
constexpr size_t page_mask(arch a) noexcept {
  return page_size(a) - 1;
}

/* Page shift:  pg_no << page_shift == pg_addr. */
constexpr size_t page_shift(arch a) noexcept {
  return abi::ext::log2_down(page_size(a));
}

/* Number of bytes in a page address. */
constexpr size_t page_addr_size(arch a) noexcept {
  switch (a) {
  case arch::i386:  // PAE makes page address 64 bit
  case arch::amd64:
    return 8;
  }
}

/* Number of bytes in a page number. */
constexpr size_t page_no_size(arch a) noexcept {
  switch (a) {
  case arch::i386:
    return 4;  // PAE: 36 bits - 12 bits = 24, which fits.
  case arch::amd64:
    return 8;
  }
}

constexpr uintptr_t PAGE_SIZE = page_size(native_arch);
constexpr uintptr_t PAGE_MASK = page_mask(native_arch);
constexpr uintptr_t PAGE_SHIFT = page_shift(native_arch);

static_assert(abi::ext::is_pow2(PAGE_SIZE),
              "Page size must be a power of 2.");

template<typename T>
constexpr T round_page_down(T v, arch a) noexcept {
  return v & ~T(page_mask(a));
}

template<typename T>
constexpr T round_page_up(T v, arch a) noexcept {
  return round_page_down(v + page_mask(a), a);
}

}} /* namespace ilias::pmap */

#endif /* _ILIAS_PMAP_CONSTS_INL_H_ */
