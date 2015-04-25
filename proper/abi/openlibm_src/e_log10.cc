/* @(#)e_log10.c 1.3 95/01/18 */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunSoft, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 */

//__FBSDID("$FreeBSD: src/lib/msun/src/e_log10.c,v 1.15 2011/10/15 05:23:28 das Exp $");

/*
 * Return the base 10 logarithm of x.  See e_log.c and k_log.h for most
 * comments.
 *
 *    log10(x) = (f - 0.5*f*f + k_log1p(f)) / ln10 + k * log10(2)
 * in not-quite-routine extra precision.
 */

#include <cmath>
#include "private.h"

_namespace_begin(std)


double log10(double x) noexcept {
  using impl::k_log1p;

  static constexpr double two54      =  1.80143985094819840000e+16;  /* 0x43500000, 0x00000000 */
  static constexpr double ivln10hi   =  4.34294481878168880939e-01;  /* 0x3fdbcb7b, 0x15200000 */
  static constexpr double ivln10lo   =  2.50829467116452752298e-11;  /* 0x3dbb9438, 0xca9aadd5 */
  static constexpr double log10_2hi  =  3.01029995663611771306e-01;  /* 0x3FD34413, 0x509F6000 */
  static constexpr double log10_2lo  =  3.69423907715893078616e-13;  /* 0x3D59FEF3, 0x11F12B36 */
  static constexpr double zero   =  0.0;

#if 0
	double f,hfsq,hi,lo,r,val_hi,val_lo,w,y,y2;
	int32_t i,k,hx;
	u_int32_t lx;
#endif

  int32_t hx;
  uint32_t lx;
  tie(hx, lx) = extract_words(x);

  int32_t k = 0;
  if (hx < 0x00100000) {  /* x < 2**-1022  */
    if (((hx & 0x7fffffff) | lx) == 0)
      return -two54 / zero;  /* log(+-0)=-inf */
    if (hx < 0) return (x - x) / zero;  /* log(-#) = NaN */
    /* subnormal number, scale up x */
    k -= 54;
    x *= two54;
    hx = get<0>(extract_words(x));
  }
  if (hx >= 0x7ff00000) return x + x;
  if (hx == 0x3ff00000 && lx == 0) return zero;  /* log(1) = +0 */
  k += (hx >> 20) - 1023;
  hx &= 0x000fffff;
  const int32_t i = (hx + 0x95f64) & 0x100000;
  x = set_high_word(x, hx | (i ^ 0x3ff00000));  /* normalize x or x/2 */
  k += i >> 20;
  const double y = static_cast<double>(k);
  const double f = x - 1.0;
  const double hfsq = 0.5 * f * f;
  const double r = k_log1p(f);

  /* See e_log2.c for most details. */
  const double hi = set_low_word(f - hfsq, 0);
  const double lo = (f - hi) - hfsq + r;
  double val_hi = hi * ivln10hi;
  const double y2 = y * log10_2hi;
  double val_lo = y * log10_2lo + (lo + hi) * ivln10lo + lo * ivln10hi;

  /*
   * Extra precision in for adding y*log10_2hi is not strictly needed
   * since there is no very large cancellation near x = sqrt(2) or
   * x = 1/sqrt(2), but we do it anyway since it costs little on CPUs
   * with some parallelism and it reduces the error for many args.
   */
  const double w = y2 + val_hi;
  val_lo += (y2 - w) + val_hi;
  val_hi = w;

  return val_lo + val_hi;
}


_namespace_end(std)
