/* s_copysignf.c -- float version of s_copysign.c.
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

//__FBSDID("$FreeBSD: src/lib/msun/src/s_copysignf.c,v 1.10 2008/02/22 02:30:35 das Exp $");

/*
 * copysignf(float x, float y)
 * copysignf(x,y) returns a value with the magnitude of x and
 * with the sign bit of y.
 */

#include <cmath>
#include "private.h"

using namespace _namespace(std);

float copysignf(float x, float y) noexcept {
  const uint32_t ix = get_float_as_int(x);
  const uint32_t iy = get_float_as_int(y);
  return set_float_from_int((ix & 0x7fffffff) | (iy & 0x80000000));
}
