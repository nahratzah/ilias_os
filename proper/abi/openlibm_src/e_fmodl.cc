/* @(#)e_fmod.c 1.3 95/01/18 */
/*-
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunSoft, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 */

//__FBSDID("$FreeBSD: src/lib/msun/src/e_fmodl.c,v 1.2 2008/07/31 20:09:47 das Exp $");

#include <cmath>
#include <cfloat>
#include <array>
#include "private.h"

using namespace _namespace(std);

/*
 * fmodl(x,y)
 * Return x mod y in exact arithmetic
 * Method: shift and subtract
 *
 * Assumptions:
 * - The low part of the mantissa fits in a manl_t exactly.
 * - The high part of the mantissa fits in an int64_t with enough room
 *   for an explicit integer bit in front of the fractional bits.
 */
long double fmodl(long double x, long double y) noexcept {
  static constexpr long double one = 1.0;
  static constexpr array<long double, 2> Zero{{ 0.0, -0.0 }};
  static constexpr auto BIAS = LDBL_MAX_EXP - 1;
  static constexpr auto HFRAC_BITS = LDBL_MANH_SIZE - 1;
  static constexpr auto MANL_SHIFT = LDBL_MANL_SIZE - 1;

  IEEE_l2bits ux = IEEE_l2bits_from_ldbl(x);
  IEEE_l2bits uy = IEEE_l2bits_from_ldbl(y);
  const int sx = ux.sign;

  /* purge off exception values */
  if ((uy.exp | uy.manh | uy.manl) == 0 ||  /* y=0 */
      (ux.exp == BIAS + LDBL_MAX_EXP) ||  /* or x not finite */
      (uy.exp == BIAS + LDBL_MAX_EXP &&
       (masked_nbit_l(uy.manh) | uy.manl) != 0))  /* or y is NaN */
    return (x * y) / (x * y);
  if (ux.exp <= uy.exp) {
    if ((ux.exp < uy.exp) || (ux.manh <= uy.manh && (ux.manh < uy.manh ||
                                                     ux.manl < uy.manl))) {
      return x;  /* |x|<|y| return x or x-y */
    }
    if (ux.manh == uy.manh && ux.manl == uy.manl) {
      return Zero[sx];  /* |x|=|y| return x*0*/
    }
  }

  /* determine ix = ilogb(x) */
  int ix;
  if (ux.exp == 0) {  /* subnormal x */
    ux = IEEE_l2bits_from_ldbl(x * 0x1.0p512);
    ix = ux.exp - (BIAS + 512);
  } else {
    ix = ux.exp - BIAS;
  }

  /* determine iy = ilogb(y) */
  int iy;
  if (uy.exp == 0) {  /* subnormal y */
    uy = IEEE_l2bits_from_ldbl(y * 0x1.0p512);
    iy = uy.exp - (BIAS + 512);
  } else {
    iy = uy.exp - BIAS;
  }

  /* set up {hx,lx}, {hy,ly} and align y to x */
  int64_t hx = ux.manh;
  uint32_t hy = uy.manh;
  uint32_t lx = ux.manl;
  uint32_t ly = uy.manl;

  /* fix point fmod */
  int n = ix - iy;

  while (n--) {
    int64_t hz = hx - hy;
    uint32_t lz = lx - ly;
    if (lx < ly) hz -= 1;
    if (hz < 0) {
      hx = hx + hx + (lx >> MANL_SHIFT); lx = lx + lx;
    } else {
      if ((hz | lz) == 0)  /* return sign(x)*0 */
        return Zero[sx];
      hx = hz + hz + (lz >> MANL_SHIFT); lx = lz + lz;
    }
  }
  int64_t hz = hx - hy;
  const uint32_t lz = lx - ly;
  if (lx < ly) hz -= 1;
  if (hz >= 0) {
    hx = hz;
    lx = lz;
  }

  /* convert back to floating value and restore the sign */
  if ((hx | lx) == 0)  /* return sign(x)*0 */
    return Zero[sx];
  while (hx < static_cast<int64_t>(1ULL << HFRAC_BITS)) {  /* normalize x */
    hx = hx + hx + (lx >> MANL_SHIFT);
    lx = lx + lx;
    iy -= 1;
  }
  ux.manh = hx; /* The mantissa is truncated here if needed. */
  ux.manl = lx;
  if (iy < LDBL_MIN_EXP) {
    ux.exp = iy + (BIAS + 512);
    ux = IEEE_l2bits_from_ldbl(ldbl_from_IEEE_l2bits(ux) * 0x1p-512);
  } else {
    ux.exp = iy + BIAS;
  }
  x = ldbl_from_IEEE_l2bits(ux) * one;  /* create necessary signal */
  return x;  /* exact output */
}
