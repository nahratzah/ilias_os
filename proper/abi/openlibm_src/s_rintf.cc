/* s_rintf.c -- float version of s_rint.c.
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

//__FBSDID("$FreeBSD: src/lib/msun/src/s_rintf.c,v 1.12 2008/02/22 02:30:35 das Exp $");

#include <cmath>
#include <array>
#include "private.h"

using namespace _namespace(std);

float rintf(float x) noexcept {
  static constexpr array<float, 2> TWO23{{
    8.3886080000e+06, /* 0x4b000000 */
   -8.3886080000e+06, /* 0xcb000000 */
  }};

  int32_t i0 = get_float_as_int(x);
  const int32_t sx = (i0 >> 31) & 1;
  const int32_t j0 = ((i0 >> 23) & 0xff) - 0x7f;
  if (j0 < 23) {
    if (j0 < 0) {
      if ((i0 & 0x7fffffff) == 0) return x;
      const float w = static_cast<float>(TWO23[sx] + x);
      float t = w-TWO23[sx];
      i0 = get_float_as_int(t);
      t = set_float_from_int((i0 & 0x7fffffff) | (sx << 31));
      return t;
    }
    const float w = static_cast<float>(TWO23[sx] + x);
    return w - TWO23[sx];
  }
  if (j0 == 0x80)
    return x + x;  /* inf or NaN */
  else
    return x;  /* x is integral */
}
