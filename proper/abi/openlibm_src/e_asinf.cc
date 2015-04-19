/* e_asinf.c -- float version of e_asin.c.
 * Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com.
 */

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

//__FBSDID("$FreeBSD: src/lib/msun/src/e_asinf.c,v 1.13 2008/08/08 00:21:27 das Exp $");

#include <cmath>
#include "private.h"

using namespace _namespace(std);

static constexpr float one =  1.0000000000e+00; /* 0x3F800000 */
static constexpr float huge =  1.000e+30;
	/* coefficient for R(x^2) */
static constexpr float pS0 =  1.6666586697e-01;
static constexpr float pS1 = -4.2743422091e-02;
static constexpr float pS2 = -8.6563630030e-03;
static constexpr float qS1 = -7.0662963390e-01;
static constexpr double pio2 =  1.570796326794896558e+00;

float asinf(float x) noexcept {
  const int32_t hx = get_float_as_int(x);
  const int32_t ix = hx & 0x7fffffff;
  if (ix >= 0x3f800000) {  /* |x| >= 1 */
    if (ix == 0x3f800000)  /* |x| == 1 */
      return x * pio2;  /* asin(+-1) = +-pi/2 with inexact */
    return (x - x) / (x - x);  /* asin(|x|>1) is NaN */
  } else if (ix < 0x3f000000) {  /* |x|<0.5 */
    if (ix < 0x39800000) {  /* |x| < 2**-12 */
      if (huge + x > one) return x;  /* return x with inexact if x!=0*/
    }
    const float t = x * x;
    const float p = t * (pS0 + t * (pS1 + t * pS2));
    const float q = one + t * qS1;
    const float w = p / q;
    return x + x * w;
  }
  /* 1> |x|>= 0.5 */
  float w = one - fabsf(x);
  float t = w * 0.5f;
  const float p = t * (pS0 + t * (pS1 + t * pS2));
  const float q = one + t * qS1;
  const double s = sqrt(static_cast<double>(t));
  w = p / q;
  t = pio2 - 2.0 * (s + s * w);
  if (hx > 0)
    return t;
  else
    return -t;
}
