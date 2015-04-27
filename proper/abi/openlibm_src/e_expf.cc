/* e_expf.c -- float version of e_exp.c.
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

//__FBSDID("$FreeBSD: src/lib/msun/src/e_expf.c,v 1.16 2011/10/21 06:26:38 das Exp $");

#include <cmath>
#include <array>
#include "private.h"

using namespace _namespace(std);

float expf(float x) noexcept {
  static constexpr float one = 1.0;
  static constexpr array<float, 2> halF{{ 0.5, -0.5 }};
  static constexpr float huge = 1.0e+30;
  static constexpr float o_threshold = 8.8721679688e+01;  /* 0x42b17180 */
  static constexpr float u_threshold = -1.0397208405e+02;  /* 0xc2cff1b5 */
  static constexpr array<float, 2> ln2HI{{ 6.9314575195e-01,  /* 0x3f317200 */
                                          -6.9314575195e-01 }};  /* 0xbf317200 */
  static constexpr array<float, 2> ln2LO{{ 1.4286067653e-06,  /* 0x35bfbe8e */
                                          -1.4286067653e-06 }};  /* 0xb5bfbe8e */
  static constexpr float invln2 = 1.4426950216e+00;  /* 0x3fb8aa3b */
  /*
   * Domain [-0.34568, 0.34568], range ~[-4.278e-9, 4.447e-9]:
   * |x*(exp(x)+1)/(exp(x)-1) - p(x)| < 2**-27.74
   */
  static constexpr float P1 =  1.6666625440e-1;  /*  0xaaaa8f.0p-26 */
  static constexpr float P2 = -2.7667332906e-3;  /* -0xb55215.0p-32 */
  static constexpr float twom100 = 7.8886090522e-31;  /* 2**-100=0x0d800000 */

  uint32_t hx = get_float_as_int(x);
  const int32_t xsb = (hx >> 31) & 1;  /* sign bit of x */
  hx &= 0x7fffffff;  /* high word of |x| */

  /* filter out non-finite argument */
  if (hx >= 0x42b17218) {  /* if |x|>=88.721... */
    if (hx > 0x7f800000) return x + x;  /* NaN */
    if (hx == 0x7f800000) return (xsb == 0 ? x : 0.0);  /* exp(+-inf)={inf,0} */
    if (x > o_threshold) return huge * huge;  /* overflow */
    if (x < u_threshold) return twom100 * twom100;  /* underflow */
  }

  /* argument reduction */
  int32_t k = 0;
  float hi = 0, lo = 0;
  if (hx > 0x3eb17218) {  /* if  |x| > 0.5 ln2 */
    if (hx < 0x3F851592) {  /* and |x| < 1.5 ln2 */
      hi = x - ln2HI[xsb];
      lo = ln2LO[xsb];
      k = 1 - xsb - xsb;
    } else {
      k = invln2 * x + halF[xsb];
      const float t = k;
      hi = x - t * ln2HI[0];  /* t*ln2HI is exact here */
      lo = t * ln2LO[0];
    }
    x = static_cast<float>(hi - lo);
  } else if (hx < 0x39000000) {  /* when |x|<2**-14 */
    if (huge + x > one)
      return one + x;  /* trigger inexact */
  }

  /* x is now in primary range */
  const float t = x * x;
  const float twopk =
      set_float_from_int(0x3f800000 + ((k >= -125 ? k : k + 100) << 23));
  const float c = x - t * (P1 + t * P2);
  if (k == 0)
    return one - ((x * c) / (c - 2.0f) - x);
  const float y = one - ((lo - (x * c) / (2.0f - c)) - hi);
  if (k >= -125) {
    if (k == 128) return y * 2.0F * 0x1p127F;
    return y * twopk;
  } else {
    return y * twopk * twom100;
  }
}
