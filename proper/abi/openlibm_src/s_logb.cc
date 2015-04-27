/* @(#)s_logb.c 5.1 93/09/24 */
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

//__FBSDID("$FreeBSD: src/lib/msun/src/s_logb.c,v 1.12 2008/02/08 01:22:13 bde Exp $");

/*
 * double logb(x)
 * IEEE 754 logb. Included to pass IEEE test suite. Not recommend.
 * Use ilogb instead.
 */

#include <cmath>
#include "private.h"

_namespace_begin(std)


double logb(double x) noexcept {
  static constexpr double two54 = 1.80143985094819840000e+16;  /* 43500000 00000000 */

  int32_t lx, ix;
  tie(ix, lx) = extract_words(x);
  ix &= 0x7fffffff;  /* high |x| */
  if ((ix | lx) == 0) return -1.0 / fabs(x);
  if (ix >= 0x7ff00000) return x * x;
  if (ix < 0x00100000) {
    x *= two54;  /* convert subnormal x to normal */
    ix = get<0>(extract_words(x));
    ix &= 0x7fffffff;
    return static_cast<double>((ix >> 20) - 1023 - 54);
  } else {
    return static_cast<double>((ix >> 20) - 1023);
  }
}


_namespace_end(std)
