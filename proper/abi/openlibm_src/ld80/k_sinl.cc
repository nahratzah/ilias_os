/* From: @(#)k_sin.c 1.3 95/01/18 */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 * Copyright (c) 2008 Steven G. Kargl, David Schultz, Bruce D. Evans.
 *
 * Developed at SunSoft, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 */

//__FBSDID("$FreeBSD: src/lib/msun/ld80/k_sinl.c,v 1.1 2008/02/17 07:32:14 das Exp $");

/*
 * ld80 version of k_sin.c.  See ../src/k_sin.c for most comments.
 */

#include "../private.h"

_namespace_begin(std)
namespace impl {


long double __kernel_sinl(long double x, long double y, int iy) noexcept {
  static constexpr double half =  0.5;

  /*
   * Domain [-0.7854, 0.7854], range ~[-1.89e-22, 1.915e-22]
   * |sin(x)/x - s(x)| < 2**-72.1
   *
   * See ../ld80/k_cosl.c for more details about the polynomial.
   */
  static constexpr long double S1 = -0.166666666666666666671L;  /* -0xaaaaaaaaaaaaaaab.0p-66 */
  static constexpr double S2 =  0.0083333333333333332;  /*  0x11111111111111.0p-59 */
  static constexpr double S3 = -0.00019841269841269427;  /* -0x1a01a01a019f81.0p-65 */
  static constexpr double S4 =  0.0000027557319223597490;  /*  0x171de3a55560f7.0p-71 */
  static constexpr double S5 = -0.000000025052108218074604;  /* -0x1ae64564f16cad.0p-78 */
  static constexpr double S6 =  1.6059006598854211e-10;  /*  0x161242b90243b5.0p-85 */
  static constexpr double S7 = -7.6429779983024564e-13;  /* -0x1ae42ebd1b2e00.0p-93 */
  static constexpr double S8 =  2.6174587166648325e-15;  /*  0x179372ea0b3f64.0p-101 */

  const long double z = x * x;
  const long double v = z * x;
  const long double r =
      S2 + z * (S3 + z * (S4 + z * (S5 + z * (S6 + z * (S7 + z * S8)))));
  if (iy == 0)
    return x + v * (S1 + z * r);
  else
    return x - ((z * (half * y - v * r) - y) - v * S1);
}


} /* namespace std::impl */
_namespace_end(std)
