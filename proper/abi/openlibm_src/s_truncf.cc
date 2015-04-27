/* @(#)s_floor.c 5.1 93/09/24 */
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

//__FBSDID("$FreeBSD: src/lib/msun/src/s_truncf.c,v 1.1 2004/06/20 09:25:43 das Exp $");

/*
 * truncf(x)
 * Return x rounded toward 0 to integral value
 * Method:
 *	Bit twiddling.
 * Exception:
 *	Inexact flag raised if x not equal to truncf(x).
 */

#include <cmath>
#include "private.h"

using namespace _namespace(std);

float truncf(float x) noexcept {
  static constexpr float huge = 1.0e30F;

  int32_t i0 = get_float_as_int(x);
  int32_t j0 = ((i0 >> 23) & 0xff) - 0x7f;
  if (j0 < 23) {
    if (j0 < 0) {  /* raise inexact if x != 0 */
      if (huge + x > 0.0F)  /* |x|<1, so return 0*sign(x) */
        i0 &= 0x80000000;
    } else {
      const uint32_t i = 0x007fffff >> j0;
      if ((i0 & i) == 0) return x;  /* x is integral */
      if (huge + x > 0.0F)  /* raise inexact flag */
        i0 &= (~i);
    }
  } else {
    if (j0 == 0x80)
      return x + x;  /* inf or NaN */
    else
      return x;  /* x is integral */
  }
  return set_float_from_int(i0);
}
