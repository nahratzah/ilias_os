/* @(#)s_tan.c 5.1 93/09/24 */
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

//__FBSDID("$FreeBSD: src/lib/msun/src/s_tan.c,v 1.13 2011/02/10 07:37:50 das Exp $");

/* tan(x)
 * Return tangent function of x.
 *
 * kernel function:
 *	__kernel_tan		... tangent function on [-pi/4,pi/4]
 *	__ieee754_rem_pio2	... argument reduction routine
 *
 * Method.
 *      Let S,C and T denote the sin, cos and tan respectively on
 *	[-PI/4, +PI/4]. Reduce the argument x to y1+y2 = x-k*pi/2
 *	in [-pi/4 , +pi/4], and let n = k mod 4.
 *	We have
 *
 *          n        sin(x)      cos(x)        tan(x)
 *     ----------------------------------------------------------
 *	    0	       S	   C		 T
 *	    1	       C	  -S		-1/T
 *	    2	      -S	  -C		 T
 *	    3	      -C	   S		-1/T
 *     ----------------------------------------------------------
 *
 * Special cases:
 *      Let trig be any of sin, cos, or tan.
 *      trig(+-INF)  is NaN, with signals;
 *      trig(NaN)    is that NaN;
 *
 * Accuracy:
 *	TRIG(x) returns trig(x) nearly rounded
 */

#include <cmath>
#include <array>
#include "private.h"

_namespace_begin(std)


double tan(double x) noexcept {
  using impl::__kernel_tan;
  using impl::__ieee754_rem_pio2;

#if 0
	double y[2],z=0.0;
	int32_t n, ix;
#endif

  /* High word of x. */
  const int32_t ix = get<0>(extract_words(x)) & 0x7fffffff;

  /* |x| ~< pi/4 */
  if (ix <= 0x3fe921fb) {
    if (ix < 0x3e400000)  /* x < 2**-27 */
      if (static_cast<int>(x) == 0) return x;  /* generate inexact */
    return __kernel_tan(x, 0.0, 1);
  } else if (ix >= 0x7ff00000) {  /* tan(Inf or NaN) is NaN */
    return x - x;  /* NaN */
  } else { /* argument reduction needed */
    array<double, 2> y;
    const int32_t n = __ieee754_rem_pio2(x, y.data());
    return __kernel_tan(y[0], y[1], 1 - ((n & 1) << 1)); /*   1 -- n even, -1 -- n odd */
  }
}


_namespace_end(std)
