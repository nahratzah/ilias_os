#define _ABI_MISC_INT_INLINE  /* Compile here. */
#include <abi/misc_int.h>
#include <tuple>

namespace __cxxabiv1 {

#if _USE_INT128
_ABI_TYPES(uint128_t) addc128(_ABI_TYPES(uint128_t) x,
                              _ABI_TYPES(uint128_t) y,
                              _ABI_TYPES(uint128_t) carry_in,
                              _ABI_TYPES(uint128_t)* carry_out) noexcept {
  _ABI_TYPES(uint64_t) y_hi = y >> 64,
                       y_lo = y,
                       x_hi = x >> 64,
                       x_lo = x,
                       c_hi = carry_in >> 64,
                       c_lo = carry_in,
                       r_lo,
                       r_hi,
                       c_hi_add,
                       c64_out1,
                       c64_out2,
                       c64_lo,
                       c64_hi;
  r_lo = addc(x_lo, y_lo, c_lo, &c_hi_add);
  c_hi = addc(c_hi, c_hi_add, 0, &c64_out1);
  r_hi = addc(x_hi, y_hi, c_hi, &c64_out2);
  c64_lo = addc(c64_out1, c64_out2, 0, &c64_hi);
  *carry_out = uint128_t(c64_hi) << 64 | c64_lo;
  return uint128_t(r_hi) << 64 | r_lo;
}

_ABI_TYPES(uint128_t) subc128(_ABI_TYPES(uint128_t) x,
                              _ABI_TYPES(uint128_t) y,
                              _ABI_TYPES(uint128_t) carry_in,
                              _ABI_TYPES(uint128_t)* carry_out) noexcept {
  _ABI_TYPES(uint64_t) y_hi = y >> 64,
                       y_lo = y,
                       x_hi = x >> 64,
                       x_lo = x,
                       c_hi = carry_in >> 64,
                       c_lo = carry_in,
                       r_lo,
                       r_hi,
                       c_hi_add,
                       c64_out1,
                       c64_out2,
                       c64_lo,
                       c64_hi;
  r_lo = subc(x_lo, y_lo, c_lo, &c_hi_add);
  c_hi = addc(c_hi, c_hi_add, 0, &c64_out1);
  r_hi = subc(x_hi, y_hi, c_hi, &c64_out2);
  c64_lo = addc(c64_out1, c64_out2, 0, &c64_hi);
  *carry_out = uint128_t(c64_hi) << 64 | c64_lo;
  return uint128_t(r_hi) << 64 | r_lo;
}

bool umul128_overflow(_ABI_TYPES(uint128_t) x,
                      _ABI_TYPES(uint128_t) y,
                      _ABI_TYPES(uint128_t)* result) noexcept {
  _ABI_TYPES(uint128_t) overflow;

  _namespace(std)::tie(*result, overflow) = umul_extend(x, y);
  return overflow != 0;
}
#endif

} /* namespace __cxxabiv1 */
