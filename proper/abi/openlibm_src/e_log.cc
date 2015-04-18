
/* @(#)e_log.c 1.3 95/01/18 */
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

//__FBSDID("$FreeBSD: src/lib/msun/src/e_log.c,v 1.15 2008/03/29 16:37:59 das Exp $");

/* __ieee754_log(x)
 * Return the logrithm of x
 *
 * Method :                  
 *   1. Argument Reduction: find k and f such that 
 *			x = 2^k * (1+f), 
 *	   where  sqrt(2)/2 < 1+f < sqrt(2) .
 *
 *   2. Approximation of log(1+f).
 *	Let s = f/(2+f) ; based on log(1+f) = log(1+s) - log(1-s)
 *		 = 2s + 2/3 s**3 + 2/5 s**5 + .....,
 *	     	 = 2s + s*R
 *      We use a special Reme algorithm on [0,0.1716] to generate 
 * 	a polynomial of degree 14 to approximate R The maximum error 
 *	of this polynomial approximation is bounded by 2**-58.45. In
 *	other words,
 *		        2      4      6      8      10      12      14
 *	    R(z) ~ Lg1*s +Lg2*s +Lg3*s +Lg4*s +Lg5*s  +Lg6*s  +Lg7*s
 *  	(the values of Lg1 to Lg7 are listed in the program)
 *	and
 *	    |      2          14          |     -58.45
 *	    | Lg1*s +...+Lg7*s    -  R(z) | <= 2 
 *	    |                             |
 *	Note that 2s = f - s*f = f - hfsq + s*hfsq, where hfsq = f*f/2.
 *	In order to guarantee error in log below 1ulp, we compute log
 *	by
 *		log(1+f) = f - s*(f - R)	(if f is not too large)
 *		log(1+f) = f - (hfsq - s*(hfsq+R)).	(better accuracy)
 *	
 *	3. Finally,  log(x) = k*ln2 + log(1+f).  
 *			    = k*ln2_hi+(f-(hfsq-(s*(hfsq+R)+k*ln2_lo)))
 *	   Here ln2 is split into two floating point number: 
 *			ln2_hi + ln2_lo,
 *	   where n*ln2_hi is always exact for |n| < 2000.
 *
 * Special cases:
 *	log(x) is NaN with signal if x < 0 (including -INF) ; 
 *	log(+INF) is +INF; log(0) is -INF with signal;
 *	log(NaN) is that NaN with no signal.
 *
 * Accuracy:
 *	according to an error analysis, the error is always less than
 *	1 ulp (unit in the last place).
 *
 * Constants:
 * The hexadecimal values are the intended ones for the following 
 * constants. The decimal values may be used, provided that the 
 * compiler will convert from decimal to binary accurately enough 
 * to produce the hexadecimal values shown.
 */

#include <cmath>
#include "private.h"

_namespace_begin(std)


double log(double x) noexcept {
  static constexpr double ln2_hi =  6.93147180369123816490e-01;  /* 3fe62e42 fee00000 */
  static constexpr double ln2_lo =  1.90821492927058770002e-10;  /* 3dea39ef 35793c76 */
  static constexpr double two54 =  1.80143985094819840000e+16;  /* 43500000 00000000 */
  static constexpr double Lg1 = 6.666666666666735130e-01;  /* 3FE55555 55555593 */
  static constexpr double Lg2 = 3.999999999940941908e-01;  /* 3FD99999 9997FA04 */
  static constexpr double Lg3 = 2.857142874366239149e-01;  /* 3FD24924 94229359 */
  static constexpr double Lg4 = 2.222219843214978396e-01;  /* 3FCC71C5 1D8E78AF */
  static constexpr double Lg5 = 1.818357216161805012e-01;  /* 3FC74664 96CB03DE */
  static constexpr double Lg6 = 1.531383769920937332e-01;  /* 3FC39A09 D078C69F */
  static constexpr double Lg7 = 1.479819860511658591e-01;  /* 3FC2F112 DF3E5244 */
  static constexpr double zero = 0.0;

#if 0
	double hfsq,f,s,z,R,w,t1,t2,dk;
	int32_t k,hx,i,j;
	u_int32_t lx;
#endif

  int32_t hx;
  uint32_t lx;
  tie(hx, lx) = extract_words(x);

  int32_t k = 0;
  if (hx < 0x00100000) {  /* x < 2**-1022  */
    if (((hx & 0x7fffffff) | lx) == 0)
      return -two54 / zero;  /* log(+-0)=-inf */
    if (hx < 0)
      return (x - x) / zero;  /* log(-#) = NaN */
    k -= 54;
    x *= two54;  /* subnormal number, scale up x */
    hx = get<0>(extract_words(x));
  }
  if (hx >= 0x7ff00000) return x + x;
  k += (hx >> 20) - 1023;
  hx &= 0x000fffff;
  {
    const int32_t i = (hx + 0x95f64) & 0x100000;
    x = set_high_word(x, hx | (i ^ 0x3ff00000));  /* normalize x or x/2 */
    k += (i >> 20);
  }
  const double f = x - 1.0;
  if ((0x000fffff & (2 + hx)) < 3) {  /* -2**-20 <= f < 2**-20 */
    if (f == zero) {
      if (k == 0) {
        return zero;
      } else {
        const double dk = static_cast<double>(k);
        return dk * ln2_hi + dk * ln2_lo;
      }
    }
    const double R = f * f * (0.5 - 0.33333333333333333 * f);
    if (k == 0)
      return f - R;
    else {
      const double dk = static_cast<double>(k);
      return dk * ln2_hi - ((R - dk * ln2_lo) - f);
    }
  }
  const double s = f / (2.0 + f);
  const double dk = static_cast<double>(k);
  const double z = s * s;
  int32_t i = hx - 0x6147a;
  const double w = z * z;
  const int32_t j = 0x6b851 - hx;
  const double t1 = w * (Lg2 + w * (Lg4 + w * Lg6));
  const double t2 = z * (Lg1 + w * (Lg3 + w * (Lg5 + w * Lg7)));
  i |= j;
  const double R = t2 + t1;
  if (i > 0) {
    const double hfsq = 0.5 * f * f;
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


_namespace_end(std)
