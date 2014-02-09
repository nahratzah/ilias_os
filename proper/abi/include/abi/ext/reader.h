#ifndef _ABI_EXT_READER_H_
#define _ABI_EXT_READER_H_

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

#if 0
/*
 * Memory reader.
 *
 * Reads bytes (preferable in readN mode, returning align_t)
 * from memory.  Effort is expended to use the largest memory
 * access as possible (width of 1 register).  Because of this,
 * at any time two registers will be held in memory, shifted
 * as appropriate for the current address.
 *
 * Reads are given a hint about how many more bytes are (likely)
 * to be read, allowing the reader to prefetch data into its
 * cache lines.
 */
class reader {
 public:
  reader() = default;
  reader(const reader&) = default;
  reader& operator=(const reader&) = default;

  inline reader(direction dir, const void* addr) noexcept
  : addr_(reinterpret_cast<uintptr_t>(addr)),
    dir_(dir)
  {
    const intptr_t delta = (dir_ == DIR_BACKWARD ?
                            -intptr_t(READ_AHEAD / ALIGN) :
                            intptr_t(READ_AHEAD / ALIGN));
    prefetch_r(reinterpret_cast<const align_t*>(addr_) + delta);
    data_ = *reinterpret_cast<const align_t*>(addr_ & ~ALIGN_MSK);
  }

  const void* addr() const noexcept {
    return reinterpret_cast<const void*>(addr_);
  }

  inline uint8_t read8() noexcept {
    align_t rv;
    switch (dir_) {
    case DIR_FORWARD:
      rv = rotate_down_addr(data_, addr_ & ALIGN_MSK);
      ++addr_;
      update();
      break;
    case DIR_BACKWARD:
      update();
      --addr_;
      rv = rotate_down_addr(data_, addr_ & ALIGN_MSK);
      break;
    }
    return consume_bytes(&rv, 1);
  }

  inline align_t readN() noexcept {
    align_t p, s;
    const auto off = addr_ & ALIGN_MSK;
    const auto off_inv = ~off & ALIGN_MSK;  // off_inv = ALIGN - off;
    unsigned int merge_off;

    switch (dir_) {
    case DIR_FORWARD:
      p = data_;
      (addr_ &= ~ALIGN_MSK) += ALIGN;  // addr_ += off_inv
      update();
      s = data_;
      addr_ |= off;  // addr_ += off
      merge_off = (addr_ & ALIGN_MSK);  // off
      break;
    case DIR_BACKWARD:
      s = data_;
      addr_ &= ~ALIGN_MSK;  // addr_ -= off
      update();
      p = data_;
      (addr_ -= ALIGN) |= off;  // addr_ -= off_inv
      merge_off = off;
      break;
    }
    return merge_addr(p, s, merge_off);
  }

  inline bool maybe_contains(uint8_t c) const noexcept {
    const align_t xordata = (data_ ^ repeat(align_t(c), CHAR_BIT));
    return collapse(xordata & mask_highaddr_bytes<align_t>(addr_ & ALIGN_MSK),
                    CHAR_BIT) == 0xffU;
  }

 private:
  inline void update() noexcept {
    if ((addr_ & ALIGN_MSK) == 0) {
      const uintptr_t addr = addr_ - (dir_ == DIR_BACKWARD ? ALIGN : 0U);
      const intptr_t delta = (dir_ == DIR_BACKWARD ?
                              -intptr_t(READ_AHEAD / ALIGN) :
                              intptr_t(READ_AHEAD / ALIGN));

      prefetch_r(reinterpret_cast<const align_t*>(addr) + delta);
      data_ = *reinterpret_cast<const align_t*>(addr);
    }
  }

  align_t data_;  // Value covering addr_.
  uintptr_t addr_;
  const direction dir_;
};
#endif // 0


template<direction Dir> class reader;

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

    if (len >= READ_AHEAD) prefetch_r(reinterpret_cast<const void*>(preread_addr));
    return data_ = *reinterpret_cast<const align_t*>(update_addr);
  }

  template<typename UInt> bool maybe_contains_(UInt c, align_t mask) const noexcept {
    auto filter = (data_ ^ repeat(align_t(c), sizeof(c) * CHAR_BIT));
    filter &= mask;
    return collapse(filter, sizeof(c) * CHAR_BIT) == ~UInt(c);
  }

  align_t data_;  // Data at (addr_ & ~ALIGN_MSK).
  uintptr_t addr_;  // Current address.
};

template<> class reader<DIR_FORWARD> : public basic_reader {
 public:
  reader() = default;
  reader(const reader&) = default;
  reader& operator=(const reader&) = default;

  inline reader(const void* addr, uintptr_t len = UINTPTR_MAX) noexcept
  : basic_reader(reinterpret_cast<uintptr_t>(addr), DIR_FORWARD, len)
  {}

  inline uint8_t read8(uintptr_t len = UINTPTR_MAX) noexcept {
    auto rv = (masked_addr() == 0 ? update(DIR_FORWARD, len) : data_);
    rv = rotate_down_addr(rv, masked_addr());
    ++addr_;
    return consume_bytes(&rv, 1U);
  }

  template<typename UInt> bool maybe_contains(UInt c) const noexcept {
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

template<> class reader<DIR_BACKWARD> : public basic_reader {
 public:
  reader() = default;
  reader(const reader&) = default;
  reader& operator=(const reader&) = default;

  inline reader(const void* addr, uintptr_t len = UINTPTR_MAX) noexcept
  : basic_reader(reinterpret_cast<uintptr_t>(addr), DIR_BACKWARD, len)
  {}

  inline uint8_t read8(uintptr_t len = UINTPTR_MAX) noexcept {
    auto rv = (masked_addr() == 0 ? update(DIR_BACKWARD, len) : data_);
    --addr_;
    rv = rotate_down_addr(rv, masked_addr());
    return consume_bytes(&rv, 1U);
  }

  template<typename UInt> bool maybe_contains(UInt c) const noexcept {
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


}}} /* namespace __cxxabiv1::ext::<unnamed> */

#endif /* _ABI_EXT_READER_H_ */
