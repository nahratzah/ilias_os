/* e_acoshf.c -- float version of e_acosh.c.
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

//__FBSDID("$FreeBSD: src/lib/msun/src/e_acoshf.c,v 1.8 2008/02/22 02:30:34 das Exp $");

#include <cmath>
#include "private.h"

using namespace _namespace(std);

float acoshf(float x) noexcept {
  static constexpr float one	= 1.0;
  static constexpr float ln2	= 6.9314718246e-01;  /* 0x3f317218 */

  const int32_t hx = get_float_as_int(x);
  if (hx < 0x3f800000) {  /* x < 1 */
    return (x - x) / (x - x);
  } else if (hx >= 0x4d800000) {  /* x > 2**28 */
    if (hx >= 0x7f800000)  /* x is inf of NaN */
      return x + x;
    else
      return logf(x) + ln2;  /* acosh(huge)=log(2x) */
  } else if (hx == 0x3f800000) {
    return 0.0f;  /* acosh(1) = 0 */
  } else if (hx > 0x40000000) {  /* 2**28 > x > 2 */
    const float t = x * x;
    return logf(2.0f * x - one / (x + sqrtf(t - one)));
  } else {  /* 1<x<2 */
    const float t = x - one;
    return log1pf(t + sqrtf(2.0f * t + t * t));
  }
}
