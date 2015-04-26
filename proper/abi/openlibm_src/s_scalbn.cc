/* @(#)s_scalbn.c 5.1 93/09/24 */
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

/*
 * scalbn (double x, int n)
 * scalbn(x,n) returns x* 2**n  computed by  exponent
 * manipulation rather than by actually performing an
 * exponentiation or a multiplication.
 */

#include <cmath>
#include "private.h"

_namespace_begin(std)

double scalbn(double x, int n) noexcept {
  static constexpr double two54  = 1.80143985094819840000e+16;  /* 0x43500000, 0x00000000 */
  static constexpr double twom54 = 5.55111512312578270212e-17;  /* 0x3C900000, 0x00000000 */
  static constexpr double huge   = 1.0e+300;
  static constexpr double tiny   = 1.0e-300;

  int32_t hx, lx;
  tie(hx, lx) = extract_words(x);
  int32_t k = (hx & 0x7ff00000) >> 20;  /* extract exponent */
  if (k == 0) {  /* 0 or subnormal x */
    if ((lx | (hx & 0x7fffffff)) == 0) return x;  /* +-0 */
    x *= two54;
    hx = get<0>(extract_words(x));
    k = ((hx & 0x7ff00000) >> 20) - 54;
    if (n < -50000) return tiny * x;  /*underflow*/
  }
  if (k == 0x7ff) return x + x;  /* NaN or Inf */
  k = k + n;
  if (k >  0x7fe) return huge * copysign(huge, x);  /* overflow  */
  if (k > 0)  /* normal result */
    return set_high_word(x,(hx & 0x800fffff) | (k << 20));
  if (k <= -54) {
    if (n > 50000)  /* in case integer overflow in n+k */
      return huge * copysign(huge, x);  /*overflow*/
    else
      return tiny * copysign(tiny, x);  /*underflow*/
  }
  k += 54;  /* subnormal result */
  x = set_high_word(x, (hx & 0x800fffff) | (k << 20));
  return x * twom54;
}

double ldexp(double x, int n) noexcept {
  return scalbn(x, n);
}

_namespace_end(std)
