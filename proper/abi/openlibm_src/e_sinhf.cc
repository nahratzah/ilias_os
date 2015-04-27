/* e_sinhf.c -- float version of e_sinh.c.
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

//__FBSDID("$FreeBSD: src/lib/msun/src/e_sinhf.c,v 1.10 2011/10/21 06:28:47 das Exp $");

#include <cmath>
#include "private.h"

using namespace _namespace(std);

float sinhf(float x) noexcept {
  using _namespace(std)::impl::__ldexp_expf;

  static constexpr float one = 1.0, shuge = 1.0e37;

  const int32_t jx = get_float_as_int(x);
  const int32_t ix = jx & 0x7fffffff;

  /* x is INF or NaN */
  if (ix >= 0x7f800000) return x + x;

  const float h = (jx < 0 ? -0.5f : 0.5f);
  /* |x| in [0,9], return sign(x)*0.5*(E+E/(E+1))) */
  if (ix < 0x41100000) {  /* |x|<9 */
    if (ix < 0x39800000)  /* |x|<2**-12 */
      if (shuge + x > one) return x;  /* sinh(tiny) = tiny with inexact */
    const float t = expm1f(fabsf(x));
    if (ix < 0x3f800000) return h * (2.0f * t - t * t / (t + one));
    return h * (t + t / (t + one));
  }

  /* |x| in [9, logf(maxfloat)] return 0.5*exp(|x|) */
  if (ix < 0x42b17217)  return h * expf(fabsf(x));

  /* |x| in [logf(maxfloat), overflowthresold] */
  if (ix <= 0x42b2d4fc)
    return h * 2.0F * __ldexp_expf(fabsf(x), -1);

  /* |x| > overflowthresold, sinh(x) overflow */
  return x * shuge;
}
