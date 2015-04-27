/* @(#)e_cosh.c 1.3 95/01/18 */
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

//__FBSDID("$FreeBSD: src/lib/msun/src/e_cosh.c,v 1.10 2011/10/21 06:28:47 das Exp $");

/* __ieee754_cosh(x)
 * Method : 
 * mathematically cosh(x) if defined to be (exp(x)+exp(-x))/2
 *	1. Replace x by |x| (cosh(x) = cosh(-x)). 
 *	2. 
 *		                                        [ exp(x) - 1 ]^2 
 *	    0        <= x <= ln2/2  :  cosh(x) := 1 + -------------------
 *			       			           2*exp(x)
 *
 *		                                  exp(x) +  1/exp(x)
 *	    ln2/2    <= x <= 22     :  cosh(x) := -------------------
 *			       			          2
 *	    22       <= x <= lnovft :  cosh(x) := exp(x)/2 
 *	    lnovft   <= x <= ln2ovft:  cosh(x) := exp(x/2)/2 * exp(x/2)
 *	    ln2ovft  <  x	    :  cosh(x) := huge*huge (overflow)
 *
 * Special cases:
 *	cosh(x) is |x| if x is +INF, -INF, or NaN.
 *	only cosh(0)=1 is exact for finite x.
 */

#include <cmath>
#include "private.h"

_namespace_begin(std)


double cosh(double x) noexcept {
  using impl::__ldexp_exp;

  static constexpr double one = 1.0, half=0.5, huge = 1.0e300;

  /* High word of |x|. */
  const int32_t ix = get<0>(extract_words(x)) & 0x7fffffff;

  /* x is INF or NaN */
  if (ix >= 0x7ff00000) return x * x;

  /* |x| in [0,0.5*ln2], return 1+expm1(|x|)^2/(2*exp(|x|)) */
  if (ix < 0x3fd62e43) {
    const double t = expm1(fabs(x));
    const double w = one + t;
    if (ix < 0x3c800000) return w;  /* cosh(tiny) = 1 */
    return one + (t * t) / (w + w);
  }

  /* |x| in [0.5*ln2,22], return (exp(|x|)+1/exp(|x|)/2; */
  if (ix < 0x40360000) {
    const double t = exp(fabs(x));
    return half * t + half / t;
  }

  /* |x| in [22, log(maxdouble)] return half*exp(|x|) */
  if (ix < 0x40862E42) return half * exp(fabs(x));

  /* |x| in [log(maxdouble), overflowthresold] */
  if (ix <= 0x408633CE)
    return __ldexp_exp(fabs(x), -1);

  /* |x| > overflowthresold, cosh(x) overflow */
  return huge * huge;
}


_namespace_end(std)
