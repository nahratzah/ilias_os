#ifndef _OPENLIBM_SRC_PRIVATE_H_
#define _OPENLIBM_SRC_PRIVATE_H_

#include <cdecl.h>
#include <cstdint>
#include <tuple>

namespace {


#if defined(__amd64__) || defined(__x86_64__)
# define HAVE_EFFICIENT_IRINT 1

inline int irint(double x) noexcept {
  int n;

  __asm__("cvtsd2si %1,%0" : "=r" (n) : "x" (x));
  return n;
}
#endif

#if defined(__i386__)
# define HAVE_EFFICIENT_IRINT 1

inline int irint(double x) noexcept {
  int n;

  __asm__("fistl %0" : "=m" (n) : "t" (x));
  return n;
}
#endif

inline uint32_t get_float_as_int(float f) noexcept {
  static_assert(sizeof(uint32_t) == sizeof(float),
                "sizeof(float) == sizeof(uint32_t)");
  uint32_t rv;
  __builtin_memcpy(&rv, &f, sizeof(f));
  return rv;
}

inline float set_float_from_int(uint32_t v) noexcept {
  static_assert(sizeof(uint32_t) == sizeof(float),
                "sizeof(float) == sizeof(uint32_t)");
  uint32_t f;
  __builtin_memcpy(&f, &v, sizeof(f));
  return f;
}

inline float mask_float(float f, uint32_t mask) noexcept {
  return set_float_from_int(get_float_as_int(f) & mask);
}


inline _namespace(std)::tuple<uint32_t, uint32_t> extract_words(double d)
    noexcept {
  using _namespace(std)::make_tuple;

  static_assert(2 * sizeof(uint32_t) == sizeof(double),
                "sizeof(float) == 2 * sizeof(uint32_t)");
  uint32_t i[2];
  __builtin_memcpy(i, &d, sizeof(double));
#if __BIG_ENDIAN__
  return make_tuple(i[0], i[1]);
#elif __LITTLE_ENDIAN__
  return make_tuple(i[1], i[0]);
#endif
}

inline double set_double_from_words(uint32_t m, uint32_t l) noexcept {
  static_assert(2 * sizeof(uint32_t) == sizeof(double),
                "sizeof(float) == 2 * sizeof(uint32_t)");
  uint32_t i[2];
#if __BIG_ENDIAN__
  i[0] = m;
  i[1] = l;
#elif __LITTLE_ENDIAN__
  i[1] = m;
  i[0] = l;
#endif

  double d;
  __builtin_memcpy(&d, i, sizeof(double));
  return d;
}

inline double set_low_word(double d, uint32_t v) noexcept {
  using _namespace(std)::get;

  return set_double_from_words(get<0>(extract_words(d)), v);
}

inline double set_high_word(double d, uint32_t v) noexcept {
  using _namespace(std)::get;

  return set_double_from_words(v, get<1>(extract_words(d)));
}


struct IEEE_f2bits {
#if __BIG_ENDIAN__
  unsigned int sign  :  1;
  unsigned int exp   :  8;
  unsigned int man   : 23;
#elif __LITTLE_ENDIAN__
  unsigned int man   : 23;
  unsigned int exp   :  8;
  unsigned int sign  :  1;
#endif
};

struct IEEE_d2bits {
#if __BIG_ENDIAN__
  unsigned int sign  :  1;
  unsigned int exp   : 11;
  unsigned int manh  : 20;
  unsigned int manl  : 32;
#elif __LITTLE_ENDIAN__
  unsigned int manl  : 32;
  unsigned int manh  : 20;
  unsigned int exp   : 11;
  unsigned int sign  :  1;
#endif
};

#if defined(__amd64__) || defined(__x86_64__) || defined(__i386__)
constexpr unsigned int LDBL_MANH_SIZE = 32;

struct IEEE_l2bits {
  unsigned int manl  : 32;
  unsigned int manh  : 32;
  unsigned int exp   : 15;
  unsigned int sign  :  1;
  unsigned int junkl : 16;
#if defined(__amd64__) || defined(__x86_64__)
  unsigned int junkh : 32;
#endif
};
#endif

#if defined(__amd64__) || defined(__x86_64__) || defined(__i386__)
struct IEEE_l2xbits {
  int64_t man        : 64;
  int64_t expsign    : 16;
#if defined(__amd64__) || defined(__x86_64__)
  int64_t junk       : 48;
#else
  int64_t junk       : 16;
#endif
};
#endif

inline IEEE_f2bits IEEE_f2bits_from_flt(float x) noexcept {
  static_assert(sizeof(IEEE_f2bits) >= sizeof(float),
                "sizeof(IEEE_f2bits) >= sizeof(float)");
  IEEE_f2bits rv;
  __builtin_memcpy(&rv, &x, sizeof(x));
  return rv;
}

inline float flt_from_IEEE_f2bits(IEEE_f2bits v) noexcept {
  static_assert(sizeof(IEEE_f2bits) >= sizeof(float),
                "sizeof(IEEE_f2bits) >= sizeof(float)");
  float rv;
  __builtin_memcpy(&rv, &v, sizeof(rv));
  return rv;
}

inline IEEE_d2bits IEEE_d2bits_from_dbl(double x) noexcept {
  static_assert(sizeof(IEEE_d2bits) >= sizeof(double),
                "sizeof(IEEE_d2bits) >= sizeof(double)");
  IEEE_d2bits rv;
  __builtin_memcpy(&rv, &x, sizeof(x));
  return rv;
}

inline double dbl_from_IEEE_d2bits(IEEE_d2bits v) noexcept {
  static_assert(sizeof(IEEE_d2bits) >= sizeof(double),
                "sizeof(IEEE_d2bits) >= sizeof(double)");
  double rv;
  __builtin_memcpy(&rv, &v, sizeof(rv));
  return rv;
}

inline IEEE_l2bits IEEE_l2bits_from_ldbl(long double x) noexcept {
  static_assert(sizeof(IEEE_l2bits) >= sizeof(long double),
                "sizeof(IEEE_l2bits) >= sizeof(long double)");
  IEEE_l2bits rv;
  __builtin_memcpy(&rv, &x, sizeof(x));
  return rv;
}

inline long double ldbl_from_IEEE_l2bits(IEEE_l2bits v) noexcept {
  static_assert(sizeof(IEEE_l2bits) >= sizeof(long double),
                "sizeof(IEEE_l2bits) >= sizeof(long double)");
  long double rv;
  __builtin_memcpy(&rv, &v, sizeof(rv));
  return rv;
}

inline IEEE_l2xbits IEEE_l2xbits_from_ldbl(long double x) noexcept {
  static_assert(sizeof(IEEE_l2xbits) >= sizeof(long double),
                "sizeof(IEEE_l2xbits) >= sizeof(long double)");
  IEEE_l2xbits rv;
  __builtin_memcpy(&rv, &x, sizeof(x));
  return rv;
}

inline long double ldbl_from_IEEE_l2xbits(IEEE_l2xbits v) noexcept {
  static_assert(sizeof(IEEE_l2xbits) >= sizeof(long double),
                "sizeof(IEEE_l2xbits) >= sizeof(long double)");
  long double rv;
  __builtin_memcpy(&rv, &v, sizeof(rv));
  return rv;
}

#if defined(__i386__) || defined(__amd64__) || defined(__x86_64__)
inline void mask_nbit_l(IEEE_l2bits& v) noexcept {
  constexpr unsigned int LDBL_NBIT = 0x80000000;
  v.manh &= ~LDBL_NBIT;
}

inline void mask_nbit_l(IEEE_l2xbits& v) noexcept {
  constexpr uint64_t LDBL_NBIT = 0x8000000000000000LL;
  v.man &= ~LDBL_NBIT;
}
#endif /* defined(__i386__) || defined(__amd64__) || defined(__x86_64__) */


} /* namespace <unnamed> */

_namespace_begin(std)
namespace impl {

int __ieee754_rem_pio2(double, double*) noexcept;
int __ieee754_rem_pio2f(float, double*) noexcept;
int __ieee754_rem_pio2l(long double, long double*) noexcept;
int __kernel_rem_pio2(double*, double*, int, int, int) noexcept;

double __kernel_sin(double, double, int) noexcept;
double __kernel_cos(double, double) noexcept;
float __kernel_sindf(double) noexcept;
float __kernel_cosdf(double) noexcept;
long double __kernel_sinl(long double, long double, int) noexcept;
long double __kernel_cosl(long double, long double) noexcept;

} /* namespace std::impl */
_namespace_end(std)

#endif /* _OPENLIBM_SRC_PRIVATE_H_ */
