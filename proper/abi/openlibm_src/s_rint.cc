/* @(#)s_rint.c 5.1 93/09/24 */
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

//__FBSDID("$FreeBSD: src/lib/msun/src/s_rint.c,v 1.16 2008/02/22 02:30:35 das Exp $");

/*
 * rint(x)
 * Return x rounded to integral value according to the prevailing
 * rounding mode.
 * Method:
 *	Using floating addition.
 * Exception:
 *	Inexact flag raised if x not equal to rint(x).
 */

#include <cmath>
#include <array>
#include "private.h"

_namespace_begin(std)


double rint(double x) noexcept {
  static constexpr array<double, 2> TWO52{{
    4.50359962737049600000e+15, /* 0x43300000, 0x00000000 */
   -4.50359962737049600000e+15, /* 0xC3300000, 0x00000000 */
  }};

#if 0
	int32_t i0,j0,sx;
	u_int32_t i,i1;
	double w,t;
#endif

  int32_t i0;
  uint32_t i1;
  tie(i0, i1) = extract_words(x);
  const int32_t sx = (i0 >> 31) & 1;
  const int32_t j0 = ((i0 >> 20) & 0x7ff) - 0x3ff;
  if (j0 < 20) {
    if (j0 < 0) {
      if (((i0 & 0x7fffffff) | i1) == 0) return x;
      i1 |= (i0 & 0x0fffff);
      i0 &= 0xfffe0000;
      i0 |= ((i1 | -i1) >> 12) & 0x80000;
      x = set_high_word(x, i0);
      const double w = static_cast<double>(TWO52[sx] + x);
      double t = w - TWO52[sx];
      i0 = get<0>(extract_words(t));
      t = set_high_word(t, (i0 & 0x7fffffff) | (sx << 31));
      return t;
    } else {
      uint32_t i = 0x000fffff >> j0;
      if (((i0 & i) | i1) == 0) return x;  /* x is integral */
      i >>= 1;
      if (((i0 & i) | i1) != 0) {
      /*
       * Some bit is set after the 0.5 bit.  To avoid the
       * possibility of errors from double rounding in
       * w = TWO52[sx]+x, adjust the 0.25 bit to a lower
       * guard bit.  We do this for all j0<=51.  The
       * adjustment is trickiest for j0==18 and j0==19
       * since then it spans the word boundary.
       */
      if (j0 == 19)
        i1 = 0x40000000;
      else if (j0 == 18)
        i1 = 0x80000000;
      else
        i0 = (i0 & ~i) | (0x20000 >> j0);
      }
    }
  } else if (j0 > 51) {
    if (j0 == 0x400)
      return x + x;  /* inf or NaN */
    else
      return x;  /* x is integral */
  } else {
    uint32_t i = static_cast<uint32_t>(0xffffffff) >> (j0 - 20);
    if ((i1 & i) == 0) return x;  /* x is integral */
    i >>= 1;
    if ((i1 & i) != 0) i1 = (i1 & ~i) | (0x40000000 >> (j0 - 20));
  }
  x = set_double_from_words(i0, i1);
  const double w = static_cast<double>(TWO52[sx] + x);
  return w - TWO52[sx];
}


_namespace_end(std)
