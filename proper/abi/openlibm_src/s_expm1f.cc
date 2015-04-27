/* s_expm1f.c -- float version of s_expm1.c.
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

//__FBSDID("$FreeBSD: src/lib/msun/src/s_expm1f.c,v 1.12 2011/10/21 06:26:38 das Exp $");

#include <cmath>
#include "private.h"

using namespace _namespace(std);

float expm1f(float x) noexcept {
  static constexpr float one         = 1.0;
  static constexpr float huge        = 1.0e+30;
  static constexpr float tiny        = 1.0e-30;
  static constexpr float o_threshold = 8.8721679688e+01;  /* 0x42b17180 */
  static constexpr float ln2_hi      = 6.9313812256e-01;  /* 0x3f317180 */
  static constexpr float ln2_lo      = 9.0580006145e-06;  /* 0x3717f7d1 */
  static constexpr float invln2      = 1.4426950216e+00;  /* 0x3fb8aa3b */
  /*
   * Domain [-0.34568, 0.34568], range ~[-6.694e-10, 6.696e-10]:
   * |6 / x * (1 + 2 * (1 / (exp(x) - 1) - 1 / x)) - q(x)| < 2**-30.04
   * Scaled coefficients: Qn_here = 2**n * Qn_for_q (see s_expm1.c):
   */
  static constexpr float Q1 = -3.3333212137e-2;  /* -0x888868.0p-28 */
  static constexpr float Q2 =  1.5807170421e-3;  /*  0xcf3010.0p-33 */

  uint32_t hx = get_float_as_int(x);
  const int32_t xsb = hx & 0x80000000;  /* sign bit of x */
  hx &= 0x7fffffff;  /* high word of |x| */

  /* filter out huge and non-finite argument */
  if (hx >= 0x4195b844) {  /* if |x|>=27*ln2 */
    if (hx >= 0x42b17218) {  /* if |x|>=88.721... */
      if (hx > 0x7f800000) return x + x;  /* NaN */
      if (hx == 0x7f800000) return (xsb == 0 ? x : -1.0);  /* exp(+-inf)={inf,-1} */
      if (x > o_threshold) return huge * huge;  /* overflow */
    }
    if (xsb != 0) {  /* x < -27*ln2, return -1.0 with inexact */
      if (x + tiny < 0.0f)  /* raise inexact */
        return tiny - one;  /* return -1 */
    }
  }

  /* argument reduction */
  int32_t k;
  double c;
  if (hx > 0x3eb17218) {  /* if  |x| > 0.5 ln2 */
    float hi, lo;
    if (hx < 0x3F851592) {  /* and |x| < 1.5 ln2 */
      if (xsb == 0) {
        hi = x - ln2_hi;
        lo = ln2_lo;
        k = 1;
      } else {
        hi = x + ln2_hi;
        lo = -ln2_lo;
        k = -1;
      }
    } else {
      k = invln2 * x + (xsb == 0 ? 0.5f : -0.5f);
      const float t = k;
      hi = x - t * ln2_hi;  /* t*ln2_hi is exact here */
      lo = t * ln2_lo;
    }
    x = static_cast<float>(hi - lo);
    c = (hi - x) - lo;
  } else if (hx < 0x33000000) {  /* when |x|<2**-25, return x */
    const float t = huge + x;  /* return x with inexact flags when x!=0 */
    return x - (t - (huge + x));
  } else {
    k = 0;
  }

  /* x is now in primary range */
  const float hfx = 0.5f * x;
  const float hxs = x * hfx;
  const float r1 = one + hxs * (Q1 + hxs * Q2);
  float t = 3.0f - r1 * hfx;
  float e = hxs * ((r1 - t) / (6.0f - x * t));
  float y;
  if (k == 0) {
    return x - (x * e - hxs);  /* c is 0 */
  } else {
    const float twopk = set_float_from_int(0x3f800000 + (k << 23));  /* 2^k */
    e = (x * (e - c) - c);
    e -= hxs;
    if (k== -1) return 0.5f * (x - e) - 0.5f;
    if (k==1) {
      if (x < -0.25f)
        return -2.0f * (e - (x + 0.5f));
      else
        return one + 2.0f * (x - e);
    }
    if (k <= -2 || k > 56) {  /* suffice to return exp(x)-1 */
      y = one - (e - x);
      if (k == 128)
        y = y * 2.0F * 0x1p127F;
      else
        y = y * twopk;
      return y - one;
    }
    t = one;
    if (k < 23) {
      t = set_float_from_int(0x3f800000 - (0x1000000 >> k));  /* t=1-2^-k */
      y = t - (e - x);
      y = y * twopk;
    } else {
      t = set_float_from_int((0x7f - k) << 23);  /* 2^-k */
      y = x - (e + t);
      y += one;
      y = y * twopk;
    }
  }
  return y;
}
