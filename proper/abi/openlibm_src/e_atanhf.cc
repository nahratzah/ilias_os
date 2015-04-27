/* e_atanhf.c -- float version of e_atanh.c.
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

//__FBSDID("$FreeBSD: src/lib/msun/src/e_atanhf.c,v 1.7 2008/02/22 02:30:34 das Exp $");

#include <cmath>
#include "private.h"

using namespace _namespace(std);

float atanhf(float x) noexcept {
  static constexpr float one = 1.0, huge = 1e30;
  static constexpr float zero = 0.0;

  const int32_t hx = get_float_as_int(x);
  const int32_t ix = hx & 0x7fffffff;
  if (ix > 0x3f800000)  /* |x|>1 */
    return (x - x) / (x - x);
  if (ix == 0x3f800000)
    return x / zero;
  if (ix < 0x31800000 && (huge + x) > zero) return x;  /* x<2**-28 */
  x = set_float_from_int(ix);
  float t;
  if(ix<0x3f000000) {  /* x < 0.5 */
      t = x + x;
      t = 0.5f * log1pf(t + t * x / (one - x));
  } else {
      t = 0.5f * log1pf((x + x) / (one - x));
  }
  if (hx >= 0)
    return t;
  else
    return -t;
}
