/* @(#)s_nextafter.c 5.1 93/09/24 */
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

//__FBSDID("$FreeBSD: src/lib/msun/src/s_nexttoward.c,v 1.3 2011/02/10 07:38:13 das Exp $");

/*
 * We assume that a long double has a 15-bit exponent.  On systems
 * where long double is the same as double, nexttoward() is an alias
 * for nextafter(), so we don't use this routine.
 */

#include <cmath>
#include <cfloat>
#include "private.h"

#if LDBL_MAX_EXP != 0x4000
# error "Unsupported long double format"
#endif

_namespace_begin(std)


double nexttoward(double x, long double y) noexcept {
#if 0
	union IEEEl2bits uy;
	volatile double t;
	int32_t hx,ix;
	u_int32_t lx;
#endif

  int32_t hx;
  uint32_t lx;
  tie(hx, lx) = extract_words(x);
  int32_t ix = hx & 0x7fffffff;  /* |x| */
  const IEEE_l2bits uy = IEEE_l2bits_from_ldbl(y);

  if (((ix >= 0x7ff00000) && ((ix - 0x7ff00000) | lx) != 0) ||
      (uy.exp == 0x7fff &&
       (masked_nbit_l(uy.manh) | uy.manl) != 0))
    return x + y;  /* x or y is nan */
  if (x == y) return static_cast<double>(y);  /* x=y, return y */
  if (x == 0.0) {
    x = set_double_from_words(static_cast<uint32_t>(uy.sign) << 31, 1);  /* return +-minsubnormal */
    volatile double t = x * x;
    if (t == x)  /* raise underflow flag */
      return t;
    else
      return x;
  }
  if ((hx > 0.0) ^ (x < y)) {  /* x -= ulp */
    if (lx == 0) hx -= 1;
    lx -= 1;
  } else {  /* x += ulp */
    lx += 1;
    if (lx == 0) hx += 1;
  }
  ix = hx & 0x7ff00000;
  if (ix >= 0x7ff00000) return x + x;  /* overflow  */
  if (ix < 0x00100000) {  /* underflow */
    volatile double t = x * x;
    if (t != x)  /* raise underflow flag */
      return set_double_from_words(hx,lx);
  }
  return set_double_from_words(hx,lx);
}


_namespace_end(std)
