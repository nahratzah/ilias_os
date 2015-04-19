/* e_acosf.c -- float version of e_acos.c.
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

//__FBSDID("$FreeBSD: src/lib/msun/src/e_acosf.c,v 1.11 2008/08/03 17:39:54 das Exp $");

#include <cmath>
#include "private.h"

using namespace _namespace(std);

float acosf(float x) noexcept {
  static constexpr float one =  1.0000000000e+00; /* 0x3F800000 */
  static constexpr float pi =  3.1415925026e+00; /* 0x40490fda */
  static constexpr float pio2_hi =  1.5707962513e+00; /* 0x3fc90fda */
  static constexpr float pio2_lo =  7.5497894159e-08; /* 0x33a22168 */
  static constexpr float pS0 =  1.6666586697e-01;
  static constexpr float pS1 = -4.2743422091e-02;
  static constexpr float pS2 = -8.6563630030e-03;
  static constexpr float qS1 = -7.0662963390e-01;

  const int32_t hx = get_float_as_int(x);
  const int32_t ix = hx & 0x7fffffff;
  if (ix >= 0x3f800000) {  /* |x| >= 1 */
    if (ix == 0x3f800000) {  /* |x| == 1 */
      if (hx > 0)  /* acos(1) = 0 */
        return 0.0;
      else  /* acos(-1)= pi */
        return pi + 2.0f * pio2_lo;  /* acos(-1)= pi */
    }
    return (x - x) / (x - x);  /* acos(|x|>1) is NaN */
  }
  if (ix < 0x3f000000) {  /* |x| < 0.5 */
    if (ix <= 0x32800000) return pio2_hi + pio2_lo;  /* if |x| < 2**-26 */
    const float z = x * x;
    const float p = z * (pS0 + z * (pS1 + z * pS2));
    const float q = one + z * qS1;
    const float r = p / q;
    return pio2_hi - (x - (pio2_lo -x * r));
  } else if (hx < 0) {  /* x < -0.5 */
    const float z = (one + x) * 0.5f;
    const float p = z * (pS0 + z * (pS1 + z * pS2));
    const float q = one + z * qS1;
    const float s = sqrtf(z);
    const float r = p / q;
    const float w = r * s - pio2_lo;
    return pi - 2.0f * (s + w);
  } else {  /* x > 0.5 */
    const float z = (one - x) * 0.5f;
    const float s = sqrtf(z);
    const float df = set_float_from_int(get_float_as_int(s) & 0xfffff000U);
    const float c = (z - df * df) / (s + df);
    const float p = z * (pS0 + z * (pS1 + z * pS2));
    const float q = one + z * qS1;
    const float r = p / q;
    const float w = r * s + c;
    return 2.0f * (df + w);
  }
}
