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

//__FBSDID("$FreeBSD: src/lib/msun/src/k_logf.h,v 1.3 2011/10/15 05:23:28 das Exp $");

#include <cmath>
#include "private.h"

_namespace_begin(std)
namespace impl {


/*
 * Float version of k_log.h.  See the latter for most comments.
 */
float k_log1pf(float f) noexcept {
  /* |(log(1+s)-log(1-s))/s - Lg(s)| < 2**-34.24 (~[-4.95e-11, 4.97e-11]). */
  static constexpr float Lg1 =      0xaaaaaa.0p-24;  /* 0.66666662693 */
  static constexpr float Lg2 =      0xccce13.0p-25;  /* 0.40000972152 */
  static constexpr float Lg3 =      0x91e9ee.0p-25;  /* 0.28498786688 */
  static constexpr float Lg4 =      0xf89e26.0p-26;  /* 0.24279078841 */

  const float s = f / (2.0f + f);
  const float z = s * s;
  const float w = z * z;
  const float t1= w * (Lg2 + w * Lg4);
  const float t2= z * (Lg1 + w * Lg3);
  const float R = t2+t1;
  const float hfsq = 0.5f * f * f;
  return s*(hfsq+R);
}


} /* namespace impl */
_namespace_end(std)
