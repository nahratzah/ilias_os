/* e_remainderf.c -- float version of e_remainder.c.
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

//__FBSDID("$FreeBSD: src/lib/msun/src/e_remainderf.c,v 1.8 2008/02/12 17:11:36 bde Exp $");

#include <cmath>
#include "private.h"

using namespace _namespace(std);

float remainderf(float x, float p) noexcept {
  static constexpr float zero = 0.0;

  int32_t hx = get_float_as_int(x);
  int32_t hp = get_float_as_int(p);
  const uint32_t sx = hx & 0x80000000;
  hp &= 0x7fffffff;
  hx &= 0x7fffffff;

  /* purge off exception values */
  if (hp == 0) return (x * p) / (x * p);  /* p = 0 */
  if (hx >= 0x7f800000 ||  /* x not finite */
      hp > 0x7f800000)  /* p is NaN */
    return (static_cast<long double>(x) * p) /
           (static_cast<long double>(x) * p);

  if (hp <= 0x7effffff) x = fmodf(x, p + p);  /* now x < 2p */
  if (hx - hp == 0) return zero * x;
  x = fabsf(x);
  p = fabsf(p);
  if (hp < 0x01000000) {
    if (x + x > p) {
      x -= p;
      if (x + x >= p) x -= p;
    }
  } else {
    const float p_half = 0.5f * p;
    if (x > p_half) {
      x -= p;
      if (x >= p_half) x -= p;
    }
  }
  hx = get_float_as_int(x);
  if ((hx & 0x7fffffff) == 0) hx = 0;
  x = set_float_from_int(hx ^ sx);
  return x;
}
