/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 *
 * From: @(#)s_ceil.c 5.1 93/09/24
 */

//__FBSDID("$FreeBSD: src/lib/msun/src/s_ceill.c,v 1.9 2008/02/14 15:10:33 bde Exp $");

/*
 * ceill(x)
 * Return x rounded toward -inf to integral value
 * Method:
 *	Bit twiddling.
 * Exception:
 *	Inexact flag raised if x not equal to ceill(x).
 */

#include <cmath>
#include <cfloat>
#include <cstdint>
#include "private.h"
#include <abi/misc_int.h>

#ifdef LDBL_IMPLICIT_NBIT
constexpr auto MANH_SIZE = LDBL_MANH_SIZE + 1U;

inline void INC_MANH(IEEE_l2bits& u, uint32_t c) noexcept {
  using abi::addc;

  uint32_t carry;
  u.manh = addc(u.manh, c, 0, &carry);
  if (carry) ++u.exp;
}
#else
constexpr auto MANH_SIZE = LDBL_MANH_SIZE;

inline void INC_MANH(IEEE_l2bits& u, uint32_t c) noexcept {
  using abi::addc;

  uint32_t carry;
  u.manh = addc(u.manh, c, 0, &carry);
  if (carry) {
    ++u.exp;
    u.manh |= static_cast<uint32_t>(1) << (LDBL_MANH_SIZE - 1);
  }
}
#endif

long double ceill(long double x) noexcept {
  static constexpr long double huge = 1.0e300;

  IEEE_l2bits u = IEEE_l2bits_from_ldbl(x);
  const int e = u.exp - LDBL_MAX_EXP + 1;

  if (e < static_cast<int>(MANH_SIZE - 1)) {
    if (e < 0) {  /* raise inexact if x != 0 */
      if (huge + x > 0.0)
        if (u.exp > 0 || (u.manh | u.manl) != 0)
          u = IEEE_l2bits_from_ldbl(u.sign ? -0.0L : 1.0L);
    } else {
      u_int64_t m = ((1llu << MANH_SIZE) - 1) >> (e + 1);
      if (((u.manh & m) | u.manl) == 0) return (x);  /* x is integral */
      if (!u.sign) {
#ifdef LDBL_IMPLICIT_NBIT
        if (e == 0)
          u.exp++;
        else
#endif
          INC_MANH(u, 1llu << (MANH_SIZE - e - 1));
      }
      if (huge + x > 0.0) {  /* raise inexact flag */
        u.manh &= ~m;
	u.manl = 0;
      }
    }
  } else if (e < LDBL_MANT_DIG - 1) {
    u_int64_t m = (u_int64_t)-1 >> (64 - LDBL_MANT_DIG + e + 1);
    if ((u.manl & m) == 0) return (x);  /* x is integral */
    if (!u.sign) {
      if (e == MANH_SIZE - 1) {
        INC_MANH(u, 1);
      } else {
        u_int64_t o = u.manl;
	u.manl += 1llu << (LDBL_MANT_DIG - e - 1);
	if (u.manl < o)  /* got a carry */
          INC_MANH(u, 1);
      }
    }
    if (huge + x > 0.0)  /* raise inexact flag */
      u.manl &= ~m;
  }
  return ldbl_from_IEEE_l2bits(u);
}
