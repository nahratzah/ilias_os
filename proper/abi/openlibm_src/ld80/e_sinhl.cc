/* @(#)e_sinh.c 5.1 93/09/24 */
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

/* sinhl(x)
 * Method :
 * mathematically sinh(x) if defined to be (exp(x)-exp(-x))/2
 *	1. Replace x by |x| (sinhl(-x) = -sinhl(x)).
 *	2.
 *		                                     E + E/(E+1)
 *	    0        <= x <= 25     :  sinhl(x) := --------------, E=expm1l(x)
 *			       			         2
 *
 *	    25       <= x <= lnovft :  sinhl(x) := expl(x)/2
 *	    lnovft   <= x <= ln2ovft:  sinhl(x) := expl(x/2)/2 * expl(x/2)
 *	    ln2ovft  <  x	    :  sinhl(x) := x*shuge (overflow)
 *
 * Special cases:
 *	sinhl(x) is |x| if x is +INF, -INF, or NaN.
 *	only sinhl(0)=0 is exact for finite x.
 */

#include <cmath>
#include "../private.h"

using namespace _namespace(std);

long double sinhl(long double x) noexcept {
  static const long double one = 1.0, shuge = 1.0e4931L;

#if 0
	long double t,w,h;
	u_int32_t jx,ix,i0,i1;
#endif

  /* Words of |x|. */
  uint32_t jx, i0, i1;
  tie(jx, i0, i1) = get_ldouble_words(x);
  const uint32_t ix = jx & 0x7fff;

  /* x is INF or NaN */
  if (ix == 0x7fff) return x + x;

  const long double h = (jx & 0x8000 ? -0.5L : 0.5L);
  /* |x| in [0,25], return sign(x)*0.5*(E+E/(E+1))) */
  if (ix < 0x4003 || (ix == 0x4003 && i0 <= 0xc8000000)) {  /* |x|<25 */
    if (ix<0x3fdf)  /* |x|<2**-32 */
      if (shuge + x > one) return x;  /* sinh(tiny) = tiny with inexact */
    const long double t = expm1l(fabsl(x));
    if (ix < 0x3fff) return h * (2.0 * t - t * t / (t + one));
    return h * (t + t / (t + one));
  }

  /* |x| in [25, log(maxdouble)] return 0.5*exp(|x|) */
  if (ix < 0x400c || (ix == 0x400c && i0 < 0xb17217f7))
    return h * expl(fabsl(x));

  /* |x| in [log(maxdouble), overflowthreshold] */
  if (ix<0x400c || (ix == 0x400c &&
                    (i0 < 0xb174ddc0 ||
                     (i0 == 0xb174ddc0 &&
                      i1 <= 0x31aec0ea)))) {
    const long double w = expl(0.5*fabsl(x));
    const long double t = h * w;
    return t * w;
  }

  /* |x| > overflowthreshold, sinhl(x) overflow */
  return x * shuge;
}
