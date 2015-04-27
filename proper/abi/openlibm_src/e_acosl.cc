
/* @(#)e_acos.c 1.3 95/01/18 */
/* FreeBSD: head/lib/msun/src/e_acos.c 176451 2008-02-22 02:30:36Z das */
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

//__FBSDID("$FreeBSD: src/lib/msun/src/e_acosl.c,v 1.2 2008/08/02 03:56:22 das Exp $");

/*
 * See comments in e_acos.c.
 * Converted to long double by David Schultz <das@FreeBSD.ORG>.
 */

#include <cmath>
#include "ld80/invtrig.h"
#include "private.h"

using namespace _namespace(std);

long double acosl(long double x) noexcept {
  static constexpr long double one=  1.00000000000000000000e+00;
  static constexpr long double pi =  3.14159265358979323846264338327950280e+00L;

  const int16_t expsign = IEEE_l2xbits_from_ldbl(x).expsign;
  const int16_t expt = expsign & 0x7fff;
  if (expt >= BIAS) {  /* |x| >= 1 */
    IEEE_l2bits u = IEEE_l2bits_from_ldbl(x);
    mask_nbit_l(u);
    if (expt == BIAS && (u.manh | u.manl) == 0) {
      if (expsign > 0)  /* acos(1) = 0  */
        return 0.0L;
      else  /* acos(-1)= pi */
        return pi+2.0*pio2_lo;
    }
    return (x - x) / (x - x);		/* acos(|x|>1) is NaN */
  }
  if (expt < BIAS - 1) {  /* |x| < 0.5 */
    if (expt < ACOS_CONST) return pio2_hi + pio2_lo;  /* x tiny: acosl=pi/2 */
    const long double z = x * x;
    const long double p = P(z);
    const long double q = Q(z);
    const long double r = p / q;
    return pio2_hi - (x - (pio2_lo - x * r));
  } else if (expsign < 0) {  /* x < -0.5 */
    const long double z = (one + x) * 0.5L;
    const long double p = P(z);
    const long double q = Q(z);
    const long double s = sqrtl(z);
    const long double r = p / q;
    const long double w = r * s - pio2_lo;
    return pi - 2.0L * (s + w);
  } else {  /* x > 0.5 */
    const long double z = (one - x) * 0.5L;
    const long double s = sqrtl(z);
    IEEE_l2bits u = IEEE_l2bits_from_ldbl(s);
    u.manl = 0;
    const long double df = ldbl_from_IEEE_l2bits(u);
    const long double c  = (z - df * df) / (s + df);
    const long double p = P(z);
    const long double q = Q(z);
    const long double r = p / q;
    const long double w = r * s + c;
    return 2.0L * (df + w);
  }
}
