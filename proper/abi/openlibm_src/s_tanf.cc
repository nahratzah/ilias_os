/* s_tanf.c -- float version of s_tan.c.
 * Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com.
 * Optimized by Bruce D. Evans.
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

//__FBSDID("$FreeBSD: src/lib/msun/src/s_tanf.c,v 1.17 2008/02/25 22:19:17 bde Exp $");

#include <cmath>
#include "private.h"

using namespace _namespace(std);

float tanf(float x) noexcept {
  using _namespace(std)::impl::__kernel_tandf;
  using _namespace(std)::impl::__ieee754_rem_pio2f;

  /* Small multiples of pi/2 rounded to double precision. */
  static constexpr double t1pio2 = 1 * M_PI_2;  /* 0x3FF921FB, 0x54442D18 */
  static constexpr double t2pio2 = 2 * M_PI_2;  /* 0x400921FB, 0x54442D18 */
  static constexpr double t3pio2 = 3 * M_PI_2;  /* 0x4012D97C, 0x7F3321D2 */
  static constexpr double t4pio2 = 4 * M_PI_2;  /* 0x401921FB, 0x54442D18 */

#if 0
	double y;
	int32_t n, hx, ix;
#endif

  const int32_t hx = get_float_as_int(x);
  const int32_t ix = hx & 0x7fffffff;

  if (ix <= 0x3f490fda) {  /* |x| ~<= pi/4 */
    if (ix < 0x39800000)  /* |x| < 2**-12 */
      if (static_cast<int>(x) == 0) return x;  /* x with inexact if x != 0 */
    return __kernel_tandf(x, 1);
  }
  if (ix <= 0x407b53d1) {  /* |x| ~<= 5*pi/4 */
    if (ix <= 0x4016cbe3)  /* |x| ~<= 3pi/4 */
      return __kernel_tandf(x + (hx > 0 ? -t1pio2 : t1pio2), -1);
    else
      return __kernel_tandf(x + (hx > 0 ? -t2pio2 : t2pio2), 1);
  }
  if (ix <= 0x40e231d5) {  /* |x| ~<= 9*pi/4 */
    if (ix <= 0x40afeddf)  /* |x| ~<= 7*pi/4 */
      return __kernel_tandf(x + (hx > 0 ? -t3pio2 : t3pio2), -1);
    else
      return __kernel_tandf(x + (hx > 0 ? -t4pio2 : t4pio2), 1);
  } else if (ix >= 0x7f800000) {  /* tan(Inf or NaN) is NaN */
    return x - x;
  } else { /* general argument reduction needed */
    double y;
    const int32_t n = __ieee754_rem_pio2f(x, &y);
    /* integer parameter: 1 -- n even; -1 -- n odd */
    return __kernel_tandf(y, 1 - ((n & 1) << 1));
  }
}
