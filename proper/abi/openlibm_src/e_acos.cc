/* @(#)e_acos.c 1.3 95/01/18 */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunSoft, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 */

//__FBSDID("$FreeBSD: src/lib/msun/src/e_acos.c,v 1.13 2008/07/31 22:41:26 das Exp $");

/* __ieee754_acos(x)
 * Method:
 *      acos(x)  = pi/2 - asin(x)
 *      acos(-x) = pi/2 + asin(x)
 * For |x|<=0.5
 *      acos(x) = pi/2 - (x + x*x^2*R(x^2))      (see asin.cc)
 * For x>0.5
 *      acos(x) = pi/2 - (pi/2 - 2asin(sqrt((1-x)/2)))
 *              = 2asin(sqrt((1-x)/2))
 *              = 2s + 2s*z*R(z)        ...z=(1-x)/2, s=sqrt(z)
 *              = 2f + (2c + 2s*z*R(z))
 *     where f=hi part of s, and c = (z-f*f)/(s+f) is the correction term
 *     for f so that f+c ~ sqrt(z).
 * For x<-0.5
 *      acos(x) = pi - 2asin(sqrt((1-|x|)/2))
 *              = pi - 0.5*(s+s*z*R(z)), where z=(1-|x|)/2,s=sqrt(z)
 *
 * Special cases:
 *      if x is NaN, return x itself;
 *      if |x|>1, return NaN with invalid signal.
 *
 * Function needed: sqrt
 */

#include <cmath>
#include "private.h"

_namespace_begin(std)


double acos(double x) noexcept {
  static constexpr double one=  1.00000000000000000000e+00; /* 0x3FF00000, 0x00000000 */
  static constexpr double pi =  3.14159265358979311600e+00; /* 0x400921FB, 0x54442D18 */
  static constexpr double pio2_hi =  1.57079632679489655800e+00; /* 0x3FF921FB, 0x54442D18 */
  static constexpr double pio2_lo =  6.12323399573676603587e-17; /* 0x3C91A626, 0x33145C07 */
  static constexpr double pS0 =  1.66666666666666657415e-01; /* 0x3FC55555, 0x55555555 */
  static constexpr double pS1 = -3.25565818622400915405e-01; /* 0xBFD4D612, 0x03EB6F7D */
  static constexpr double pS2 =  2.01212532134862925881e-01; /* 0x3FC9C155, 0x0E884455 */
  static constexpr double pS3 = -4.00555345006794114027e-02; /* 0xBFA48228, 0xB5688F3B */
  static constexpr double pS4 =  7.91534994289814532176e-04; /* 0x3F49EFE0, 0x7501B288 */
  static constexpr double pS5 =  3.47933107596021167570e-05; /* 0x3F023DE1, 0x0DFDF709 */
  static constexpr double qS1 = -2.40339491173441421878e+00; /* 0xC0033A27, 0x1C8A2D4B */
  static constexpr double qS2 =  2.02094576023350569471e+00; /* 0x40002AE5, 0x9C598AC8 */
  static constexpr double qS3 = -6.88283971605453293030e-01; /* 0xBFE6066C, 0x1B8D0159 */
  static constexpr double qS4 =  7.70381505559019352791e-02; /* 0x3FB3B8C5, 0xB12E9282 */

  const int32_t hx = get<0>(extract_words(x));
  const int32_t ix = hx & 0x7fffffff;
  if (ix >= 0x3ff00000) {  /* |x| >= 1 */
    const u_int32_t lx = get<1>(extract_words(x));
    if (((ix - 0x3ff00000) | lx) == 0) {  /* |x|==1 */
      if (hx > 0)  /* acos(1) = 0  */
        return 0.0;
      else  /* acos(-1)= pi */
        return pi + 2.0 * pio2_lo;
    }
    return (x - x) / (x - x);  /* acos(|x|>1) is NaN */
  }
  if (ix < 0x3fe00000) {  /* |x| < 0.5 */
    if (ix <= 0x3c600000) return pio2_hi + pio2_lo;  /* if |x| < 2**-57 */
    const double z = x * x;
    const double p =
        z * (pS0 + z * (pS1 + z * (pS2 + z * (pS3 + z * (pS4 + z * pS5)))));
    const double q = one + z * (qS1 + z * (qS2 + z * (qS3 + z * qS4)));
    const double r = p / q;
    return pio2_hi - (x - (pio2_lo - x * r));
  } else if (hx < 0) {  /* x < -0.5 */
    const double z = (one + x) * 0.5;
    const double p =
        z * (pS0 + z * (pS1 + z * (pS2 + z * (pS3 + z * (pS4 + z * pS5)))));
    const double q = one + z * (qS1 + z * (qS2 + z * (qS3 + z * qS4)));
    const double s = sqrt(z);
    const double r = p / q;
    const double w = r * s - pio2_lo;
    return pi - 2.0 * (s + w);
  } else {  /* x > 0.5 */
    const double z = (one - x) * 0.5;
    const double s = sqrt(z);
    const double df = set_low_word(s, 0);
    const double c = (z - df * df) / (s + df);
    const double p =
        z * (pS0 + z * (pS1 + z * (pS2 + z * (pS3 + z * (pS4 + z * pS5)))));
    const double q = one + z * (qS1 + z * (qS2 + z * (qS3 + z * qS4)));
    const double r = p / q;
    const double w = r * s + c;
    return 2.0 * (df + w);
  }
}


_namespace_end(std)
