/* e_atan2f.c -- float version of e_atan2.c.
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

//__FBSDID("$FreeBSD: src/lib/msun/src/e_atan2f.c,v 1.12 2008/08/03 17:39:54 das Exp $");

#include <cmath>
#include "private.h"

using namespace _namespace(std);

float atan2f(float y, float x) noexcept {
  static constexpr float tiny  = 1.0e-30;
  static constexpr float zero  = 0.0;
  static constexpr float pi_o_4  = 7.8539818525e-01; /* 0x3f490fdb */
  static constexpr float pi_o_2  = 1.5707963705e+00; /* 0x3fc90fdb */
  static constexpr float pi      = 3.1415927410e+00; /* 0x40490fdb */
  static constexpr float pi_lo   = -8.7422776573e-08; /* 0xb3bbbd2e */

  const int32_t hx = get_float_as_int(x);
  const int32_t ix = hx & 0x7fffffff;
  const int32_t hy = get_float_as_int(y);
  const int32_t iy = hy & 0x7fffffff;
  if ((ix > 0x7f800000) || (iy > 0x7f800000))  /* x or y is NaN */
    return x + y;
  if (hx == 0x3f800000) return atanf(y);  /* x=1.0 */
  int32_t m = ((hy >> 31) & 1) | ((hx >> 30) & 2);  /* 2*sign(x)+sign(y) */

  /* when y = 0 */
  if (iy == 0) {
    switch(m) {
    case 0:
    case 1:
      return y;  /* atan(+-0,+anything)=+-0 */
    case 2:
      return  pi + tiny;  /* atan(+0,-anything) = pi */
    case 3:
      return -pi - tiny;  /* atan(-0,-anything) =-pi */
    }
  }
  /* when x = 0 */
  if (ix == 0) return (hy < 0 ? -pi_o_2 - tiny : pi_o_2 + tiny);

  /* when x is INF */
  if (ix == 0x7f800000) {
    if (iy == 0x7f800000) {
      switch (m) {
      case 0:
        return  pi_o_4+tiny;  /* atan(+INF,+INF) */
      case 1:
        return -pi_o_4-tiny;  /* atan(-INF,+INF) */
      case 2:
        return  3.0f * pi_o_4 + tiny;  /*atan(+INF,-INF)*/
      case 3:
        return -3.0f * pi_o_4 - tiny;  /*atan(-INF,-INF)*/
      }
    } else {
      switch(m) {
      case 0:
        return  zero;  /* atan(+...,+INF) */
      case 1:
        return -zero;  /* atan(-...,+INF) */
      case 2:
        return  pi + tiny;  /* atan(+...,-INF) */
      case 3:
        return -pi - tiny;  /* atan(-...,-INF) */
      }
    }
  }
  /* when y is INF */
  if (iy == 0x7f800000) return (hy < 0 ? -pi_o_2 - tiny : pi_o_2 + tiny);

  /* compute y/x */
  const int32_t k = (iy - ix) >> 23;
  float z;
  if (k > 26) {  /* |y/x| >  2**26 */
    z = pi_o_2 + 0.5f * pi_lo;
    m &= 1;
  } else if (k < -26 && hx < 0) {
    z = 0.0;  /* 0 > |y|/x > -2**-26 */
  } else {
    z = atanf(fabsf(y / x));  /* safe to do y/x */
  }
  switch (m) {
  case 0:
    return  z;  /* atan(+,+) */
  case 1:
    return -z;  /* atan(-,+) */
  case 2:
    return  pi - (z - pi_lo);  /* atan(+,-) */
  default:  /* case 3 */
    return  (z - pi_lo) - pi;  /* atan(-,-) */
  }
}
