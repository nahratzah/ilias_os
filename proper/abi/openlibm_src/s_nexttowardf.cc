/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 */

//__FBSDID("$FreeBSD: src/lib/msun/src/s_nexttowardf.c,v 1.3 2011/02/10 07:38:38 das Exp $");

#include <cmath>
#include <cfloat>
#include "private.h"

float nexttowardf(float x, long double y) noexcept {
  static constexpr auto LDBL_INFNAN_EXP = LDBL_MAX_EXP * 2 - 1;

  int32_t hx = get_float_as_int(x);
  int32_t ix = hx & 0x7fffffff;  /* |x| */
  const IEEE_l2bits uy = IEEE_l2bits_from_ldbl(y);

  if ((ix>0x7f800000) ||
      (uy.exp == LDBL_INFNAN_EXP &&
       (masked_nbit_l(uy.manh) | uy.manl) != 0))
    return x + y;  /* x or y is nan */
  if (x == y) return static_cast<float>(y);  /* x=y, return y */
  if (ix == 0) {  /* x == 0 */
    x = set_float_from_int((static_cast<uint32_t>(uy.sign) << 31) | 1);  /* return +-minsubnormal */
    volatile float t = x * x;
    if (t == x)  /* raise underflow flag */
      return t;
    else
      return x;
  }
  if ((hx>=0) ^ (x < y))  /* x -= ulp */
    hx -= 1;
  else  /* x += ulp */
    hx += 1;
  ix = hx & 0x7f800000;
  if (ix >= 0x7f800000) return x + x;  /* overflow  */
  if (ix < 0x00800000) {  /* underflow */
    volatile float t = x * x;
    if (t != x)  /* raise underflow flag */
      return set_float_from_int(hx);
  }
  return set_float_from_int(hx);
}
