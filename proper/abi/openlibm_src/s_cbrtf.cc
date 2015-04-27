/* s_cbrtf.c -- float version of s_cbrt.c.
 * Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com.
 * Debugged and optimized by Bruce D. Evans.
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

//__FBSDID("$FreeBSD: src/lib/msun/src/s_cbrtf.c,v 1.18 2008/02/22 02:30:35 das Exp $");

#include <cmath>
#include "private.h"

using namespace _namespace(std);

/* cbrtf(x)
 * Return cube root of x
 */
float cbrtf(float x) noexcept {
  static constexpr uint32_t B1 = 709958130;  /* B1 = (127-127.0/3-0.03306235651)*2**23 */
  static constexpr uint32_t B2 = 642849266;  /* B2 = (127-127.0/3-24/3-0.03306235651)*2**23 */

  int32_t hx = get_float_as_int(x);
  uint32_t sign = hx & 0x80000000;  /* sign= sign(x) */
  hx ^= sign;
  if (hx >= 0x7f800000) return x + x;  /* cbrt(NaN,INF) is itself */

  /* rough cbrt to 5 bits */
  float t;
  if (hx < 0x00800000) {  /* zero or subnormal? */
    if (hx == 0) return x;  /* cbrt(+-0) is itself */
    t = set_float_from_int(0x4b800000);  /* set t= 2**24 */
    t *= x;
    uint32_t high = get_float_as_int(t);
    t = set_float_from_int(sign | ((high & 0x7fffffff) / 3 + B2));
  } else {
    t = set_float_from_int(sign | (hx / 3 + B1));
  }

  /*
   * First step Newton iteration (solving t*t-x/t == 0) to 16 bits.  In
   * double precision so that its terms can be arranged for efficiency
   * without causing overflow or underflow.
   */
  double T = t;
  double r = T * T * T;
  T = T * (static_cast<double>(x) + x + r) / (x + r + r);

  /*
   * Second step Newton iteration to 47 bits.  In double precision for
   * efficiency and accuracy.
   */
  r = T * T * T;
  T = T * (static_cast<double>(x) + x + r) / (x + r + r);

  /* rounding to 24 bits is perfect in round-to-nearest mode */
  return T;
}
