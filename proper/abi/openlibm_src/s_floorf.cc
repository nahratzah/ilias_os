/* s_floorf.c -- float version of s_floor.c.
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

//__FBSDID("$FreeBSD: src/lib/msun/src/s_floorf.c,v 1.8 2008/02/22 02:30:35 das Exp $");

/*
 * floorf(x)
 * Return x rounded toward -inf to integral value
 * Method:
 *      Bit twiddling.
 * Exception:
 *      Inexact flag raised if x not equal to floorf(x).
 */

#include <cmath>
#include "private.h"

using namespace _namespace(std);

float floorf(float x) noexcept {
  static constexpr float huge = 1.0e30;

  int32_t i0 = get_float_as_int(x);
  const int32_t j0 = ((i0 >> 23) & 0xff) - 0x7f;
  if (j0 < 23) {
    if (j0 < 0) {  /* raise inexact if x != 0 */
      if (huge + x > 0.0f) {  /* return 0*sign(x) if |x|<1 */
        if (i0 >= 0) {
          i0 = 0;
        } else if ((i0 & 0x7fffffff) != 0) {
          i0 = 0xbf800000;
        }
      }
    } else {
      const uint32_t i = 0x007fffff >> j0;
      if ((i0 & i) == 0) return x;  /* x is integral */
      if (huge + x > 0.0f) {  /* raise inexact flag */
        if (i0 < 0) i0 += 0x00800000 >> j0;
        i0 &= ~i;
      }
    }
  } else {
    if (j0 == 0x80)
      return x + x;  /* inf or NaN */
    else
      return x;  /* x is integral */
  }
  return set_float_from_int(i0);
}
