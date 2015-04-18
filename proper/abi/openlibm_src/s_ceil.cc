/* @(#)s_ceil.c 5.1 93/09/24 */
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

//__FBSDID("$FreeBSD: src/lib/msun/src/s_ceil.c,v 1.11 2008/02/15 07:01:40 bde Exp $");

/*
 * ceil(x)
 * Return x rounded toward -inf to integral value
 * Method:
 *	Bit twiddling.
 * Exception:
 *	Inexact flag raised if x not equal to ceil(x).
 */

#include <cmath>
#include "private.h"

_namespace_begin(std)


double ceil(double x) noexcept {
  static constexpr double huge = 1.0e300;

  int32_t i0, i1;
  tie(i0, i1) = extract_words(x);
  const int32_t j0 = ((i0 >> 20) & 0x7ff) - 0x3ff;
  if (j0 < 20) {
    if (j0 < 0) {  /* raise inexact if x != 0 */
      if (huge + x > 0.0) {  /* return 0*sign(x) if |x|<1 */
        if (i0 < 0) {
          i0 = 0x80000000;
	  i1 = 0;
	} else if ((i0 | i1) !=0) {
          i0 = 0x3ff00000;
	  i1 = 0;
	}
      }
    } else {
      const uint32_t i = 0x000fffff >> j0;
      if (((i0 & i) | i1) == 0) return x;  /* x is integral */
      if (huge + x > 0.0) {  /* raise inexact flag */
        if (i0 > 0) i0 += 0x00100000 >> j0;
	i0 &= ~i;
	i1 = 0;
      }
    }
  } else if (j0 > 51) {
    if (j0 == 0x400)
      return x + x;  /* inf or NaN */
    else
      return x;  /* x is integral */
  } else {
    const uint32_t i = 0xffffffffU >> (j0 - 20);
    if ((i1 & i) == 0) return x;  /* x is integral */
    if (huge + x > 0.0) {  /* raise inexact flag */
      if (i0 > 0) {
        if (j0 == 20) {
          i0 += 1;
	} else {
          const uint32_t j = i1 + (1 << (52 - j0));
	  if (static_cast<int32_t>(j) < i1) i0 += 1;  /* got a carry */
	  i1 = j;
	}
      }
      i1 &= ~i;
    }
  }
  return set_double_from_words(i0, i1);
}


_namespace_end(std)
