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

//__FBSDID("$FreeBSD: src/lib/msun/src/s_remquol.c,v 1.2 2008/07/31 20:09:47 das Exp $");

#include <cmath>
#include <cfloat>
#include <array>
#include "private.h"

using namespace _namespace(std);

/*
 * Return the IEEE remainder and set *quo to the last n bits of the
 * quotient, rounded to the nearest integer.  We choose n=31 because
 * we wind up computing all the integer bits of the quotient anyway as
 * a side-effect of computing the remainder by the shift and subtract
 * method.  In practice, this is far more bits than are needed to use
 * remquo in reduction algorithms.
 *
 * Assumptions:
 * - The low part of the mantissa fits in a manl_t exactly.
 * - The high part of the mantissa fits in an int64_t with enough room
 *   for an explicit integer bit in front of the fractional bits.
 */
long double remquol(long double x, long double y, int *quo) noexcept {
  static constexpr array<long double, 2> Zero{{0.0L, -0.0L}};
  static constexpr auto BIAS = LDBL_MAX_EXP - 1;
  static constexpr auto HFRAC_BITS = LDBL_MANH_SIZE - 1;
  static constexpr auto MANL_SHIFT = LDBL_MANH_SIZE - 1;

  IEEE_l2bits ux = IEEE_l2bits_from_ldbl(x);
  IEEE_l2bits uy = IEEE_l2bits_from_ldbl(y);
  const int sx = ux.sign;
  const int sxy = sx ^ uy.sign;
  ux.sign = 0;  /* |x| */
  uy.sign = 0;  /* |y| */
  x = ldbl_from_IEEE_l2bits(ux);

  int64_t hz;
  uint32_t lz;
  int n;
  int64_t hx;
  uint32_t hy;
  uint32_t lx;
  uint32_t ly;

  /* purge off exception values */
  int q;
  if ((uy.exp | uy.manh | uy.manl) == 0 ||  /* y=0 */
      ux.exp == BIAS + LDBL_MAX_EXP ||  /* or x not finite */
      (uy.exp == BIAS + LDBL_MAX_EXP &&
       (masked_nbit_l(uy.manh) | uy.manl) != 0)) /* or y is NaN */
    return (x * y) / (x * y);
  if (ux.exp <= uy.exp) {
    if ((ux.exp < uy.exp) ||
        (ux.manh <= uy.manh && (ux.manh < uy.manh ||
				ux.manl < uy.manl))) {
      q = 0;
      goto fixup;  /* |x|<|y| return x or x-y */
    }
    if (ux.manh == uy.manh && ux.manl == uy.manl) {
      *quo = 1;
      return Zero[sx];  /* |x|=|y| return x*0*/
    }
  }

  /* determine ix = ilogb(x) */
  int ix;
  if (ux.exp == 0) {  /* subnormal x */
    ux = IEEE_l2bits_from_ldbl(ldbl_from_IEEE_l2bits(ux) * 0x1.0p512);
    ix = ux.exp - (BIAS + 512);
  } else {
    ix = ux.exp - BIAS;
  }

  /* determine iy = ilogb(y) */
  int iy;
  if (uy.exp == 0) {  /* subnormal y */
    uy = IEEE_l2bits_from_ldbl(ldbl_from_IEEE_l2bits(uy) * 0x1.0p512);
    iy = uy.exp - (BIAS + 512);
  } else {
    iy = uy.exp - BIAS;
  }

  /* set up {hx,lx}, {hy,ly} and align y to x */
  hx = ux.manh;
  hy = uy.manh;
  lx = ux.manl;
  ly = uy.manl;

  /* fix point fmod */
  n = ix - iy;
  q = 0;

  while (n--) {
    int64_t hz = hx - hy;
    const uint32_t lz = lx - ly;
    if (lx < ly) hz -= 1;
    if (hz < 0) {
      hx = hx + hx + (lx >> MANL_SHIFT);
      lx = lx + lx;
    } else {
      hx = hz + hz + (lz >> MANL_SHIFT);
      lx = lz + lz;
      q++;
    }
    q <<= 1;
  }
  hz = hx - hy;
  lz = lx - ly;
  if (lx < ly) hz -= 1;
  if (hz >= 0) {
    hx = hz;
    lx = lz;
    q++;
  }

  /* convert back to floating value and restore the sign */
  if ((hx | lx) == 0) {  /* return sign(x)*0 */
    *quo = (sxy ? -q : q);
    return Zero[sx];
  }
  while (hx < static_cast<int64_t>(1ULL << HFRAC_BITS)) {  /* normalize x */
    hx = hx + hx + (lx >> MANL_SHIFT);
    lx = lx + lx;
    iy -= 1;
  }
  ux.manh = hx;  /* The integer bit is truncated here if needed. */
  ux.manl = lx;
  if (iy < LDBL_MIN_EXP) {
    ux.exp = iy + (BIAS + 512);
    ux = IEEE_l2bits_from_ldbl(ldbl_from_IEEE_l2bits(ux) * 0x1p-512);
  } else {
    ux.exp = iy + BIAS;
  }
  ux.sign = 0;
  x = ldbl_from_IEEE_l2bits(ux);
fixup:
  y = fabsl(y);
  if (y < LDBL_MIN * 2) {
    if (x + x > y || (x + x == y && (q & 1))) {
      q++;
      x -= y;
    }
  } else if (x > 0.5 * y || (x == 0.5 * y && (q & 1))) {
    q++;
    x -= y;
  }

  ux = IEEE_l2bits_from_ldbl(x);
  ux.sign ^= sx;
  x = ldbl_from_IEEE_l2bits(ux);

  q &= 0x7fffffff;
  *quo = (sxy ? -q : q);
  return x;
}
