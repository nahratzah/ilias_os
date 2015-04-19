/* s_modff.c -- float version of s_modf.c.
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

//__FBSDID("$FreeBSD: src/lib/msun/src/s_modff.c,v 1.9 2008/02/22 02:30:35 das Exp $");

#include <cmath>
#include "private.h"

using namespace _namespace(std);

float modff(float x, float *iptr) noexcept {
  static constexpr float one = 1.0;

  const int32_t i0 = get_float_as_int(x);
  const int32_t j0 = ((i0 >> 23) & 0xff) - 0x7f;  /* exponent of x */
  if (j0 < 23) {  /* integer part in x */
    if (j0 < 0) {  /* |x|<1 */
      *iptr = set_float_from_int(i0 & 0x80000000);  /* *iptr = +-0 */
      return x;
    } else {
      const uint32_t i = 0x007fffffU >> j0;
      if ((i0 & i) == 0) {  /* x is integral */
        *iptr = x;
        uint32_t ix = get_float_as_int(x);
        x = set_float_from_int(ix & 0x80000000);  /* return +-0 */
        return x;
      } else {
        *iptr = set_float_from_int(i0 & (~i));
        return x - *iptr;
      }
    }
  } else {  /* no fraction part */
    *iptr = x * one;
    if (x != x)  /* NaN */
      return x;
    const uint32_t ix = get_float_as_int(x);
    x = set_float_from_int(ix & 0x80000000);  /* return +-0 */
    return x;
  }
}
