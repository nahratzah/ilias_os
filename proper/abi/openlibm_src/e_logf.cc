/* e_logf.c -- float version of e_log.c.
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

//__FBSDID("$FreeBSD: src/lib/msun/src/e_logf.c,v 1.11 2008/03/29 16:37:59 das Exp $");

#include <cmath>
#include "private.h"

using namespace _namespace(std);

float logf(float x) noexcept {
  static constexpr float ln2_hi = 6.9313812256e-01;  /* 0x3f317180 */
  static constexpr float ln2_lo = 9.0580006145e-06;  /* 0x3717f7d1 */
  static constexpr float two25 = 3.355443200e+07;  /* 0x4c000000 */
  /* |(log(1+s)-log(1-s))/s - Lg(s)| < 2**-34.24 (~[-4.95e-11, 4.97e-11]). */
  static constexpr float Lg1 = 0xaaaaaa.0p-24;  /* 0.66666662693 */
  static constexpr float Lg2 = 0xccce13.0p-25;  /* 0.40000972152 */
  static constexpr float Lg3 = 0x91e9ee.0p-25;  /* 0.28498786688 */
  static constexpr float Lg4 = 0xf89e26.0p-26;  /* 0.24279078841 */
  static constexpr float zero   =  0.0;

#if 0
	float hfsq,f,s,z,R,w,t1,t2,dk;
	int32_t k,ix,i,j;
#endif

  int32_t ix = get_float_as_int(x);

  int32_t k = 0;
  if (ix < 0x00800000) {  /* x < 2**-126  */
    if ((ix & 0x7fffffff) == 0)
      return -two25 / zero;  /* log(+-0)=-inf */
    if (ix < 0)
      return (x - x) / zero;  /* log(-#) = NaN */
    k -= 25;
    x *= two25;  /* subnormal number, scale up x */
    ix = get_float_as_int(x);
  }
  if (ix >= 0x7f800000)
    return x + x;
  k += (ix >> 23) - 127;
  ix &= 0x007fffff;
  int32_t i = (ix + (0x95f64 << 3)) & 0x800000;
  x = set_float_from_int(ix | (i ^ 0x3f800000));  /* normalize x or x/2 */
  k += (i >> 23);
  const float f = x - 1.0f;
  if ((0x007fffff & (0x8000 + ix)) < 0xc000) {  /* -2**-9 <= f < 2**-9 */
    if (f == zero) {
      if (k == 0) {
        return zero;
      } else {
        const float dk = static_cast<float>(k);
        return dk * ln2_hi + dk * ln2_lo;
      }
    }
    const float R = f * f * (0.5f - 0.33333333333333333f * f);
    if (k == 0)
      return f - R;
    else {
      const float dk = static_cast<float>(k);
      return dk * ln2_hi - ((R - dk * ln2_lo) - f);
    }
  }
  const float s = f / (2.0f + f);
  const float dk = static_cast<float>(k);
  const float z = s * s;
  i = ix - (0x6147a << 3);
  const float w = z * z;
  const int32_t j = (0x6b851 << 3) - ix;
  const float t1 = w * (Lg2 + w * Lg4);
  const float t2 = z * (Lg1 + w * Lg3);
  i |= j;
  const float R = t2 + t1;
  if (i > 0) {
    const float hfsq = 0.5f * f * f;
    if (k == 0)
      return f - (hfsq - s * (hfsq + R));
    else
      return dk * ln2_hi - ((hfsq - (s * (hfsq + R) + dk * ln2_lo)) - f);
  } else {
    if (k == 0)
      return f - s * (f - R);
    else
      return dk * ln2_hi - ((s * (f - R) - dk * ln2_lo) - f);
  }
}
