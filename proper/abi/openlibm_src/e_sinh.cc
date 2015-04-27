/* @(#)e_sinh.c 1.3 95/01/18 */
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

//__FBSDID("$FreeBSD: src/lib/msun/src/e_sinh.c,v 1.11 2011/10/21 06:28:47 das Exp $");

/* __ieee754_sinh(x)
 * Method : 
 * mathematically sinh(x) if defined to be (exp(x)-exp(-x))/2
 *	1. Replace x by |x| (sinh(-x) = -sinh(x)). 
 *	2. 
 *		                                    E + E/(E+1)
 *	    0        <= x <= 22     :  sinh(x) := --------------, E=expm1(x)
 *			       			        2
 *
 *	    22       <= x <= lnovft :  sinh(x) := exp(x)/2 
 *	    lnovft   <= x <= ln2ovft:  sinh(x) := exp(x/2)/2 * exp(x/2)
 *	    ln2ovft  <  x	    :  sinh(x) := x*shuge (overflow)
 *
 * Special cases:
 *	sinh(x) is |x| if x is +INF, -INF, or NaN.
 *	only sinh(0)=0 is exact for finite x.
 */

#include <cmath>
#include "private.h"

_namespace_begin(std)


double sinh(double x) noexcept {
  using _namespace(std)::impl::__ldexp_exp;

  static constexpr double one = 1.0, shuge = 1.0e307;

  /* High word of |x|. */
  const int32_t jx = get<0>(extract_words(x));
  const int32_t ix = jx & 0x7fffffff;

  /* x is INF or NaN */
  if (ix >= 0x7ff00000) return x + x;

  const double h = (jx < 0 ? -0.5 : 0.5);
  /* |x| in [0,22], return sign(x)*0.5*(E+E/(E+1))) */
  if (ix < 0x40360000) {  /* |x|<22 */
    if (ix < 0x3e300000)  /* |x|<2**-28 */
      if (shuge + x > one) return x;  /* sinh(tiny) = tiny with inexact */
    const double t = expm1(fabs(x));
    if (ix < 0x3ff00000) return h * (2.0 * t - t * t / (t + one));
    return h * (t + t / (t + one));
  }

  /* |x| in [22, log(maxdouble)] return 0.5*exp(|x|) */
  if (ix < 0x40862E42) return h * exp(fabs(x));

  /* |x| in [log(maxdouble), overflowthresold] */
  if (ix <= 0x408633CE)
    return h * 2.0 * __ldexp_exp(fabs(x), -1);

  /* |x| > overflowthresold, sinh(x) overflow */
  return x * shuge;
}


_namespace_end(std)
