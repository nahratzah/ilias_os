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

//__FBSDID("$FreeBSD: src/lib/msun/src/e_log10f.c,v 1.13 2011/10/16 05:36:23 das Exp $");

/*
 * Float version of e_log10.c.  See the latter for most comments.
 */

#include <cmath>
#include "private.h"

using namespace _namespace(std);

float log10f(float x) noexcept {
  using _namespace(std)::impl::k_log1pf;

  static constexpr float two25      =  3.3554432000e+07;  /* 0x4c000000 */
  static constexpr float ivln10hi   =  4.3432617188e-01;  /* 0x3ede6000 */
  static constexpr float ivln10lo   = -3.1689971365e-05;  /* 0xb804ead9 */
  static constexpr float log10_2hi  =  3.0102920532e-01;  /* 0x3e9a2080 */
  static constexpr float log10_2lo  =  7.9034151668e-07;  /* 0x355427db */
  static constexpr float zero       =  0.0;

#if 0
	float f,hfsq,hi,lo,r,y;
	int32_t i,k,hx;
#endif

  int32_t hx = get_float_as_int(x);

  int32_t k = 0;
  if (hx < 0x00800000) {  /* x < 2**-126  */
    if ((hx & 0x7fffffff) == 0) return -two25 / zero;  /* log(+-0)=-inf */
    if (hx<0) return (x - x) / zero;  /* log(-#) = NaN */
    /* subnormal number, scale up x */
    k -= 25;
    x *= two25;
    hx = get_float_as_int(x);
  }
  if (hx >= 0x7f800000) return x + x;
  if (hx == 0x3f800000) return zero;  /* log(1) = +0 */
  k += (hx >> 23) - 127;
  hx &= 0x007fffff;
  const int32_t i = (hx + 0x4afb0d) & 0x800000;
  x = set_float_from_int(hx | (i ^ 0x3f800000));  /* normalize x or x/2 */
  k += i >> 23;
  const float y = static_cast<float>(k);
  const float f = x - 1.0f;
  const float hfsq = 0.5f * f * f;
  const float r = k_log1pf(f);

  /* See e_log2f.c and e_log2.c for details. */
  const float hi = set_float_from_int(get_float_as_int(f - hfsq) & 0xfffff000);
  const float lo = (f - hi) - hfsq + r;
  return y * log10_2lo +
         (lo + hi) * ivln10lo +
         lo * ivln10hi +
         hi * ivln10hi +
         y * log10_2hi;
}
