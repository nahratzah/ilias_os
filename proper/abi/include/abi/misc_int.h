#ifndef _ABI_MISC_INT_H_
#define _ABI_MISC_INT_H_

#include <cdecl.h>
#include <abi/abi.h>
#ifdef __cplusplus
# include <tuple>
# include <type_traits>
#endif
#include <climits>

#ifdef __cplusplus
namespace __cxxabiv1 {
#endif /* __cplusplus */

inline unsigned int addc(unsigned int x, unsigned int y,
                         unsigned int carry_in,
                         unsigned int* carry_out) noexcept {
  return __builtin_addc(x, y, carry_in, carry_out);
}

inline unsigned char addcb(unsigned char x, unsigned char y,
                           unsigned char carry_in,
                           unsigned char* carry_out) noexcept {
  return __builtin_addcb(x, y, carry_in, carry_out);
}

inline unsigned short addcs(unsigned short x, unsigned short y,
                            unsigned short carry_in,
                            unsigned short* carry_out) noexcept {
  return __builtin_addcs(x, y, carry_in, carry_out);
}

inline unsigned long addcl(unsigned long x, unsigned long y,
                           unsigned long carry_in,
                           unsigned long* carry_out) noexcept {
  return __builtin_addcl(x, y, carry_in, carry_out);
}

inline unsigned long long addcll(unsigned long long x, unsigned long long y,
                                 unsigned long long carry_in,
                                 unsigned long long* carry_out) noexcept {
  return __builtin_addcll(x, y, carry_in, carry_out);
}

#if _USE_INT128
_ABI_TYPES(uint128_t) addc128(_ABI_TYPES(uint128_t) x, _ABI_TYPES(uint128_t) y,
                              _ABI_TYPES(uint128_t) carry_in,
                              _ABI_TYPES(uint128_t)* carry_out) noexcept;
#endif

#ifdef __cplusplus

inline unsigned char addc(unsigned char x, unsigned char y,
                          unsigned char carry_in,
                          unsigned char* carry_out) noexcept {
  return __builtin_addcb(x, y, carry_in, carry_out);
}

inline unsigned short addc(unsigned short x, unsigned short y,
                           unsigned short carry_in,
                           unsigned short* carry_out) noexcept {
  return __builtin_addcs(x, y, carry_in, carry_out);
}

inline unsigned long addc(unsigned long x, unsigned long y,
                          unsigned long carry_in,
                          unsigned long* carry_out) noexcept {
  return __builtin_addcl(x, y, carry_in, carry_out);
}

inline unsigned long long addc(unsigned long long x, unsigned long long y,
                               unsigned long long carry_in,
                               unsigned long long* carry_out) noexcept {
  return __builtin_addcll(x, y, carry_in, carry_out);
}

# if _USE_INT128
inline _ABI_TYPES(uint128_t) addc(_ABI_TYPES(uint128_t) x,
                                  _ABI_TYPES(uint128_t) y,
                                  _ABI_TYPES(uint128_t) carry_in,
                                  _ABI_TYPES(uint128_t)* carry_out) noexcept {
  return addc128(x, y, carry_in, carry_out);
}
# endif


template<typename T, typename = _namespace(std)::enable_if_t<
    _namespace(std)::is_integral<T>::value &&
    _namespace(std)::is_unsigned<T>::value, void>>
_namespace(std)::tuple<T, T> addc(T x, T y, T carry_in = 0) noexcept {
  T carry_out;
  T result = addc(x, y, carry_in, &carry_out);
  return _namespace(std)::make_tuple(result, carry_out);
}

#endif /* __cplusplus */


inline unsigned int subc(unsigned int x, unsigned int y,
                         unsigned int carry_in,
                         unsigned int* carry_out) noexcept {
  return __builtin_subc(x, y, carry_in, carry_out);
}

inline unsigned char subcb(unsigned char x, unsigned char y,
                           unsigned char carry_in,
                           unsigned char* carry_out) noexcept {
  return __builtin_subcb(x, y, carry_in, carry_out);
}

inline unsigned short subcs(unsigned short x, unsigned short y,
                            unsigned short carry_in,
                            unsigned short* carry_out) noexcept {
  return __builtin_subcs(x, y, carry_in, carry_out);
}

inline unsigned long subcl(unsigned long x, unsigned long y,
                           unsigned long carry_in,
                           unsigned long* carry_out) noexcept {
  return __builtin_subcl(x, y, carry_in, carry_out);
}

inline unsigned long long subcll(unsigned long long x, unsigned long long y,
                                 unsigned long long carry_in,
                                 unsigned long long* carry_out) noexcept {
  return __builtin_subcll(x, y, carry_in, carry_out);
}

#if _USE_INT128
_ABI_TYPES(uint128_t) subc128(_ABI_TYPES(uint128_t) x, _ABI_TYPES(uint128_t) y,
                              _ABI_TYPES(uint128_t) carry_in,
                              _ABI_TYPES(uint128_t)* carry_out) noexcept;
#endif

#ifdef __cplusplus

inline unsigned char subc(unsigned char x, unsigned char y,
                          unsigned char carry_in,
                          unsigned char* carry_out) noexcept {
  return __builtin_subcb(x, y, carry_in, carry_out);
}

inline unsigned short subc(unsigned short x, unsigned short y,
                           unsigned short carry_in,
                           unsigned short* carry_out) noexcept {
  return __builtin_subcs(x, y, carry_in, carry_out);
}

inline unsigned long subc(unsigned long x, unsigned long y,
                          unsigned long carry_in,
                          unsigned long* carry_out) noexcept {
  return __builtin_subcl(x, y, carry_in, carry_out);
}

inline unsigned long long subc(unsigned long long x, unsigned long long y,
                               unsigned long long carry_in,
                               unsigned long long* carry_out) noexcept {
  return __builtin_subcll(x, y, carry_in, carry_out);
}

# if _USE_INT128
inline _ABI_TYPES(uint128_t) subc(_ABI_TYPES(uint128_t) x,
                                  _ABI_TYPES(uint128_t) y,
                                  _ABI_TYPES(uint128_t) carry_in,
                                  _ABI_TYPES(uint128_t)* carry_out) noexcept {
  return subc128(x, y, carry_in, carry_out);
}
# endif


template<typename T, typename = _namespace(std)::enable_if_t<
    _namespace(std)::is_integral<T>::value &&
    _namespace(std)::is_unsigned<T>::value, void>>
_namespace(std)::tuple<T, T> subc(T x, T y, T carry_in = 0) noexcept {
  T carry_out;
  T result = subc(x, y, carry_in, &carry_out);
  return _namespace(std)::make_tuple(result, carry_out);
}

#endif /* __cplusplus */


inline bool umulb_overflow(unsigned char x, unsigned char y,
                           unsigned char* result) noexcept {
  using int_t = unsigned int;
  int_t m = int_t(x) * int_t(y);
  *result = m;
  return (m >= (1U << (sizeof(unsigned char) * CHAR_BIT)));
}

inline bool umuls_overflow(unsigned short x, unsigned short y,
                           unsigned short* result) noexcept {
  using int_t = unsigned int;
  int_t m = int_t(x) * int_t(y);
  *result = m;
  return (m >= (1U << (sizeof(unsigned short) * CHAR_BIT)));
}

inline bool umul_overflow(unsigned int x, unsigned int y,
                          unsigned int* result) noexcept {
  return __builtin_umul_overflow(x, y, result);
}

inline bool umull_overflow(unsigned long x, unsigned long y,
                           unsigned long* result) noexcept {
  return __builtin_umull_overflow(x, y, result);
}

inline bool umulll_overflow(unsigned long long x, unsigned long long y,
                            unsigned long long* result) noexcept {
  return __builtin_umulll_overflow(x, y, result);
}

#if _USE_INT128
bool umul128_overflow(_ABI_TYPES(uint128_t) x, _ABI_TYPES(uint128_t) y,
                      _ABI_TYPES(uint128_t)* result) noexcept;
#endif

#ifdef __cplusplus

inline bool umul_overflow(unsigned char x, unsigned char y,
                          unsigned char* result) noexcept {
  return umulb_overflow(x, y, result);
}

inline bool umul_overflow(unsigned short x, unsigned short y,
                          unsigned short* result) noexcept {
  return umuls_overflow(x, y, result);
}

inline bool umul_overflow(unsigned long x, unsigned long y,
                          unsigned long* result) noexcept {
  return __builtin_umull_overflow(x, y, result);
}

inline bool umul_overflow(unsigned long long x, unsigned long long y,
                          unsigned long long* result) noexcept {
  return __builtin_umulll_overflow(x, y, result);
}

#if _USE_INT128
inline bool umul_overflow(_ABI_TYPES(uint128_t) x, _ABI_TYPES(uint128_t) y,
                          _ABI_TYPES(uint128_t)* result) noexcept {
  return umul128_overflow(x, y, result);
}
#endif


template<typename T, typename = _namespace(std)::enable_if_t<
    _namespace(std)::is_integral<T>::value &&
    _namespace(std)::is_unsigned<T>::value, void>>
_namespace(std)::tuple<T, bool> umul_overflow(T x, T y) noexcept {
  T result;
  bool overflowed = umul_overflow(x, y, &result);
  return _namespace(std)::make_tuple(result, overflowed);
}

#endif /* __cplusplus */


_cdecl_begin

unsigned char umulb_extend(unsigned char, unsigned char,
                           unsigned char*) noexcept;
unsigned short umuls_extend(unsigned short, unsigned short,
                            unsigned short*) noexcept;
unsigned int umul_extend(unsigned int, unsigned int,
                         unsigned int*) noexcept;
unsigned long umull_extend(unsigned long, unsigned long,
                           unsigned long*) noexcept;
unsigned long long umulll_extend(unsigned long long, unsigned long long,
                                 unsigned long long*) noexcept;

_cdecl_end

#ifdef __cplusplus

inline unsigned char umul_extend(unsigned char x, unsigned char y,
                                 unsigned char* hi) noexcept {
  using uint = unsigned int;

  uint result = uint(x) * uint(y);
  *hi = result >> (sizeof(unsigned char) * CHAR_BIT);
  return result;
}

inline unsigned short umul_extend(unsigned short x, unsigned short y,
                                  unsigned short* hi) noexcept {
  using uint = unsigned int;

  uint result = uint(x) * uint(y);
  *hi = result >> (sizeof(unsigned short) * CHAR_BIT);
  return result;
}

template<typename T, typename = _namespace(std)::enable_if_t<
    _namespace(std)::is_integral<T>::value &&
    _namespace(std)::is_unsigned<T>::value, void>>
_namespace(std)::tuple<T, T> umul_extend(T x, T y) noexcept {
  using _namespace(std)::tie;
  using _namespace(std)::make_tuple;

  constexpr unsigned int bits = sizeof(T) * CHAR_BIT;
  constexpr unsigned int hbits = bits / 2;
  constexpr T lo_mask = (T(1) << hbits) - 1U;

  const T x_lo = x & lo_mask;
  const T y_lo = y & lo_mask;
  const T x_hi = x >> hbits;
  const T y_hi = y >> hbits;

  T bottom = x_lo * y_lo;
  T top = x_hi * y_hi;
  const T mid1 = x_lo * y_hi;
  const T mid2 = x_hi * y_lo;

  T carry = T();
  tie(bottom, carry) = addc(bottom, mid1 << hbits);
  top += carry;
  tie(bottom, carry) = addc(bottom, mid2 << hbits);
  top += carry;
  top += (mid1 >> hbits);
  top += (mid2 >> hbits);
  return make_tuple(bottom, top);
}

inline unsigned long umul_extend(unsigned long x, unsigned long y,
                                 unsigned long* hi) noexcept {
  using _namespace(std)::tie;

  unsigned long result;
  tie(result, *hi) = umul_extend(x, y);
  return result;
}

inline unsigned long long umul_extend(unsigned long long x,
                                      unsigned long long y,
                                      unsigned long long* hi) noexcept {
  using _namespace(std)::tie;

  unsigned long long result;
  tie(result, *hi) = umul_extend(x, y);
  return result;
}

# if _USE_INT128
inline _ABI_TYPES(uint128_t) umul_extend(_ABI_TYPES(uint128_t) x,
                                         _ABI_TYPES(uint128_t) y,
                                         _ABI_TYPES(uint128_t)* hi) noexcept {
  using _namespace(std)::tie;

  _ABI_TYPES(uint128_t) result;
  tie(result, *hi) = umul_extend(x, y);
  return result;
}
# endif

#ifndef _ABI_MISC_INT_INLINE
# define _ABI_MISC_INT_INLINE extern inline
#endif

_ABI_MISC_INT_INLINE
unsigned char umulb_extend(unsigned char x, unsigned char y,
                           unsigned char* hi) noexcept {
  return umul_extend(x, y, hi);
}

_ABI_MISC_INT_INLINE
unsigned short umuls_extend(unsigned short x, unsigned short y,
                            unsigned short* hi) noexcept {
  return umul_extend(x, y, hi);
}

_ABI_MISC_INT_INLINE
unsigned int umul_extend(unsigned int x, unsigned int y,
                         unsigned int* hi) noexcept {
  using _namespace(std)::tie;

  unsigned int result;
  tie(result, *hi) = umul_extend(x, y);
  return result;
}


_ABI_MISC_INT_INLINE
unsigned long umull_extend(unsigned long x, unsigned long y,
                           unsigned long* hi) noexcept {
  return umul_extend(x, y, hi);
}

_ABI_MISC_INT_INLINE
unsigned long long umulll_extend(unsigned long long x, unsigned long long y,
                                 unsigned long long* hi) noexcept {
  return umul_extend(x, y, hi);
}

#undef _ABI_MISC_INT_INLINE

#endif /* __cplusplus */


constexpr int ctzb(unsigned char x) noexcept {
  return (x == 0 ? sizeof(unsigned char) * CHAR_BIT : __builtin_ctz(x));
}

constexpr int ctzs(unsigned short x) noexcept {
  return (x == 0 ? sizeof(unsigned short) * CHAR_BIT : __builtin_ctz(x));
}

constexpr int ctz(unsigned int x) noexcept {
  return (x == 0 ? sizeof(unsigned int) * CHAR_BIT : __builtin_ctz(x));
}

constexpr int ctzl(unsigned long x) noexcept {
  return (x == 0 ? sizeof(unsigned long) * CHAR_BIT : __builtin_ctzl(x));
}

constexpr int ctzll(unsigned long long x) noexcept {
  return (x == 0 ? sizeof(unsigned long long) * CHAR_BIT : __builtin_ctzll(x));
}

#ifdef __cplusplus

constexpr int ctz(int x) noexcept {
  return (x == 0 ? sizeof(int) * CHAR_BIT : __builtin_ctz(x));
}

constexpr int ctz(unsigned char x) noexcept {
  return (x == 0 ? sizeof(unsigned char) * CHAR_BIT : __builtin_ctz(x));
}

constexpr int ctz(signed char x) noexcept {
  return (x == 0 ? sizeof(signed char) * CHAR_BIT : __builtin_ctz(x));
}

constexpr int ctz(char x) noexcept {
  return (x == 0 ? sizeof(char) * CHAR_BIT : __builtin_ctz(x));
}

constexpr int ctz(unsigned short x) noexcept {
  return (x == 0 ? sizeof(unsigned short) * CHAR_BIT : __builtin_ctz(x));
}

constexpr int ctz(short x) noexcept {
  return (x == 0 ? sizeof(short) * CHAR_BIT : __builtin_ctz(x));
}

constexpr int ctz(unsigned long x) noexcept {
  return (x == 0 ? sizeof(unsigned long) * CHAR_BIT : __builtin_ctzl(x));
}

constexpr int ctz(long x) noexcept {
  return (x == 0 ? sizeof(long) * CHAR_BIT : __builtin_ctzl(x));
}

constexpr int ctz(unsigned long long x) noexcept {
  return (x == 0 ? sizeof(unsigned long long) * CHAR_BIT : __builtin_ctzll(x));
}

constexpr int ctz(long long x) noexcept {
  return (x == 0 ? sizeof(long long) * CHAR_BIT : __builtin_ctzll(x));
}

#endif /* __cplusplus */


constexpr int clzb(unsigned char x) noexcept {
  /* clz counts for int, so we need to compensate... */
  return (x == 0 ?
          sizeof(unsigned char) * CHAR_BIT :
          __builtin_clz(x) -
          (sizeof(unsigned int) - sizeof(unsigned char)) * CHAR_BIT);
}

constexpr int clzs(unsigned short x) noexcept {
  /* clz counts for int, so we need to compensate... */
  return (x == 0 ?
          sizeof(unsigned short) * CHAR_BIT :
          __builtin_clz(x) -
          (sizeof(unsigned int) - sizeof(unsigned short)) * CHAR_BIT);
}

constexpr int clz(unsigned int x) noexcept {
  return (x == 0 ? sizeof(unsigned int) * CHAR_BIT : __builtin_clz(x));
}

constexpr int clzl(unsigned long x) noexcept {
  return (x == 0 ? sizeof(unsigned long) * CHAR_BIT : __builtin_clzl(x));
}

constexpr int clzll(unsigned long long x) noexcept {
  return (x == 0 ? sizeof(unsigned long long) * CHAR_BIT : __builtin_clzll(x));
}

#ifdef __cplusplus

constexpr int clz(int x) noexcept {
  return (x == 0 ? sizeof(int) * CHAR_BIT : __builtin_clz(x));
}

constexpr int clz(unsigned char x) noexcept {
  /* clz counts for int, so we need to compensate... */
  return (x == 0 ?
          sizeof(unsigned char) * CHAR_BIT :
          __builtin_clz(x) -
          (sizeof(unsigned int) - sizeof(unsigned char)) * CHAR_BIT);
}

constexpr int clz(signed char x) noexcept {
  /* clz counts for int, so we need to compensate... */
  return (x == 0 ?
          sizeof(signed char) * CHAR_BIT :
          __builtin_clz(x) -
          (sizeof(int) - sizeof(signed char)) * CHAR_BIT);
}

constexpr int clz(unsigned short x) noexcept {
  /* clz counts for int, so we need to compensate... */
  return (x == 0 ?
          sizeof(unsigned short) * CHAR_BIT :
          __builtin_clz(x) -
          (sizeof(unsigned int) - sizeof(unsigned short)) * CHAR_BIT);
}

constexpr int clz(short x) noexcept {
  /* clz counts for int, so we need to compensate... */
  return (x == 0 ?
          sizeof(short) * CHAR_BIT :
          __builtin_clz(x) -
          (sizeof(int) - sizeof(short)) * CHAR_BIT);
}

constexpr int clz(unsigned long x) noexcept {
  return (x == 0 ? sizeof(unsigned long) * CHAR_BIT : __builtin_clzl(x));
}

constexpr int clz(long x) noexcept {
  return (x == 0 ? sizeof(long) * CHAR_BIT : __builtin_clzl(x));
}

constexpr int clz(unsigned long long x) noexcept {
  return (x == 0 ? sizeof(unsigned long long) * CHAR_BIT : __builtin_clzll(x));
}

constexpr int clz(long long x) noexcept {
  return (x == 0 ? sizeof(long long) * CHAR_BIT : __builtin_clzll(x));
}

#endif /* __cplusplus */


#ifdef __cplusplus
} /* namespace __cxxabiv1 */
#endif /* __cplusplus */


#ifdef __cplusplus
using abi::addcb;
using abi::addcs;
using abi::addc;
using abi::addcl;
using abi::addcll;

using abi::subcb;
using abi::subcs;
using abi::subc;
using abi::subcl;
using abi::subcll;

using abi::umulb_overflow;
using abi::umuls_overflow;
using abi::umul_overflow;
using abi::umull_overflow;
using abi::umulll_overflow;

using abi::umulb_extend;
using abi::umuls_extend;
using abi::umul_extend;
using abi::umull_extend;
using abi::umulll_extend;

using abi::ctzb;
using abi::ctzs;
using abi::ctz;
using abi::ctzl;
using abi::ctzll;

using abi::clzb;
using abi::clzs;
using abi::clz;
using abi::clzl;
using abi::clzll;
#endif

#endif /* _ABI_MISC_INT_H_ */
