/* s_frexpf.c -- float version of s_frexp.c.
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

//__FBSDID("$FreeBSD: src/lib/msun/src/s_frexpf.c,v 1.10 2008/02/22 02:30:35 das Exp $");

#include <cmath>
#include "private.h"

float frexpf(float x, int *eptr) noexcept {
  static constexpr float two25 =  3.3554432000e+07; /* 0x4c000000 */

  int32_t hx = get_float_as_int(x);
  int32_t ix = hx & 0x7fffffff;
  *eptr = 0;
  if (ix >= 0x7f800000 || (ix == 0)) return x;  /* 0,inf,nan */
  if (ix < 0x00800000) {  /* subnormal */
    x *= two25;
    hx = get_float_as_int(x);
    ix = hx & 0x7fffffff;
    *eptr = -25;
  }
  *eptr += (ix >> 23) - 126;
  hx = (hx & 0x807fffff) | 0x3f000000;
  x = set_float_from_int(hx);
  return x;
}
