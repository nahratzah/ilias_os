/* s_cosf.c -- float version of s_cos.c.
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

//__FBSDID("$FreeBSD: src/lib/msun/src/s_cosf.c,v 1.18 2008/02/25 22:19:17 bde Exp $");

#include <cmath>
#include "private.h"

using namespace _namespace(std);

float cosf(float x) noexcept {
  using _namespace(std)::impl::__ieee754_rem_pio2f;
  using _namespace(std)::impl::__kernel_sindf;
  using _namespace(std)::impl::__kernel_cosdf;

  /* Small multiples of pi/2 rounded to double precision. */
  static constexpr double c1pio2 = 1*M_PI_2;  /* 0x3FF921FB, 0x54442D18 */
  static constexpr double c2pio2 = 2*M_PI_2;  /* 0x400921FB, 0x54442D18 */
  static constexpr double c3pio2 = 3*M_PI_2;  /* 0x4012D97C, 0x7F3321D2 */
  static constexpr double c4pio2 = 4*M_PI_2;  /* 0x401921FB, 0x54442D18 */

  const int32_t hx = get_float_as_int(x);
  const int32_t ix = hx & 0x7fffffff;

  if (ix <= 0x3f490fda) {  /* |x| ~<= pi/4 */
    if (ix < 0x39800000)  /* |x| < 2**-12 */
      if (static_cast<int>(x) == 0) return 1.0f;  /* 1 with inexact if x != 0 */
    return __kernel_cosdf(x);
  }
  if (ix <= 0x407b53d1) {  /* |x| ~<= 5*pi/4 */
    if (ix <= 0x4016cbe3) {  /* |x|  ~> 3*pi/4 */
      if (hx > 0)
        return __kernel_sindf(c1pio2 - x);
      else
        return __kernel_sindf(x + c1pio2);
    } else {
      return -__kernel_cosdf(x + (hx > 0 ? -c2pio2 : c2pio2));
    }
  }
  if (ix <= 0x40e231d5) {  /* |x| ~<= 9*pi/4 */
    if (ix <= 0x40afeddf) {  /* |x|  ~> 7*pi/4 */
      if (hx > 0)
        return __kernel_sindf(x - c3pio2);
      else
        return __kernel_sindf(-c3pio2 - x);
    } else {
      return __kernel_cosdf(x + (hx > 0 ? -c4pio2 : c4pio2));
    }
  } else if (ix>=0x7f800000) {  /* cos(Inf or NaN) is NaN */
    return x-x;
  } else { /* general argument reduction needed */
    double y;
    const int32_t n = __ieee754_rem_pio2f(x, &y);
    switch (n & 3) {
    case 0:
      return  __kernel_cosdf(y);
    case 1:
      return  __kernel_sindf(-y);
    case 2:
      return -__kernel_cosdf(y);
    default:
      return  __kernel_sindf(y);
    }
  }
}
