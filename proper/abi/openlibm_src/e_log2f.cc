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

//__FBSDID("$FreeBSD: src/lib/msun/src/e_log2f.c,v 1.5 2011/10/15 05:23:28 das Exp $");

/*
 * Float version of e_log2.c.  See the latter for most comments.
 */

#include <cmath>
#include "private.h"

using namespace _namespace(std);

float log2f(float x) noexcept {
  using _namespace(std)::impl::k_log1pf;

  static constexpr float two25   =  3.3554432000e+07;  /* 0x4c000000 */
  static constexpr float ivln2hi =  1.4428710938e+00;  /* 0x3fb8b000 */
  static constexpr float ivln2lo = -1.7605285393e-04;  /* 0xb9389ad4 */
  static constexpr float zero    =  0.0;

  int32_t hx = get_float_as_int(x);

  int32_t k = 0;
  if (hx < 0x00800000) {  /* x < 2**-126  */
    if ((hx & 0x7fffffff) == 0) return -two25 / zero;  /* log(+-0)=-inf */
    if (hx < 0) return (x - x) / zero;  /* log(-#) = NaN */
    k -= 25;
    x *= two25;  /* subnormal number, scale up x */
    hx = get_float_as_int(x);
  }
  if (hx >= 0x7f800000) return x + x;
  if (hx == 0x3f800000) return zero;  /* log(1) = +0 */
  k += (hx >> 23) - 127;
  hx &= 0x007fffff;
  const int32_t i = (hx + (0x4afb0d)) & 0x800000;
  x = set_float_from_int(hx | (i ^ 0x3f800000));  /* normalize x or x/2 */
  k += i >> 23;
  const float y = static_cast<float>(k);
  const float f = x - 1.0f;
  const float hfsq = 0.5f * f * f;
  const float r = k_log1pf(f);

  /*
   * We no longer need to avoid falling into the multi-precision
   * calculations due to compiler bugs breaking Dekker's theorem.
   * Keep avoiding this as an optimization.  See e_log2.c for more
   * details (some details are here only because the optimization
   * is not yet available in double precision).
   *
   * Another compiler bug turned up.  With gcc on i386,
   * (ivln2lo + ivln2hi) would be evaluated in float precision
   * despite runtime evaluations using double precision.  So we
   * must cast one of its terms to float_t.  This makes the whole
   * expression have type float_t, so return is forced to waste
   * time clobbering its extra precision.
   */
  const float hi = set_float_from_int(get_float_as_int(f - hfsq) & 0xfffff000);
  const float lo = (f - hi) - hfsq + r;
  return (lo + hi) * ivln2lo + lo * ivln2hi + hi * ivln2hi + y;
}
