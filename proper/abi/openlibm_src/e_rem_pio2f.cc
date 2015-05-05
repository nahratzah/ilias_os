/* e_rem_pio2f.c -- float version of e_rem_pio2.c
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

//__FBSDID("$FreeBSD: src/lib/msun/src/e_rem_pio2f.c,v 1.32 2009/06/03 08:16:34 ed Exp $");

/* __ieee754_rem_pio2f(x,y)
 *
 * return the remainder of x rem pi/2 in *y
 * use double precision for everything except passing x
 * use __kernel_rem_pio2() for large x
 */

#include <cmath>
#include <array>
#include "private.h"

_namespace_begin(std)
namespace impl {


int __ieee754_rem_pio2f(float x, double *y) noexcept {
  /*
   * invpio2:  53 bits of 2/pi
   * pio2_1:   first  33 bit of pi/2
   * pio2_1t:  pi/2 - pio2_1
   */
  static constexpr double invpio2 =  6.36619772367581382433e-01;  /* 0x3FE45F30, 0x6DC9C883 */
  static constexpr double pio2_1  =  1.57079631090164184570e+00;  /* 0x3FF921FB, 0x50000000 */
  static constexpr double pio2_1t =  1.58932547735281966916e-08;  /* 0x3E5110b4, 0x611A6263 */

#if 0
	double w,r,fn;
	double tx[1],ty[1];
	float z;
	int32_t e0,n,ix,hx;
#endif

  const int32_t hx = get_float_as_int(x);
  const int32_t ix = hx & 0x7fffffff;
  /* 33+53 bit pi is good enough for medium size */
  if (ix < 0x4dc90fdb) {  /* |x| ~< 2^28*(pi/2), medium size */
    /* Use a specialized rint() to get fn.  Assume round-to-nearest. */
    const double fn = static_cast<double>(x * invpio2 + 0x1.8p52) - 0x1.8p52;
#ifdef HAVE_EFFICIENT_IRINT
    const int32_t n = irint(fn);
#else
    const int32_t n = static_cast<int32_t>(fn);
#endif
    const double r = x - fn * pio2_1;
    const double w = fn * pio2_1t;
    *y = r - w;
    return n;
  }
  /*
   * all other (large) arguments
   */
  if (ix >= 0x7f800000) {  /* x is inf or NaN */
    *y = x - x;
    return 0;
  }
  /* set z = scalbn(|x|,ilogb(|x|)-23) */
  const int32_t e0 = (ix >> 23) - 150;  /* e0 = ilogb(|x|)-23; */
  const float z = set_float_from_int(ix - static_cast<int32_t>(e0 << 23));
  array<double, 1> tx{{ z }};
  array<double, 1> ty;
  const int32_t n = __kernel_rem_pio2(tx.data(), ty.data(), e0, 1, 0);
  if (hx < 0) {
    *y = -ty[0];
    return -n;
  }
  *y = ty[0];
  return n;
}


} /* namespace std::impl */
_namespace_end(std)