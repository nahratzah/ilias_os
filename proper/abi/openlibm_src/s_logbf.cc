/* s_logbf.c -- float version of s_logb.c.
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

//__FBSDID("$FreeBSD: src/lib/msun/src/s_logbf.c,v 1.9 2008/02/22 02:30:35 das Exp $");

#include <cmath>
#include "private.h"

using namespace _namespace(std);

float logbf(float x) noexcept {
  static constexpr float two25 = 3.355443200e+07;  /* 0x4c000000 */

  int32_t ix = get_float_as_int(x);
  ix &= 0x7fffffff;  /* high |x| */
  if (ix == 0) return -1.0f / fabsf(x);
  if (ix >= 0x7f800000) return x * x;
  if (ix < 0x00800000) {
    x *= two25;  /* convert subnormal x to normal */
    ix = get_float_as_int(x);
    ix &= 0x7fffffff;
    return static_cast<float>((ix >> 23) - 127 - 25);
  } else {
    return static_cast<float>((ix >> 23) - 127);
  }
}
