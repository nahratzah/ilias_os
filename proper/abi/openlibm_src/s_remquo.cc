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

//__FBSDID("$FreeBSD: src/lib/msun/src/s_remquo.c,v 1.2 2008/03/30 20:47:26 das Exp $");

#include <cmath>
#include <array>
#include "private.h"

_namespace_begin(std)


/*
 * Return the IEEE remainder and set *quo to the last n bits of the
 * quotient, rounded to the nearest integer.  We choose n=31 because
 * we wind up computing all the integer bits of the quotient anyway as
 * a side-effect of computing the remainder by the shift and subtract
 * method.  In practice, this is far more bits than are needed to use
 * remquo in reduction algorithms.
 */
double remquo(double x, double y, int *quo) noexcept {
  static constexpr array<double, 2> Zero{{0.0, -0.0,}};

#if 0
	int32_t n,hx,hy,hz,ix,iy,sx,i;
	u_int32_t lx,ly,lz,q,sxy;
#endif

  int32_t hx, hy;
  uint32_t lx, ly;
  tie(hx, lx) = extract_words(x);
  tie(hy, ly) = extract_words(y);
  const uint32_t sxy = (hx ^ hy) & 0x80000000;
  const int32_t sx = hx & 0x80000000;  /* sign of x */
  hx ^= sx;  /* |x| */
  hy &= 0x7fffffff;  /* |y| */

  int32_t hz;
  uint32_t lz;
  int32_t n;

  /* purge off exception values */
  uint32_t q;
  if ((hy | ly) == 0 || hx >= 0x7ff00000 ||  /* y=0,or x not finite */
      (hy | ((ly | -ly) >> 31)) > 0x7ff00000)  /* or y is NaN */
    return (x * y) / (x * y);
  if (hx <= hy) {
    if ((hx < hy) || (lx < ly)) {
      q = 0;
      goto fixup;  /* |x|<|y| return x or x-y */
    }
    if (lx == ly) {
      *quo = 1;
      return Zero[static_cast<uint32_t>(sx) >> 31];  /* |x|=|y| return x*0*/
    }
  }

  /* determine ix = ilogb(x) */
  int32_t ix;
  if (hx < 0x00100000) {  /* subnormal x */
    if (hx == 0) {
      int32_t i;
      for (ix = -1043, i = lx; i > 0; i <<= 1)
        ix -= 1;
    } else {
      int32_t i;
      for (ix = -1022, i = (hx << 11); i > 0; i <<= 1)
        ix -=1;
    }
  } else {
    ix = (hx >> 20) - 1023;
  }

  /* determine iy = ilogb(y) */
  int32_t iy;
  if (hy < 0x00100000) {  /* subnormal y */
    if (hy == 0) {
      int32_t i;
      for (iy = -1043, i = ly; i > 0; i <<= 1)
        iy -=1;
    } else {
      int32_t i;
      for (iy = -1022, i = (hy << 11); i > 0; i <<= 1)
        iy -=1;
    }
  } else {
    iy = (hy >> 20) - 1023;
  }

  /* set up {hx,lx}, {hy,ly} and align y to x */
  if (ix >= -1022) {
    hx = 0x00100000 | (0x000fffff & hx);
  } else {  /* subnormal x, shift x to normal */
    n = -1022 - ix;
    if (n <= 31) {
      hx = (hx << n) | (lx >> (32 - n));
      lx <<= n;
    } else {
      hx = lx << (n - 32);
      lx = 0;
    }
  }
  if (iy >= -1022) {
    hy = 0x00100000 | (0x000fffff & hy);
  } else {  /* subnormal y, shift y to normal */
    n = -1022-iy;
    if (n <= 31) {
      hy = (hy << n) | (ly >> (32 - n));
      ly <<= n;
    } else {
      hy = ly << (n - 32);
      ly = 0;
    }
  }

  /* fix point fmod */
  n = ix - iy;
  q = 0;
  while (n--) {
    int32_t hz = hx - hy;
    const uint32_t lz = lx - ly;
    if (lx < ly) hz -= 1;
    if (hz < 0) {
      hx = hx + hx + (lx >> 31);
      lx = lx + lx;
    } else {
      hx = hz + hz + (lz >> 31);
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
    return Zero[static_cast<uint32_t>(sx) >> 31];
  }
  while (hx < 0x00100000) {  /* normalize x */
    hx = hx + hx + (lx >> 31);
    lx = lx + lx;
    iy -= 1;
  }
  if (iy >= -1022) {  /* normalize output */
    hx = (hx - 0x00100000) | ((iy + 1023) << 20);
  } else {  /* subnormal output */
    n = -1022 - iy;
    if (n <= 20) {
      lx = (lx >> n) | (static_cast<uint32_t>(hx) << (32 - n));
      hx >>= n;
    } else if (n <= 31) {
      lx = (hx << (32 - n)) | (lx >> n);
      hx = sx;
    } else {
      lx = hx >> (n - 32);
      hx = sx;
    }
  }
fixup:
  x = set_double_from_words(hx, lx);
  y = fabs(y);
  if (y < 0x1p-1021) {
    if (x + x > y || (x + x == y && (q & 1))) {
      q++;
      x -= y;
    }
  } else if (x > 0.5 * y || (x == 0.5 * y && (q & 1))) {
    q++;
    x -= y;
  }
  hx = get<0>(extract_words(x));
  x = set_high_word(x, hx ^ sx);
  q &= 0x7fffffff;
  *quo = (sxy ? -q : q);
  return x;
}


_namespace_end(std)