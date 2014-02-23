#ifndef _ABI_EXT_READER_H_
#define _ABI_EXT_READER_H_

#include <cdecl.h>
#include <abi/abi.h>
#include <cstdint>

namespace __cxxabiv1 {
namespace ext {
namespace {


/* Repeat a bit pattern. */
template<typename UInt> constexpr UInt repeat(UInt base, unsigned int delta) {
  return (delta >= CHAR_BIT * sizeof(UInt) ?
          base :
          repeat((base << delta) | base, 2U * delta));
}

/*
 * Collapse v, such that every N-th bit is the inclusive or
 * of every (N % bits) bit.
 * Example: collapse(0xf00a, 8) = 0x00fa
 */
template<typename UInt> constexpr UInt collapse(UInt v, unsigned int bits) {
  return (bits >= CHAR_BIT * sizeof(UInt) ?
          v :
          ((collapse(v, 2 * bits) >> bits) |
           (collapse(v, 2 * bits) & ((UInt(1) << bits) - 1U))));
}

/*
 * Produce a bitmask that is laid out in memory as ffff0000
 * (with given number of bytes set to 0xff)
 */
template<typename UInt> constexpr UInt mask_lowaddr_bytes(unsigned int bytes) {
#if defined(__BIG_ENDIAN__)
  return (bytes >= sizeof(UInt) ?
          UInt(0) :
          ~((UInt(1) << (bytes * CHAR_BIT)) - UInt(1)));
#elif defined(__LITTLE_ENDIAN__)
  return (bytes >= sizeof(UInt) ?
          ~UInt(0) :
          ((UInt(1) << (bytes * CHAR_BIT)) - UInt(1)));
#else
  static_assert(false, "Unknown endianness.");
#endif
}
/*
 * Produce a bitmask that is laid out in memory as 0000ffff
 * (with given number of bytes set to 0xff)
 */
template<typename UInt> constexpr UInt mask_highaddr_bytes(
    unsigned int bytes) {
#if defined(__BIG_ENDIAN__)
  return (bytes >= sizeof(UInt) ?
          ~UInt(0) :
          ((UInt(1) << (bytes * CHAR_BIT)) - UInt(1)));
#elif defined(__LITTLE_ENDIAN__)
  return (bytes >= sizeof(UInt) ?
          UInt(0) :
          ~((UInt(1) << (bytes * CHAR_BIT)) - UInt(1)));
#else
  static_assert(false, "Unknown endianness.");
#endif
}
/*
 * Rotate int, such that byte N becomes byte 0.
 */
template<typename UInt> constexpr UInt rotate_down_addr(UInt v,
                                                        unsigned int bytes) {
#if defined(__BIG_ENDIAN__)
  return (bytes % sizeof(v) == 0 ?
          v :
          (v << ((            (bytes % sizeof(v))) * CHAR_BIT) |
           v >> ((sizeof(v) - (bytes % sizeof(v))) * CHAR_BIT)));
#elif defined(__LITTLE_ENDIAN__)
  return (bytes % sizeof(v) == 0 ?
          v :
          (v >> ((            (bytes % sizeof(v))) * CHAR_BIT) |
           v << ((sizeof(v) - (bytes % sizeof(v))) * CHAR_BIT)));
#else
  static_assert(false, "Unknown endianness.");
#endif
}
/*
 * Rotate int, such that byte 0 becomes byte N.
 */
template<typename UInt> constexpr UInt rotate_up_addr(UInt v,
                                                        unsigned int bytes) {
#if defined(__BIG_ENDIAN__)
  return (bytes % sizeof(v) == 0 ?
          v :
          (v >> ((            (bytes % sizeof(v))) * CHAR_BIT) |
           v << ((sizeof(v) - (bytes % sizeof(v))) * CHAR_BIT)));
#elif defined(__LITTLE_ENDIAN__)
  return (bytes % sizeof(v) == 0 ?
          v :
          (v << ((            (bytes % sizeof(v))) * CHAR_BIT) |
           v >> ((sizeof(v) - (bytes % sizeof(v))) * CHAR_BIT)));
#else
  static_assert(false, "Unknown endianness.");
#endif
}
/*
 * Merge two ints together, such that the resulting int spans
 * from N bytes into the first int, up to N bytes into the second
 * int.
 */
template<typename UInt> constexpr UInt merge_addr(UInt a, UInt b,
                                                  unsigned int bytes) {
  return rotate_down_addr(a & mask_highaddr_bytes<UInt>(bytes), bytes) |
         rotate_up_addr(b & mask_lowaddr_bytes<UInt>(bytes),
                        sizeof(UInt) - bytes);
}

using align_t = uintptr_t;
constexpr uintptr_t ALIGN = sizeof(align_t);
constexpr uintptr_t ALIGN_MSK = ALIGN - 1U;

/*
 * Instruct the CPU to prefect this many bytes ahead.
 * Must be a power-of-2.
 */
constexpr unsigned int READ_AHEAD = 3 * ALIGN;
constexpr unsigned int WRITE_AHEAD = ALIGN;

/* Prefetch for read access, with no locality. */
inline void prefetch_r(const void* addr) noexcept {
  __builtin_prefetch(addr, 0, 0);
}
/* Prefetch for write access, with no locality. */
inline void prefetch_w(const void* addr) noexcept {
  __builtin_prefetch(addr, 1, 0);
}

/*
 * Returns false if we are absolutely sure that needle is not in haystack.
 * Returns true if the needle may be in the haystack.
 */
template<typename Haystack, typename Needle> constexpr bool maybe_contains(
    Haystack h, Needle n) {
  static_assert(sizeof(n) <= sizeof(h), "Needle is larger than haystack.");
  static_assert(sizeof(h) % sizeof(n) == 0,
      "Haystack size must be a multiple of needle size.");

  /*
   * Filter haystack, all bits that are equal will be set to 1,
   * then collapse it into a single needle-sized entry;
   * if that entry is all 1s, the needle may be present.
   */
  return collapse<Haystack>(~(h ^ repeat<Haystack>(n, CHAR_BIT * sizeof(n))),
                            CHAR_BIT * sizeof(n)) ==
         ((Needle(1) << (CHAR_BIT * sizeof(n))) - 1U);
}

/*
 * Return the low N bytes from v, shifting v in the process.
 */
template<typename UInt> UInt consume_bytes(UInt* v, unsigned int bytes)
    noexcept {
  UInt rv = *v & mask_lowaddr_bytes<UInt>(bytes);
  *v = rotate_down_addr(*v, bytes);
#if defined(__BIG_ENDIAN__)
  rv >>= CHAR_BIT * (sizeof(rv) - bytes);
#elif defined(__LITTLE_ENDIAN__)
  /* Don't need to do anything. */
#else
  static_assert(false, "Unknown endianness.");
#endif
  return rv;
}

/* Read direction. */
enum direction {
  DIR_FORWARD,
  DIR_BACKWARD
};


template<direction Dir, typename UInt> class reader;

class basic_reader {
 public:
  template<typename T> const T* addr() const noexcept {
    return reinterpret_cast<const T*>(addr_);
  }

  inline uintptr_t masked_addr() const noexcept {
    return addr_ & ALIGN_MSK;
  }

  inline uintptr_t base_addr() const noexcept {
    return addr_ & ~ALIGN_MSK;
  }

 protected:
  basic_reader() = default;
  basic_reader(const basic_reader&) = default;
  basic_reader& operator=(const basic_reader&) = default;

  inline basic_reader(uintptr_t addr, direction dir, uintptr_t len) noexcept
  : addr_(addr)
  {
    switch (dir) {
    case DIR_FORWARD:
      for (uintptr_t i = 0; i <= READ_AHEAD && i < len; i += ALIGN)
        prefetch_r(reinterpret_cast<const void*>(base_addr() + i));
      break;
    case DIR_BACKWARD:
      for (uintptr_t i = (masked_addr() == 0 ? 1 : 0);
           i <= READ_AHEAD && i < len;
           i += ALIGN)
        prefetch_r(reinterpret_cast<const void*>(base_addr() - i));
      break;
    }

    if (masked_addr() != 0)
      data_ = *reinterpret_cast<const align_t*>(base_addr());
  }

  inline align_t update(direction dir, uintptr_t len) noexcept {
    uintptr_t update_addr = base_addr();
    uintptr_t preread_addr;

    switch (dir) {
    case DIR_FORWARD:
      preread_addr = update_addr + READ_AHEAD;
      break;
    case DIR_BACKWARD:
      if (masked_addr() == 0) update_addr -= ALIGN;
      preread_addr = update_addr - READ_AHEAD;
      break;
    }

    if (len >= READ_AHEAD)
      prefetch_r(reinterpret_cast<const void*>(preread_addr));
    return data_ = *reinterpret_cast<const align_t*>(update_addr);
  }

  template<typename UInt> bool maybe_contains_(UInt c, align_t mask)
      const noexcept {
    auto filter = (data_ ^ repeat(align_t(c), sizeof(c) * CHAR_BIT));
    filter &= mask;
    return collapse(filter, sizeof(c) * CHAR_BIT) == ~UInt(c);
  }

  align_t data_;  // Data at (addr_ & ~ALIGN_MSK).
  uintptr_t addr_;  // Current address.
};

template<typename UInt> class reader<DIR_FORWARD, UInt>
: public basic_reader {
 public:
  static_assert(sizeof(UInt) <= sizeof(align_t), "Integral type too large.");
  static_assert(sizeof(align_t) % sizeof(UInt) == 0,
      "Integral type cannot align");

  reader() = default;
  reader(const reader&) = default;
  reader& operator=(const reader&) = default;

  inline reader(const void* addr, uintptr_t len = UINTPTR_MAX) noexcept
  : basic_reader(reinterpret_cast<uintptr_t>(addr), DIR_FORWARD, len)
  {}

  inline uint8_t read8(uintptr_t len = UINTPTR_MAX) noexcept {
    auto rv = (masked_addr() == 0 ? update(DIR_FORWARD, len) : data_);
    rv = rotate_down_addr(rv, masked_addr());
    addr_ += sizeof(UInt);
    return consume_bytes(&rv, sizeof(UInt));
  }

  bool maybe_contains(UInt c) const noexcept {
    if (avail() == 0) return false;
    return maybe_contains_(c, mask_highaddr_bytes<align_t>(masked_addr()));
  }

  inline uintptr_t avail() const noexcept {
    return (masked_addr() == 0 ? 0U : ALIGN - masked_addr());
  }

  inline align_t readN(uintptr_t len = UINTPTR_MAX) noexcept {
    const auto p = (masked_addr() == 0 ? update(DIR_FORWARD, len) : data_);
    addr_ += ALIGN;
    if (masked_addr() == 0) return p;
    const auto s = (len > avail() ? update(DIR_FORWARD, len) : 0U);
    return merge_addr(p, s, masked_addr());
  }
};

template<typename UInt> class reader<DIR_BACKWARD, UInt>
: public basic_reader {
 public:
  static_assert(sizeof(UInt) <= sizeof(align_t), "Integral type too large.");
  static_assert(sizeof(align_t) % sizeof(UInt) == 0,
      "Integral type cannot align");

  reader() = default;
  reader(const reader&) = default;
  reader& operator=(const reader&) = default;

  inline reader(const void* addr, uintptr_t len = UINTPTR_MAX) noexcept
  : basic_reader(reinterpret_cast<uintptr_t>(addr), DIR_BACKWARD, len)
  {}

  inline UInt read8(uintptr_t len = UINTPTR_MAX) noexcept {
    auto rv = (masked_addr() == 0 ? update(DIR_BACKWARD, len) : data_);
    addr_ -= sizeof(UInt);
    rv = rotate_down_addr(rv, masked_addr());
    return consume_bytes(&rv, sizeof(UInt));
  }

  bool maybe_contains(UInt c) const noexcept {
    if (avail() == 0) return false;
    return this->maybe_contains_(c, mask_lowaddr_bytes<align_t>(masked_addr()));
  }

  inline uintptr_t avail() const noexcept {
    return masked_addr();
  }

  inline align_t readN(uintptr_t len = UINTPTR_MAX) noexcept {
    const auto s = (masked_addr() == 0 ? update(DIR_BACKWARD, len) : data_);
    addr_ -= ALIGN;
    if (masked_addr() == 0) return s;
    const auto p = update(DIR_BACKWARD, len);
    return merge_addr(p, s, masked_addr());
  }
};


} /* namespace __cxxabiv1::ext::<unnamed> */


template<typename UInt> int memcmp(const UInt* a, const UInt* b, size_t len)
    noexcept {
  if (len == 0) return 0;
  auto ra = reader<DIR_FORWARD, UInt>(a, len);
  auto rb = reader<DIR_FORWARD, UInt>(b, len);

  /* Read in strides of register. */
  while (len >= ALIGN) {
    auto va = ra.readN(len);
    auto vb = ra.readN(len);

    if (_predict_false(va != vb)) {
      /* Difference found. */
      for (;;) {
        const auto ca = consume_bytes(&va, sizeof(UInt));
        const auto cb = consume_bytes(&vb, sizeof(UInt));
        if (ca != cb)
          return (sizeof(UInt) < sizeof(int) ?
                  int(ca) - int(cb) :
                  (ca < cb ? -1 : 1));
      }
      /* UNREACHABLE */
    }

    len -= ALIGN / sizeof(UInt);
  }

  /* Read in strides of UInt. */
  while (len > 0) {
    auto va = ra.read8(len + 1U);
    auto vb = rb.read8(len + 1U);

    if (_predict_false(va != vb)) {
      return (sizeof(UInt) < sizeof(int) ?
              int(va) - int(vb) :
              (va < vb ? -1 : 1));
    }

    --len;
  }
  return 0;
}


template<typename UInt> const UInt* memfind(const UInt* s_haystack,
                                            size_t n_haystack,
                                            const UInt* s_needle,
                                            size_t n_needle) noexcept {
  for (; n_haystack >= n_needle; ++s_haystack, --n_haystack) {
    if (memcmp(s_haystack, s_needle, n_needle) == 0)
      return s_haystack;
  }
  return nullptr;
}

/*
 * Specialize memfind for small integral type.
 * The function uses a histogram to reduce complexity.
 */
const uint8_t* memfind(const uint8_t*, size_t,
                       const uint8_t*, size_t) noexcept;

/* Use uint8_t memfind specialization for char. */
inline const char* memfind(const char* s_haystack, size_t n_haystack,
                           const char* s_needle, size_t n_needle) noexcept {
  return reinterpret_cast<const char*>(memfind(
      reinterpret_cast<const uint8_t*>(s_haystack),
      n_haystack,
      reinterpret_cast<const uint8_t*>(s_needle),
      n_needle));
}

/* Use uint8_t memfind specialization for signed char. */
inline const int8_t* memfind(const int8_t* s_haystack, size_t n_haystack,
                             const int8_t* s_needle, size_t n_needle)
    noexcept {
  return reinterpret_cast<const int8_t*>(memfind(
      reinterpret_cast<const uint8_t*>(s_haystack),
      n_haystack,
      reinterpret_cast<const uint8_t*>(s_needle),
      n_needle));
}


}} /* namespace __cxxabiv1::ext */

#endif /* _ABI_EXT_READER_H_ */
