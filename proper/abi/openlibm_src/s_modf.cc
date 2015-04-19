/* @(#)s_modf.c 5.1 93/09/24 */
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
 * modf(double x, double *iptr)
 * return fraction part of x, and return x's integral part in *iptr.
 * Method:
 *      Bit twiddling.
 *
 * Exception:
 *      No exception.
 */

#include <cmath>
#include "private.h"

_namespace_begin(std)


double modf(double x, double *iptr) noexcept {
  static constexpr double one = 1.0;

  int32_t i0, i1;
  tie(i0, i1) = extract_words(x);
  const int32_t j0 = ((i0 >> 20) & 0x7ff) - 0x3ff;  /* exponent of x */
  if (j0 < 20) {  /* integer part in high x */
    if (j0 < 0) {  /* |x|<1 */
      *iptr = set_double_from_words(i0 & 0x80000000, 0);  /* *iptr = +-0 */
      return x;
    } else {
      const uint32_t i = 0x000fffffU >> j0;
      if (((i0 & i) | i1) == 0) {  /* x is integral */
        *iptr = x;
        const uint32_t high = get<0>(extract_words(x));
        x = set_double_from_words(high & 0x80000000, 0);  /* return +-0 */
        return x;
      } else {
        *iptr = set_double_from_words(i0 & (~i), 0);
        return x - *iptr;
      }
    }
  } else if (j0 > 51) {  /* no fraction part */
    if (j0 == 0x400) {  /* inf/NaN */
      *iptr = x;
      return 0.0 / x;
    }
    *iptr = x * one;
    const uint32_t high = get<0>(extract_words(x));
    x = set_double_from_words(high & 0x80000000, 0);  /* return +-0 */
    return x;
  } else {  /* fraction part in low x */
    const uint32_t i = 0xffffffffU >> (j0 - 20);
    if ((i1 & i) == 0) {  /* x is integral */
      *iptr = x;
      const uint32_t high = get<0>(extract_words(x));
      x = set_double_from_words(high & 0x80000000, 0);  /* return +-0 */
      return x;
    } else {
      *iptr = set_double_from_words(i0, i1 & (~i));
      return x - *iptr;
    }
  }
}


_namespace_end(std)
