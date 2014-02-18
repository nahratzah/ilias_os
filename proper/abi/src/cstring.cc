#include <abi/ext/reader.h>
#include <abi/errno.h>
#include <cstring>
#include <cstdint>
#include <cstdlib>
#include <cstdio>

_namespace_begin(std)


using ::abi::ext::align_t;
using ::abi::ext::ALIGN;
using ::abi::ext::ALIGN_MSK;
using ::abi::ext::prefetch_w;
using ::abi::ext::WRITE_AHEAD;
using ::abi::ext::reader;
using ::abi::ext::maybe_contains;
using ::abi::ext::consume_bytes;
using ::abi::ext::direction;
using ::abi::ext::DIR_FORWARD;
using ::abi::ext::DIR_BACKWARD;
using ::abi::ext::mask_lowaddr_bytes;
using ::abi::ext::mask_highaddr_bytes;
using ::abi::ext::repeat;

namespace {
constexpr unsigned char CHAR_ZERO = 0;
} /* namespace std::<unnamed> */


#if __has_include(<clocale>)
int strcoll(const char*, const char*) noexcept;  // XXX implement: strcmp based on current locale
int strcoll_l(const char*, const char*, locale_t) noexcept;  // XXX implement: strcmp based on given locale
char* strerror(int e) noexcept;  // XXX return string for errno e in current locale
char* strerror_l(int e, locale_t loc) noexcept;  // XXX return string for errno e in given locale
int strerror_r(int e, char* buf, size_t buflen) noexcept;  // XXX strerror, but with given buffer
char* strsignal(int sig) noexcept;  // XXX return signal name in current locale
size_t strxfrm(char*__restrict a, const char*__restrict b, size_t n) noexcept;  // XXX find current locale, invoke strxfrm_l(..., current_locale())
size_t strxfrm(char*__restrict a, const char*__restrict b, size_t n, locale_t loc) noexcept;  // XXX transform b into a (at most n chars long), such that strcoll_l(b, ..., loc) returns the same result as strcmp(a, ...).  Return length of a, if n was infinite.
#endif /* __has_include(<clocale>) */


size_t strlen(const char* s) noexcept {
  auto r = reader<DIR_FORWARD>(s);

  align_t v;
  while (r.read8() != CHAR_ZERO);
  return r.addr<char>() - 1 - s;
}

int strcmp(const char* a, const char* b) noexcept {
  auto ra = reader<DIR_FORWARD>(a);
  auto rb = reader<DIR_FORWARD>(b);

  for (;;) {
    bool rza = ra.maybe_contains(CHAR_ZERO);
    bool rzb = rb.maybe_contains(CHAR_ZERO);
    if (rza || rzb) {
      /* Handle possible string termination. */
      while (rza || rzb) {
        unsigned char ca = ra.read8();
        unsigned char cb = rb.read8();
        if (ca != cb || ca == '\0') return int(ca) - int(cb);

        rza &= (ra.avail() > 0);
        rzb &= (rb.avail() > 0);
      }
    } else {
      /* Compare in strides of 1 register. */
      align_t va = ra.readN();
      align_t vb = rb.readN();

      if (_predict_false(va != vb || maybe_contains(va, CHAR_ZERO))) {
        /* Find known difference. */
        for (unsigned int i = 0; i < ALIGN; ++i) {
          unsigned char ca = consume_bytes(&va, 1);
          unsigned char cb = consume_bytes(&vb, 1);
          if (ca != cb || ca == '\0') return int(ca) - int(cb);
        }
      }
    } // if (rza || rzb) -- else
  } // for (;;)
}

int memcmp(const void* a, const void* b, size_t len) noexcept {
  if (len == 0) return 0;  // Readers need at least 1 readable byte.
  auto ra = reader<DIR_FORWARD>(a, len);
  auto rb = reader<DIR_FORWARD>(b, len);

  /* Compare in strides of 1 register. */
  for (;;) {
    auto va = ra.readN(len);
    auto vb = rb.readN(len);

    if (va != vb) {
      /* Difference found in register stride. */
      while (len-- > 0) {
        const auto ca = consume_bytes(&va, 1);
        const auto cb = consume_bytes(&vb, 1);
        if (ca != cb) return int(ca) - int(cb);
      }
      return 0;
    }
    if (len < ALIGN) return 0;  // End reached.

    len -= ALIGN;
  }
}

void* memcpy(void*__restrict dst, const void*__restrict src, size_t len)
    noexcept {
  if (len == 0) return dst;  // Reader needs at least 1 readable byte.
  void*const orig_dst = dst;
  auto r = reader<DIR_FORWARD>(src, len);

  /* Read single bytes, until output is aligned to ALIGN. */
  while ((reinterpret_cast<uintptr_t>(dst) & ALIGN_MSK) && len > 0) {
    if (len >= WRITE_AHEAD)
      prefetch_w(static_cast<uint8_t*>(dst) + WRITE_AHEAD);
    *static_cast<uint8_t*>(dst) = r.read8(len);
    dst = static_cast<uint8_t*>(dst) + 1U;
    --len;
  }

  /* Write align_t bytes at a time. */
  while (len >= ALIGN) {
    if (len >= WRITE_AHEAD)
      prefetch_w(static_cast<uint8_t*>(dst) + WRITE_AHEAD);
    *static_cast<align_t*>(dst) = r.readN(len);
    len -= ALIGN;
    dst = static_cast<uint8_t*>(dst) + ALIGN;
  }

  /* Read single bytes, to fill in the remaining bytes in dst. */
  while (len > 0) {
    if (len >= WRITE_AHEAD)
      prefetch_w(static_cast<uint8_t*>(dst) + WRITE_AHEAD);
    *static_cast<uint8_t*>(dst) = r.read8(len);
    dst = static_cast<uint8_t*>(dst) + 1U;
    --len;
  }

  return orig_dst;
}

void* memmove(void* dst, const void* src, size_t len) noexcept {
  if (len == 0 || dst == src) return dst;  // Reader needs at least 1 byte.
  void*const orig_dst = dst;

  if (dst > src && dst < reinterpret_cast<const uint8_t*>(src) + len) {
    /*
     * dst and src overlap in such a way that we need to copy things
     * in reverse order.
     *
     * Position starting point at end of copied range.
     */
    dst = reinterpret_cast<uint8_t*>(dst) + len;
    auto r = reader<DIR_BACKWARD>(reinterpret_cast<const uint8_t*>(src) + len);

    /* Read single bytes, until output is aligned to ALIGN. */
    while ((reinterpret_cast<uintptr_t>(dst) & ALIGN_MSK) && len > 0) {
      dst = static_cast<uint8_t*>(dst) - 1U;
      if (len > WRITE_AHEAD)
        prefetch_w(static_cast<uint8_t*>(dst) - WRITE_AHEAD);
      *static_cast<uint8_t*>(dst) = r.read8(len);
      --len;
    }

    /* Write align_t bytes at a time. */
    while (len > ALIGN) {
      dst = static_cast<uint8_t*>(dst) - ALIGN;
      if (len > WRITE_AHEAD)
        prefetch_w(static_cast<uint8_t*>(dst) - WRITE_AHEAD);
      *static_cast<align_t*>(dst) = r.readN(len);
      len -= ALIGN;
    }

    /* Read single bytes, to fill in the remaining bytes in dst. */
    while (len > 0) {
      dst = static_cast<uint8_t*>(dst) - 1U;
      if (len > WRITE_AHEAD)
        prefetch_w(static_cast<uint8_t*>(dst) - WRITE_AHEAD);
      *static_cast<uint8_t*>(dst) = r.read8(len);
      --len;
    }
  } else {
    /*
     * Use forward copy, since either:
     * - src and dst don't overlap
     * - src and dst overlap, but dst < src
     */
    auto r = reader<DIR_FORWARD>(src);

    /* Read single bytes, until output is aligned to ALIGN. */
    while ((reinterpret_cast<uintptr_t>(dst) & ALIGN_MSK) && len > 0) {
      if (len > WRITE_AHEAD)
        prefetch_w(static_cast<uint8_t*>(dst) + WRITE_AHEAD);
      *static_cast<uint8_t*>(dst) = r.read8(len);
      dst = static_cast<uint8_t*>(dst) + 1U;
      --len;
    }

    /* Write align_t bytes at a time. */
    while (len > ALIGN) {
      if (len > WRITE_AHEAD)
        prefetch_w(static_cast<uint8_t*>(dst) + WRITE_AHEAD);
      *static_cast<align_t*>(dst) = r.readN(len);
      dst = static_cast<uint8_t*>(dst) + ALIGN;
      len -= ALIGN;
    }

    /* Read single bytes, to fill in the remaining bytes in dst. */
    while (len > 0) {
      if (len > WRITE_AHEAD)
        prefetch_w(static_cast<uint8_t*>(dst) - WRITE_AHEAD);
      *static_cast<uint8_t*>(dst) = r.read8(len);
      dst = static_cast<uint8_t*>(dst) + 1U;
      --len;
    }
  }

  return orig_dst;
}

void bcopy(const void* src, void* dst, size_t len) noexcept {
  memmove(dst, src, len);
}

void* memset(void* p, int c, size_t len) noexcept {
  /* Repeating pattern of byte c. */
  const align_t cc = repeat(align_t(c & 0xff), 8);

  uint8_t* i;
  align_t* l;

  /* Memset per byte, until we reach correct alignment for cc. */
  for (i = static_cast<uint8_t*>(p);
       len >= sizeof(*i) && reinterpret_cast<uintptr_t>(i) % ALIGN != 0;
       len -= sizeof(*i)) {
    if (len > WRITE_AHEAD)
      prefetch_w(static_cast<uint8_t*>(i) + WRITE_AHEAD);
    *i++ = c;
  }

  /* Use repeating pattern of cc, fill the memory on a per-register basis. */
  for (l = reinterpret_cast<align_t*>(i);
       len >= ALIGN;
       len -= ALIGN) {
    if (len > WRITE_AHEAD)
      prefetch_w(reinterpret_cast<uint8_t*>(l) + WRITE_AHEAD);
    *l++ = cc;
  }

  /* Set the trailing bytes of p. */
  for (i = reinterpret_cast<uint8_t*>(l);
       len >= sizeof(*i);
       len -= sizeof(*i)) {
    if (len > WRITE_AHEAD)
      prefetch_w(static_cast<uint8_t*>(i) + WRITE_AHEAD);
    *i++ = c;
  }

  return p;
}

void* memchr(const void* p, int c, size_t len) noexcept {
  if (len == 0) return nullptr;  // Reader needs at least 1 readable byte.
  auto r = reader<DIR_FORWARD>(p, len);

  while (len > 0) {
    auto rcc = r.read8(len);
    if (rcc == c) return const_cast<uint8_t*>(r.addr<uint8_t>() - 1);
    --len;
  }
  return nullptr;
}

void* memrchr(const void* p, int c, size_t len) noexcept {
  if (len == 0) return nullptr;  // Reader needs at least 1 readable byte.
  auto r = reader<DIR_BACKWARD>(reinterpret_cast<const uint8_t*>(p) + len,
                                len);

  while (len > 0) {
    auto rcc = r.read8(len);
    if (rcc == c) return const_cast<void*>(r.addr<void>());
    --len;
  }
  return nullptr;
}

void* memccpy(void*__restrict dst, const void*__restrict src, int c,
    size_t len) noexcept {
  if (len == 0) return nullptr;  // Reader needs at least 1 readable byte.
  auto r = reader<DIR_FORWARD>(src, len);

  /* Read single bytes, until output is aligned to ALIGN. */
  while ((reinterpret_cast<uintptr_t>(dst) & ALIGN_MSK) && len > 0) {
    if (len >= WRITE_AHEAD)
      prefetch_w(static_cast<uint8_t*>(dst) + WRITE_AHEAD);
    int rc = *static_cast<uint8_t*>(dst) = r.read8(len);
    dst = static_cast<uint8_t*>(dst) + 1U;
    --len;

    if (rc == c) return dst;
  }

  /* Write align_t bytes at a time. */
  while (len >= ALIGN) {
    if (len >= WRITE_AHEAD)
      prefetch_w(static_cast<uint8_t*>(dst) + WRITE_AHEAD);
    align_t rc = r.readN(len);
    if (_predict_true(!maybe_contains(rc, uint8_t(c)))) {
      *static_cast<align_t*>(dst) = rc;
      len -= ALIGN;
      dst = static_cast<uint8_t*>(dst) + ALIGN;
    } else {
      for (unsigned int i = 0; i < ALIGN; ++i) {
        const auto rcc = consume_bytes(&rc, 1U);
        *static_cast<uint8_t*>(dst) = rcc;
        --len;
        dst = static_cast<uint8_t*>(dst) + 1U;

        if (rcc == c) return dst;
      }
    }
  }

  /* Read single bytes, to fill in the remaining bytes in dst. */
  while (len > 0) {
    if (len >= WRITE_AHEAD)
      prefetch_w(static_cast<uint8_t*>(dst) + WRITE_AHEAD);
    int rc = *static_cast<uint8_t*>(dst) = r.read8(len);
    dst = static_cast<uint8_t*>(dst) + 1U;
    --len;

    if (rc == c) return dst;
  }

  return nullptr;
}

char* stpcpy(char*__restrict dst, const char*__restrict src) noexcept {
  auto r = reader<DIR_FORWARD>(src);

  /* Read single bytes, until output is aligned to ALIGN. */
  while ((reinterpret_cast<uintptr_t>(dst) & ALIGN_MSK)) {
    prefetch_w(dst + WRITE_AHEAD);
    auto c = r.read8();
    *dst = c;
    if (c == CHAR_ZERO) return dst;
    ++dst;
  }

  /* Write align_t bytes at a time. */
  for (;;) {
    prefetch_w(dst + WRITE_AHEAD);

    if (r.maybe_contains(CHAR_ZERO)) {
      /* Copy per byte, taking care not to cross a page-read boundary. */
      for (unsigned int i = 0; i < ALIGN; ++i) {
        auto c = r.read8();
        *dst = c;
        if (c == CHAR_ZERO) return dst;
        ++dst;
      }
    } else {
      /* Handle a register-sized stride. */
      align_t rc = r.readN();

      if (_predict_true(!maybe_contains(rc, CHAR_ZERO))) {
        *reinterpret_cast<align_t*>(dst) = rc;
        dst += ALIGN;
      } else {
        for (unsigned int i = 0; i < ALIGN; ++i) {
          auto c = consume_bytes(&rc, 1);
          *dst = c;
          if (c == CHAR_ZERO) return dst;
          ++dst;
        }
      }
    }
  }
}

char* stpncpy(char*__restrict dst, const char*__restrict src, size_t len)
    noexcept {
  auto r = reader<DIR_FORWARD>(src, len);

  /* Read single bytes, until output is aligned to ALIGN. */
  while ((reinterpret_cast<uintptr_t>(dst) & ALIGN_MSK) && len > 0) {
    if (len >= WRITE_AHEAD) prefetch_w(dst + WRITE_AHEAD);
    auto c = r.read8(len);
    *dst = c;
    if (c == CHAR_ZERO) return dst;
    ++dst;
    --len;
  }

  /* Write align_t bytes at a time. */
  while (len >= ALIGN) {
    if (len >= WRITE_AHEAD)
      prefetch_w(dst + WRITE_AHEAD);

    if (r.maybe_contains(CHAR_ZERO)) {
      for (unsigned int i = 0; i < ALIGN; ++i) {
        auto c = r.read8(len);
        *dst = c;
        if (c == CHAR_ZERO) return dst;
        ++dst;
        --len;
      }
    } else {
      align_t rc = r.readN(len);

      if (_predict_true(!maybe_contains(rc, CHAR_ZERO))) {
        *reinterpret_cast<align_t*>(dst) = rc;
        dst += ALIGN;
      } else {
        auto c = consume_bytes(&rc, 1);
        *dst = c;
        if (c == CHAR_ZERO) return dst;
        ++dst;
      }

      len -= ALIGN;
    }
  }

  /* Read single bytes, to fill in the remaining bytes in dst. */
  while (len > 0) {
    if (len >= WRITE_AHEAD) prefetch_w(dst + WRITE_AHEAD);
    auto c = r.read8(len);
    *dst = c;
    if (c == CHAR_ZERO) return dst;
    ++dst;
    --len;
  }

  return dst;
}

char* strcat(char*__restrict dst, const char*__restrict src) noexcept {
  strcpy(dst + strlen(dst), src);
  return dst;
}

char* strncat(char*__restrict dst, const char*__restrict src, size_t len)
    noexcept {
  size_t dst_len = strnlen(dst, len);
  strlcpy(dst + dst_len, src, len + 1U - dst_len);
  return dst;
}

size_t strlcat(char*__restrict dst, const char*__restrict src, size_t len)
    noexcept {
  size_t dst_len = strlen(dst);
  return dst_len + strlcpy(dst + dst_len, src, len - dst_len);
}

char* strchr(const char* s, int c) noexcept {
  auto r = reader<DIR_FORWARD>(s);

  /* Get reader aligned. */
  for (unsigned int i = 0; i < r.avail(); ++i) {
    auto rcc = r.read8();
    if (rcc == c) return const_cast<char*>(r.addr<char>() - 1);
    if (rcc == CHAR_ZERO) return nullptr;
  }

  /* Handle align_t bytes at a time. */
  for (;;) {
    align_t rc = r.readN();
    if (_predict_true(!maybe_contains(rc, CHAR_ZERO) &&
                      !maybe_contains(rc, uint8_t(c))))
      continue;

    const char* addr = r.addr<char>() - ALIGN;
    for (unsigned int i = 0; i < ALIGN; ++i) {
      auto rcc = consume_bytes(&rc, 1U);
      if (rcc == c) return const_cast<char*>(addr + i);
      if (rcc == CHAR_ZERO) return nullptr;
    }
  }
}

char* strcpy(char*__restrict dst, const char*__restrict src) noexcept {
  stpcpy(dst, src);
  return dst;
}

char* strncpy(char*__restrict dst, const char*__restrict src, size_t len)
    noexcept {
  char* e = stpncpy(dst, src, len);
  memset(e, 0, len - (e - dst));
  return dst;
}

size_t strlcpy(char* dst, const char* src, size_t len) noexcept {
  auto r = reader<DIR_FORWARD>(src);
  size_t rv = 0;

  /* Read single bytes, until output is aligned to ALIGN. */
  while (reinterpret_cast<uintptr_t>(dst) & ALIGN_MSK) {
    auto rc = r.read8(len);
    if (rv == len - 1U)
      *dst = CHAR_ZERO;
    else if (rv < len)
      *dst = rc;
    if (rc == CHAR_ZERO) return rv;
    ++dst;
    ++rv;
  }

  /* Write align_t bytes at a time. */
  while (rv + ALIGN <= len) {
    if (r.maybe_contains(CHAR_ZERO)) {
      for (unsigned int i = 0; i < ALIGN; ++i) {
        auto rcc = r.read8(len);
        *dst++ = rcc;
        if (rcc == CHAR_ZERO) return rv;
        ++rv;
      }
    } else {
      align_t rc = r.readN();

      if (_predict_true(!maybe_contains(rc, CHAR_ZERO))) {
        *reinterpret_cast<align_t*>(dst) = rc;
        dst += ALIGN;
        rv += ALIGN;
      } else {
        for (unsigned int i = 0; i < ALIGN; ++i) {
          auto rcc = consume_bytes(&rc, 1U);
          if (rcc == CHAR_ZERO) return rv;
          ++dst;
          ++rv;
        }
      }
    }
  }

  /* Read remaining bytes from src, first fixing alignment of reader. */
  while (r.avail() > 0) {
    if (r.read8() == CHAR_ZERO) return rv;
    ++rv;
  }

  /* Now read per-register strides of src. */
  for (;;) {
    align_t rc = r.readN();
    if (_predict_true(!maybe_contains(rc, CHAR_ZERO))) {
      rv += ALIGN;
    } else {
      for (unsigned int i = 0; i < ALIGN; ++i) {
        if (consume_bytes(&rv, 1U) == CHAR_ZERO) return rv;
        ++rv;
      }
    }
  }
}

size_t strcspn(const char* s, const char* set) noexcept {
  auto r = reader<DIR_FORWARD>(s);
  size_t rv = 0;

  for (auto c = r.read8();
       c != CHAR_ZERO && !strchr(set, c);
       ++c)
    ++rv;
  return rv;
}

char* strdup(const char* s) noexcept {
#ifdef _TEST  // Figure out why malloc doesn't want to connect properly.
  return nullptr;
#else
  const size_t sz = strlen(s) + 1U;
  void*const dup = malloc(sz);
  if (_predict_false(dup == nullptr)) return nullptr;
  memcpy(dup, s, sz);
  return reinterpret_cast<char*>(dup);
#endif
}

int strncmp(const char* a, const char* b, size_t len) noexcept {
  if (len == 0) return 0;  // Readers need at least 1 readable byte.
  auto ra = reader<DIR_FORWARD>(a, len);
  auto rb = reader<DIR_FORWARD>(b, len);

  for (;;) {
    bool rza = ra.maybe_contains(CHAR_ZERO);
    bool rzb = rb.maybe_contains(CHAR_ZERO);
    if (rza || rzb) {
      /* Read per byte, in order to not read too far past the end. */
      while (rza || rzb) {
        auto ca = ra.read8(len);
        auto cb = rb.read8(len);
        if (ca != cb || ca == CHAR_ZERO) return int(ca) - int(cb);
        if (--len == 0) return 0;

        rza &= (ra.avail() > 0);
        rzb &= (rb.avail() > 0);
      }
    } else {
      /* Read a whole register. */
      auto va = ra.readN(len);
      auto vb = rb.readN(len);

      if (_predict_false(va != vb)) {
        for (unsigned int i = 0; i < ALIGN; ++i) {
          auto ca = consume_bytes(&va, 1);
          auto cb = consume_bytes(&vb, 1);
          if (ca != cb || ca == CHAR_ZERO) return int(ca) - int(cb);
          if (--len == 0) return 0;
        }
      } else if (len < ALIGN) {
        return 0;
      } else {
        len -= ALIGN;
      }
    }
  }
}

char* strndup(const char* s, size_t len) noexcept {
#ifdef _TEST  // Figure out why malloc doesn't want to connect properly.
  return nullptr;
#else
  const size_t sz = strnlen(s, len) + 1U;
  void*const dup = malloc(sz);
  if (_predict_false(dup == nullptr)) return nullptr;
  strlcpy(reinterpret_cast<char*>(dup), s, sz);
  return reinterpret_cast<char*>(dup);
#endif
}

size_t strnlen(const char* s, size_t len) noexcept {
  auto r = reader<DIR_FORWARD>(s);
  const size_t orig_len = len;

  /* Read per byte, to align reader. */
  while (len > 0 && r.avail() > 0) {
    auto c = r.read8(len);
    if (c == CHAR_ZERO) return r.addr<char>() - 1 - s;
  }
  if (len == 0) return orig_len;

  /* Handle align_t bytes at a time. */
  for (;;) {
    align_t rc = r.readN(len);
    if (_predict_true(!maybe_contains(rc, CHAR_ZERO))) {
      if (len < ALIGN) return orig_len;
      len -= ALIGN;
      continue;
    }

    const char* addr = r.addr<char>() - ALIGN;
    for (unsigned int i = 0; len > 0 && i < ALIGN; ++i, --len)
      if (consume_bytes(&rc, 1U) == CHAR_ZERO) return addr + i - s;
    if (len == 0) return orig_len;
  }
}

char* strpbrk(const char* s, const char* set) noexcept {
  auto r = reader<DIR_FORWARD>(s);

  for (;;) {
    auto c = r.read8();
    if (c == CHAR_ZERO) return nullptr;
    if (strchr(set, c)) return const_cast<char*>(r.addr<char>() - 1);
  }
}

char* strrchr(const char* s, int c) noexcept {
  auto r = reader<DIR_FORWARD>(s);
  const char* rv = nullptr;

  for (;;) {
    auto cc = r.read8();
    if (cc == c) rv = r.addr<char>() - 1;
    if (cc == CHAR_ZERO) return const_cast<char*>(rv);
  }
}

size_t strspn(const char* s, const char* set) noexcept {
  auto r = reader<DIR_FORWARD>(s);

  for (auto cc = r.read8();
       cc != CHAR_ZERO && strchr(set, cc);
       cc = r.read8());
  return r.addr<char>() - 1 - s;
}

char* strstr(const char* haystack, const char* needle) noexcept {
  constexpr size_t histogram_sz = UINT8_MAX + 1U;
  ssize_t histogram[histogram_sz];
  size_t needle_len = 0;  // Length of needle, for backtracking.
  unsigned int nz = 0;  // # of non-zero histogram entries.

  /* Fill histogram, by subtracting needle. */
  {
    auto r = reader<DIR_FORWARD>(needle);
    for (auto c = r.read8(); c != CHAR_ZERO; c = r.read8()) {
      ++needle_len;
      if (histogram[c]-- == 0) ++nz;
    }
  }

  /* Pre-read needle_len bytes from haystack. */
  auto head = reader<DIR_FORWARD>(haystack);
  auto tail = head;
  while (needle_len > 0) {
    auto c = head.read8();
    if (c == CHAR_ZERO) return nullptr;
    switch (histogram[c]++) {
    default:
      break;
    case 0:
      ++nz;
      break;
    case -1:
      --nz;
      break;
    }

    --needle_len;
  }

  /* Walk window, described by [tail - head) through histogram. */
  for (;;) {
    /* GUARD: tail points at string equal to needle. */
    if (nz == 0 &&
        memcmp(tail.addr<void>(), needle,
               head.addr<uint8_t>() - tail.addr<uint8_t>()) == 0)
      return const_cast<char*>(tail.addr<char>());

    const auto head_c = head.read8();
    if (head_c == CHAR_ZERO) return nullptr;
    const auto tail_c = tail.read8();

    /* Add head character to histogram. */
    switch (histogram[head_c]++) {
    default:
      break;
    case 0:
      ++nz;
      break;
    case -1:
      --nz;
      break;
    }

    /* Remove tail character from histogram. */
    switch (histogram[tail_c]--) {
    default:
      break;
    case 0:
      ++nz;
      break;
    case 1:
      --nz;
      break;
    }
  }
}

char* strtok(char*__restrict s, const char*__restrict sep) noexcept {
#ifdef _TEST
  static char* last_s;
#else
  thread_local char* last_s;
#endif
  return strtok_r(s, sep, &last_s);
}

char* strtok_r(char*__restrict s, const char*__restrict sep,
    char**__restrict last_s) noexcept {
  if (s == nullptr) s = *last_s;
  if (*s == '\0') return nullptr;

  s += strspn(s, sep);  // Skip tokens in separators at start of string.
  char* e = s + strcspn(s, sep);  // Search until separators.
  if (*e == '\0') {
    *last_s = e;
  } else {
    *e = '\0';
    *last_s = e + 1;
  }
  return s;
}

int strerror_r(int errnum, char* buf, size_t buflen) noexcept {
  using abi::sys_nerr;
  using abi::sys_errlist;

#if __has_include(<clocale>)
  ...
#else /* __has_include(<clocale>) */
  if (_predict_false(errnum < 0 || errnum >= sys_nerr)) {
    snprintf(buf, buflen, "Unknown error: %d", errnum);
    return _ABI_EINVAL;
  }
  return (strlcpy(buf, sys_errlist[errnum], buflen) >= buflen ?
          _ABI_ERANGE :
          0);
#endif /* __has_include(<clocale>) ... else */
}

char* strerror(int errnum) noexcept {
  thread_local char buf[32];
  strerror_r(errnum, buf, sizeof(buf) / sizeof(buf[0]));
  return buf;
}


_namespace_end(std)
