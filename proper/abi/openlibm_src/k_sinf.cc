/* k_sinf.c -- float version of k_sin.c
 * Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com.
 * Optimized by Bruce D. Evans.
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

//__FBSDID("$FreeBSD: src/lib/msun/src/k_sinf.c,v 1.16 2009/06/03 08:16:34 ed Exp $");

#include "private.h"

_namespace_begin(std)
namespace impl {


float __kernel_sindf(double x) noexcept {
  /* |sin(x)/x - s(x)| < 2**-37.5 (~[-4.89e-12, 4.824e-12]). */
  static constexpr double S1 = -0x15555554cbac77.0p-55;  /* -0.166666666416265235595 */
  static constexpr double S2 =  0x111110896efbb2.0p-59;  /*  0.0083333293858894631756 */
  static constexpr double S3 = -0x1a00f9e2cae774.0p-65;  /* -0.000198393348360966317347 */
  static constexpr double S4 =  0x16cd878c3b46a7.0p-71;  /*  0.0000027183114939898219064 */

  /* Try to optimize for parallel evaluation as in k_tanf.c. */
  const double z = x*x;
  const double w = z*z;
  const double r = S3+z*S4;
  const double s = z*x;
  return (x + s*(S1+z*S2)) + s*w*r;
}


} /* namespace impl */
_namespace_end(std)
