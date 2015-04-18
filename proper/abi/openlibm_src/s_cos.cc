/* @(#)s_cos.c 5.1 93/09/24 */
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

//__FBSDID("$FreeBSD: src/lib/msun/src/s_cos.c,v 1.13 2011/02/10 07:37:50 das Exp $");

/* cos(x)
 * Return cosine function of x.
 *
 * kernel function:
 *	__kernel_sin		... sine function on [-pi/4,pi/4]
 *	__kernel_cos		... cosine function on [-pi/4,pi/4]
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


double cos(double x) noexcept {
  using impl::__ieee754_rem_pio2;
  using impl::__kernel_sin;
  using impl::__kernel_cos;

  /* High word of x. */
  const double z = 0.0;
  const int32_t ix = get<0>(extract_words(x)) & 0x7fffffff;

  /* |x| ~< pi/4 */
  if (ix <= 0x3fe921fb) {
    if (ix < 0x3e46a09e)  /* if x < 2**-27 * sqrt(2) */
      if (static_cast<int>(x) == 0)
        return 1.0;  /* generate inexact */
    return __kernel_cos(x, z);
  } else if (ix>=0x7ff00000) {  /* cos(Inf or NaN) is NaN */
    return x - x;
  } else {  /* argument reduction needed */
    array<double, 2> y;
    const int32_t n = __ieee754_rem_pio2(x, y.data());
    switch (n & 3) {
    case 0:
      return  __kernel_cos(y[0], y[1]);
    case 1:
      return -__kernel_sin(y[0], y[1], 1);
    case 2:
      return -__kernel_cos(y[0], y[1]);
    default:
      return  __kernel_sin(y[0], y[1], 1);
    }
  }
}


_namespace_end(std)
