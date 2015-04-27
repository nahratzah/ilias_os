/* s_scalbnf.c -- float version of s_scalbn.c.
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


#include <cmath>
#include "private.h"

using namespace _namespace(std);

float scalbnf(float x, int n) noexcept {
  static constexpr float two25  =  3.355443200e+07;  /* 0x4c000000 */
  static constexpr float twom25 =  2.9802322388e-08;  /* 0x33000000 */
  static constexpr float huge   = 1.0e+30;
  static constexpr float tiny   = 1.0e-30;

#if 0
	int32_t k,ix;
#endif
  int32_t ix = get_float_as_int(x);
  int32_t k = (ix & 0x7f800000) >> 23;  /* extract exponent */
  if (k == 0) {  /* 0 or subnormal x */
    if ((ix & 0x7fffffff) == 0) return x;  /* +-0 */
    x *= two25;
    ix = get_float_as_int(x);
    k = ((ix & 0x7f800000) >> 23) - 25;
    if (n < -50000) return tiny * x;  /*underflow*/
  }
  if (k == 0xff) return x + x;  /* NaN or Inf */
  k = k + n;
  if (k > 0xfe) return huge * copysignf(huge, x);  /* overflow  */
  if (k > 0)  /* normal result */
    return set_float_from_int((ix & 0x807fffff) | (k << 23));
  if (k <= -25) {
    if (n > 50000)  /* in case integer overflow in n+k */
      return huge * copysignf(huge, x);  /*overflow*/
    else
      return tiny * copysignf(tiny, x);  /*underflow*/
  }
  k += 25;  /* subnormal result */
  x = set_float_from_int((ix & 0x807fffff) | (k << 23));
  return x * twom25;
}

float ldexpf(float x, int n) noexcept {
  return scalbnf(x, n);
}
