/* @(#)e_asin.c 1.3 95/01/18 */
/* FreeBSD: head/lib/msun/src/e_asin.c 176451 2008-02-22 02:30:36Z das */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunSoft, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 */

//__FBSDID("$FreeBSD: src/lib/msun/src/e_asinl.c,v 1.2 2008/08/03 17:49:05 das Exp $");

/*
 * See comments in e_asin.c.
 * Converted to long double by David Schultz <das@FreeBSD.ORG>.
 */

#include <cmath>
#include "ld80/invtrig.h"
#include "private.h"

using namespace _namespace(std);

long double asinl(long double x) noexcept {
  static constexpr long double one =  1.00000000000000000000e+00;
  static constexpr long double huge = 1.000e+300;

  const int16_t expsign = IEEE_l2xbits_from_ldbl(x).expsign;
  const int16_t expt = expsign & 0x7fff;
  IEEE_l2bits u = IEEE_l2bits_from_ldbl(x);
  if (expt >= BIAS) {  /* |x|>= 1 */
    mask_nbit_l(u);
    if (expt == BIAS && (u.manh | u.manl) == 0) {
      /* asin(1)=+-pi/2 with inexact */
      return x * pio2_hi + x * pio2_lo;
    }
    return (x - x) / (x - x);  /* asin(|x|>1) is NaN */
  } else if (expt < BIAS - 1) {  /* |x|<0.5 */
    if (expt < ASIN_LINEAR) {  /* if |x| is small, asinl(x)=x */
      if (huge + x > one) return x;  /* return x with inexact if x!=0*/
    }
    const long double t = x * x;
    const long double p = P(t);
    const long double q = Q(t);
    const long double w = p / q;
    return x + x * w;
  }
  /* 1> |x|>= 0.5 */
  long double w = one - fabsl(x);
  long double t = w * 0.5;
  long double p = P(t);
  long double q = Q(t);
  const long double s = sqrtl(t);
  if (u.manh >= THRESH) {  /* if |x| is close to 1 */
    w = p / q;
    t = pio2_hi - (2.0 * (s + s * w) - pio2_lo);
  } else {
    u = IEEE_l2bits_from_ldbl(s);
    u.manl = 0;
    w = ldbl_from_IEEE_l2bits(u);
    const long double c = (t - w * w) / (s + w);
    const long double r = p / q;
    p = 2.0L * s * r - (pio2_lo - 2.0L * c);
    q = pio4_hi - 2.0 * w;
    t = pio4_hi - (p - q);
  }
  if (expsign > 0)
    return t;
  else
    return -t;
}
