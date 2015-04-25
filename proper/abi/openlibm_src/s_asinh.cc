/* @(#)s_asinh.c 5.1 93/09/24 */
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

//__FBSDID("$FreeBSD: src/lib/msun/src/s_asinh.c,v 1.9 2008/02/22 02:30:35 das Exp $");

/* asinh(x)
 * Method :
 *	Based on
 *		asinh(x) = sign(x) * log [ |x| + sqrt(x*x+1) ]
 *	we have
 *	asinh(x) := x  if  1+x*x=1,
 *		 := sign(x)*(log(x)+ln2)) for large |x|, else
 *		 := sign(x)*log(2|x|+1/(|x|+sqrt(x*x+1))) if|x|>2, else
 *		 := sign(x)*log1p(|x| + x^2/(1 + sqrt(1+x^2)))
 */

#include <cmath>
#include "private.h"

_namespace_begin(std)


double asinh(double x) noexcept {
  static constexpr double one =  1.00000000000000000000e+00;  /* 0x3FF00000, 0x00000000 */
  static constexpr double ln2 =  6.93147180559945286227e-01;  /* 0x3FE62E42, 0xFEFA39EF */
  static constexpr double huge=  1.00000000000000000000e+300;

  const int32_t hx = get<0>(extract_words(x));
  const int32_t ix = hx & 0x7fffffff;
  if (ix >= 0x7ff00000) return x + x;  /* x is inf or NaN */
  if (ix < 0x3e300000) {  /* |x|<2**-28 */
    if(huge + x > one) return x;  /* return x inexact except 0 */
  }
  double w;
  if (ix > 0x41b00000) {  /* |x| > 2**28 */
    w = log(fabs(x)) + ln2;
  } else if (ix > 0x40000000) {  /* 2**28 > |x| > 2.0 */
    const double t = fabs(x);
    w = log(2.0 * t + one / (sqrt(x * x + one) + t));
  } else {  /* 2.0 > |x| > 2**-28 */
    const double t = x * x;
    w =log1p(fabs(x) + t / (one + sqrt(one + t)));
  }
  if (hx > 0)
    return w;
  else
    return -w;
}


_namespace_end(std)
