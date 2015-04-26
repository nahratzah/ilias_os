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

//__FBSDID("$FreeBSD: src/lib/msun/src/s_nextafterl.c,v 1.2 2008/02/22 02:30:36 das Exp $");

/* IEEE functions
 *	nextafter(x,y)
 *	return the next machine floating-point number of x in the
 *	direction toward y.
 *   Special cases:
 */

#include <cmath>
#include <cfloat>
#include "private.h"

#if LDBL_MAX_EXP != 0x4000
#error "Unsupported long double format"
#endif

long double nextafterl(long double x, long double y) noexcept {
#if 0
	volatile long double t;
	union IEEEl2bits ux, uy;
#endif

  IEEE_l2bits ux = IEEE_l2bits_from_ldbl(x);
  IEEE_l2bits uy = IEEE_l2bits_from_ldbl(y);

  if ((ux.exp == 0x7fff && (masked_nbit_l(ux.manh) | ux.manl) != 0) ||
      (uy.exp == 0x7fff && (masked_nbit_l(uy.manh) | uy.manl) != 0))
    return x + y;  /* x or y is nan */
  if (x == y) return y;  /* x=y, return y */
  if (x == 0.0) {
    ux.manh = 0;  /* return +-minsubnormal */
    ux.manl = 1;
    ux.sign = uy.sign;
    volatile long double t = ldbl_from_IEEE_l2bits(ux) *
                             ldbl_from_IEEE_l2bits(ux);
    if (t == ldbl_from_IEEE_l2bits(ux))  /* raise underflow flag */
      return t;
    else
      return ldbl_from_IEEE_l2bits(ux);
  }
  if ((x > 0.0) ^ (x < y)) {  /* x -= ulp */
    if(ux.manl == 0) {
      if (masked_nbit_l(ux.manh) == 0) ux.exp -= 1;
      ux.manh = (ux.manh - 1) | (ux.manh & 0x80000000U);
    }
    ux.manl -= 1;
  } else {  /* x += ulp */
    ux.manl += 1;
    if (ux.manl == 0) {
      ux.manh = (ux.manh + 1) | (ux.manh & 0x80000000);
      if (masked_nbit_l(ux.manh) == 0) ux.exp += 1;
    }
  }
  if (ux.exp == 0x7fff) return x + x;  /* overflow  */
  if (ux.exp == 0) {  /* underflow */
    mask_nbit_l(ux);
    volatile long double t = ldbl_from_IEEE_l2bits(ux) *
                             ldbl_from_IEEE_l2bits(ux);
    if (t != ldbl_from_IEEE_l2bits(ux))  /* raise underflow flag */
      return ldbl_from_IEEE_l2bits(ux);
  }
  return ldbl_from_IEEE_l2bits(ux);
}
