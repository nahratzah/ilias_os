/* @(#)s_copysign.c 5.1 93/09/24 */
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

//__FBSDID("$FreeBSD: src/lib/msun/src/s_copysign.c,v 1.10 2008/02/22 02:30:35 das Exp $");

/*
 * copysign(double x, double y)
 * copysign(x,y) returns a value with the magnitude of x and
 * with the sign bit of y.
 */

#include <cmath>
#include "private.h"

_namespace_begin(std)


double copysign(double x, double y) noexcept {
  const uint32_t hx = get<0>(extract_words(x));
  const uint32_t hy = get<0>(extract_words(y));
  return set_high_word(x, (hx & 0x7fffffff) | (hy & 0x80000000));
}


_namespace_end(std)
