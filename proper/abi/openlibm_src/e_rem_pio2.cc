
/* @(#)e_rem_pio2.c 1.4 95/01/18 */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunSoft, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 *
 * Optimized by Bruce D. Evans.
 */

//__FBSDID("$FreeBSD: src/lib/msun/src/e_rem_pio2.c,v 1.22 2011/06/19 17:07:58 kargl Exp $");

/* __ieee754_rem_pio2(x,y)
 * 
 * return the remainder of x rem pi/2 in y[0]+y[1] 
 * use __kernel_rem_pio2()
 */

#include <cmath>
#include <array>
#include "private.h"

/*
 * invpio2:  53 bits of 2/pi
 * pio2_1:   first  33 bit of pi/2
 * pio2_1t:  pi/2 - pio2_1
 * pio2_2:   second 33 bit of pi/2
 * pio2_2t:  pi/2 - (pio2_1+pio2_2)
 * pio2_3:   third  33 bit of pi/2
 * pio2_3t:  pi/2 - (pio2_1+pio2_2+pio2_3)
 */

_namespace_begin(std)
namespace impl {


int __ieee754_rem_pio2(double x, double* y) noexcept {
  static constexpr double zero = 0.00000000000000000000e+00; /* 0x00000000, 0x00000000 */
  static constexpr double two24 = 1.67772160000000000000e+07; /* 0x41700000, 0x00000000 */
  static constexpr double invpio2 = 6.36619772367581382433e-01; /* 0x3FE45F30, 0x6DC9C883 */
  static constexpr double pio2_1  = 1.57079632673412561417e+00; /* 0x3FF921FB, 0x54400000 */
  static constexpr double pio2_1t = 6.07710050650619224932e-11; /* 0x3DD0B461, 0x1A626331 */
  static constexpr double pio2_2  = 6.07710050630396597660e-11; /* 0x3DD0B461, 0x1A600000 */
  static constexpr double pio2_2t = 2.02226624879595063154e-21; /* 0x3BA3198A, 0x2E037073 */
  static constexpr double pio2_3  = 2.02226624871116645580e-21; /* 0x3BA3198A, 0x2E000000 */
  static constexpr double pio2_3t = 8.47842766036889956997e-32; /* 0x397B839A, 0x252049C1 */

  const int32_t hx = get<0>(extract_words(x));
  const int32_t ix = hx&0x7fffffff;
#if 0 /* Must be handled in caller. */
  if (ix <= 0x3fe921fb) {  /* |x| ~<= pi/4 , no need for reduction */
    y[0] = x;
    y[1] = 0;
    return 0;
  }
#endif
  if (ix <= 0x400f6a7a) {  /* |x| ~<= 5pi/4 */
    if ((ix & 0xfffff) == 0x921fb)  /* |x| ~= pi/2 or 2pi/2 */
      goto medium;  /* cancellation -- use medium case */
    if (ix <= 0x4002d97c) {  /* |x| ~<= 3pi/4 */
      if (hx > 0) {
        const double z = x - pio2_1;  /* one round good to 85 bits */
	y[0] = z - pio2_1t;
	y[1] = (z - y[0]) - pio2_1t;
	return 1;
      } else {
        const double z = x + pio2_1;
	y[0] = z + pio2_1t;
	y[1] = (z - y[0]) + pio2_1t;
	return -1;
      }
    } else {
      if (hx > 0) {
        const double z = x - 2 * pio2_1;
	y[0] = z - 2 * pio2_1t;
	y[1] = (z - y[0]) - 2 * pio2_1t;
	return 2;
      } else {
        const double z = x + 2 * pio2_1;
	y[0] = z + 2 * pio2_1t;
	y[1] = (z - y[0]) + 2 * pio2_1t;
	return -2;
      }
    }
  }
  if (ix <= 0x401c463b) {  /* |x| ~<= 9pi/4 */
    if (ix <= 0x4015fdbc) {  /* |x| ~<= 7pi/4 */
      if (ix == 0x4012d97c)  /* |x| ~= 3pi/2 */
        goto medium;
      if (hx > 0) {
        const double z = x - 3 * pio2_1;
	y[0] = z - 3 * pio2_1t;
	y[1] = (z - y[0]) - 3 * pio2_1t;
	return 3;
      } else {
        const double z = x + 3 * pio2_1;
	y[0] = z + 3 * pio2_1t;
	y[1] = (z - y[0]) + 3 * pio2_1t;
	return -3;
      }
    } else {
      if (ix == 0x401921fb)  /* |x| ~= 4pi/2 */
        goto medium;
      if (hx > 0) {
        const double z = x - 4 * pio2_1;
	y[0] = z - 4 * pio2_1t;
	y[1] = (z - y[0]) - 4 * pio2_1t;
	return 4;
      } else {
        const double z = x + 4 * pio2_1;
	y[0] = z + 4 * pio2_1t;
	y[1] = (z - y[0]) + 4 * pio2_1t;
	return -4;
      }
    }
  }
  if (ix < 0x413921fb) {  /* |x| ~< 2^20*(pi/2), medium size */
medium:
    /* Use a specialized rint() to get fn.  Assume round-to-nearest. */
    const double fn = static_cast<double>(x * invpio2 + 0x1.8p52) - 0x1.8p52;
#ifdef HAVE_EFFICIENT_IRINT
    const int32_t n = irint(fn);
#else
    const int32_t n = (int32_t)fn;
#endif
    double r = x - fn * pio2_1;
    double w = fn * pio2_1t;  /* 1st round good to 85 bit */
    {
      const int32_t j = ix >> 20;
      y[0] = r - w;
      uint32_t high = get<0>(extract_words(y[0]));
      int32_t i = j - ((high >> 20) & 0x7ff);
      if (i > 16) {  /* 2nd iteration needed, good to 118 */
        double t = r;
	w = fn * pio2_2;
	r = t - w;
	w = fn * pio2_2t - (t - r - w);
	y[0] = r - w;
	high = get<0>(extract_words(y[0]));
	i = j - ((high >> 20) & 0x7ff);
	if (i > 49) {  /* 3rd iteration need, 151 bits acc */
          t = r;  /* will cover all possible cases */
	  w = fn * pio2_3;
	  r = t - w;
	  w = fn * pio2_3t - (t - r - w);
	  y[0] = r - w;
	}
      }
    }
    y[1] = r - y[0] - w;
    return n;
  }
  /*
   * all other (large) arguments
   */
  if (ix >= 0x7ff00000) {  /* x is inf or NaN */
    y[0] = y[1] = x - x;
    return 0;
  }
  /* set z = scalbn(|x|,ilogb(x)-23) */
  const uint32_t low = get<1>(extract_words(x));
  const int32_t e0 = (ix >> 20) - 1046;  /* e0 = ilogb(z)-23; */
  double z = set_double_from_words(ix - static_cast<int32_t>(e0 << 20), low);
  array<double, 3> tx;
  for (int32_t i = 0; i < 2; i++) {
    tx[i] = static_cast<double>(static_cast<int32_t>(z));
    z = (z - tx[i]) * two24;
  }
  tx[2] = z;
  int32_t nx = 3;
  while (tx[nx - 1] == zero) nx--;  /* skip zero term */
  array<double, 2> ty;
  const int32_t n = __kernel_rem_pio2(tx.data(), ty.data(), e0, nx, 1);
  if (hx < 0) {
    y[0] = -ty[0];
    y[1] = -ty[1];
    return -n;
  }
  y[0] = ty[0];
  y[1] = ty[1];
  return n;
}


} /* namespace impl */
_namespace_end(std)
