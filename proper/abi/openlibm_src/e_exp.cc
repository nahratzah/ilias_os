
/* @(#)e_exp.c 1.6 04/04/22 */
/*
 * ====================================================
 * Copyright (C) 2004 by Sun Microsystems, Inc. All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 */

//__FBSDID("$FreeBSD: src/lib/msun/src/e_exp.c,v 1.14 2011/10/21 06:26:38 das Exp $");

/* __ieee754_exp(x)
 * Returns the exponential of x.
 *
 * Method
 *   1. Argument reduction:
 *      Reduce x to an r so that |r| <= 0.5*ln2 ~ 0.34658.
 *	Given x, find r and integer k such that
 *
 *               x = k*ln2 + r,  |r| <= 0.5*ln2.
 *
 *      Here r will be represented as r = hi-lo for better
 *	accuracy.
 *
 *   2. Approximation of exp(r) by a special rational function on
 *	the interval [0,0.34658]:
 *	Write
 *          R(r**2) = r*(exp(r)+1)/(exp(r)-1) = 2 + r*r/6 - r**4/360 + ...
 *      We use a special Remes algorithm on [0,0.34658] to generate
 *      a polynomial of degree 5 to approximate R. The maximum error
 *	of this polynomial approximation is bounded by 2**-59. In
 *	other words,
 *          R(z) ~ 2.0 + P1*z + P2*z**2 + P3*z**3 + P4*z**4 + P5*z**5
 *      (where z=r*r, and the values of P1 to P5 are listed below)
 *	and
 *          |                  5          |     -59
 *          | 2.0+P1*z+...+P5*z   -  R(z) | <= 2
 *          |                             |
 *	The computation of exp(r) thus becomes
 *                             2*r
 *		exp(r) = 1 + -------
 *                            R - r
 *                                 r*R1(r)
 *                     = 1 + r + ----------- (for better accuracy)
 *                                2 - R1(r)
 *	where
 *                               2       4             10
 *		R1(r) = r - (P1*r  + P2*r  + ... + P5*r   ).
 *
 *   3. Scale back to obtain exp(x):
 *	From step 1, we have
 *         exp(x) = 2^k * exp(r)
 *
 * Special cases:
 *	exp(INF) is INF, exp(NaN) is NaN;
 *	exp(-INF) is 0, and
 *	for finite argument, only exp(0)=1 is exact.
 *
 * Accuracy:
 *	according to an error analysis, the error is always less than
 *	1 ulp (unit in the last place).
 *
 * Misc. info.
 *	For IEEE double
 *          if x >  7.09782712893383973096e+02 then exp(x) overflow
 *          if x < -7.45133219101941108420e+02 then exp(x) underflow
 *
 * Constants:
 * The hexadecimal values are the intended ones for the following
 * constants. The decimal values may be used, provided that the
 * compiler will convert from decimal to binary accurately enough
 * to produce the hexadecimal values shown.
 */

#include <cmath>
#include <array>
#include "private.h"

_namespace_begin(std)


double exp(double x) noexcept {
  static constexpr double one = 1.0;
  static constexpr array<double, 2> halF{{ 0.5, -0.5 }};
  static constexpr double huge = 1.0e+300;
  static constexpr double o_threshold = 7.09782712893383973096e+02;  /* 0x40862E42, 0xFEFA39EF */
  static constexpr double u_threshold= -7.45133219101941108420e+02;  /* 0xc0874910, 0xD52D3051 */
  static constexpr array<double, 2> ln2HI      {{ 6.93147180369123816490e-01,  /* 0x3fe62e42, 0xfee00000 */
                                                 -6.93147180369123816490e-01 }};  /* 0xbfe62e42, 0xfee00000 */
  static constexpr array<double, 2> ln2LO      {{ 1.90821492927058770002e-10,  /* 0x3dea39ef, 0x35793c76 */
                                                 -1.90821492927058770002e-10 }};  /* 0xbdea39ef, 0x35793c76 */
  static constexpr double invln2 =  1.44269504088896338700e+00; /* 0x3ff71547, 0x652b82fe */
  static constexpr double P1   =  1.66666666666666019037e-01; /* 0x3FC55555, 0x5555553E */
  static constexpr double P2   = -2.77777777770155933842e-03; /* 0xBF66C16C, 0x16BEBD93 */
  static constexpr double P3   =  6.61375632143793436117e-05; /* 0x3F11566A, 0xAF25DE2C */
  static constexpr double P4   = -1.65339022054652515390e-06; /* 0xBEBBBD41, 0xC5D26BF1 */
  static constexpr double P5   =  4.13813679705723846039e-08; /* 0x3E663769, 0x72BEA4D0 */
  static constexpr double twom1000= 9.33263618503218878990e-302;  /* 2**-1000=0x01700000,0*/

  uint32_t hx = get<0>(extract_words(x));
  const int32_t xsb = (hx >> 31) & 1;  /* sign bit of x */
  hx &= 0x7fffffff;  /* high word of |x| */

  /* filter out non-finite argument */
  if (hx >= 0x40862E42) {  /* if |x|>=709.78... */
    if (hx >= 0x7ff00000) {
      const uint32_t lx = get<1>(extract_words(x));
      if (((hx & 0xfffff) | lx) != 0)
        return x + x;  /* NaN */
      else
        return (xsb == 0 ? x : 0.0);  /* exp(+-inf)={inf,0} */
    }
    if (x > o_threshold) return huge * huge;  /* overflow */
    if (x < u_threshold) return twom1000 * twom1000;  /* underflow */
  }

  /* this implementation gives 2.7182818284590455 for exp(1.0),
   * which is well within the allowable error. however,
   * 2.718281828459045 is closer to the true value so we prefer that
   * answer, given that 1.0 is such an important argument value. */
  if (x == 1.0)
    return M_E;

  /* argument reduction */
  int32_t k = 0;
  double hi = 0, lo = 0;
  if (hx > 0x3fd62e42) {  /* if  |x| > 0.5 ln2 */
    if (hx < 0x3FF0A2B2) {  /* and |x| < 1.5 ln2 */
      hi = x - ln2HI[xsb];
      lo = ln2LO[xsb];
      k = 1 - xsb - xsb;
    } else {
      k  = static_cast<int>(invln2 * x + halF[xsb]);
      const double t = k;
      hi = x - t*ln2HI[0];  /* t*ln2HI is exact here */
      lo = t * ln2LO[0];
    }
    x = static_cast<double>(hi - lo);
  } else if (hx < 0x3e300000) {  /* when |x|<2**-28 */
    if (huge + x > one)
      return one + x;  /* trigger inexact */
  }

  /* x is now in primary range */
  const double t = x * x;
  const double twopk =
      set_double_from_words(0x3ff00000 + ((k >= -1021 ? k : k + 1000) << 20),
                            0);
  const double c = x - t * (P1 + t * (P2 + t * (P3 + t * (P4 + t * P5))));
  if (k==0)
    return one - ((x * c) / (c - 2.0) - x);
  const double y = one - ((lo - (x * c) / (2.0 - c)) - hi);
  if (k >= -1021) {
    if (k==1024) return y * 2.0 * 0x1p1023;
    return y*twopk;
  } else {
    return y * twopk * twom1000;
  }
}


_namespace_end(std)
