
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

//__FBSDID("$FreeBSD: src/lib/msun/src/e_log2.c,v 1.4 2011/10/15 05:23:28 das Exp $");

/*
 * Return the base 2 logarithm of x.  See e_log.c and k_log.h for most
 * comments.
 *
 * This reduces x to {k, 1+f} exactly as in e_log.c, then calls the kernel,
 * then does the combining and scaling steps
 *    log2(x) = (f - 0.5*f*f + k_log1p(f)) / ln2 + k
 * in not-quite-routine extra precision.
 */

#include <cmath>
#include "private.h"

_namespace_begin(std)


double log2(double x) noexcept {
  using impl::k_log1p;

  static constexpr double two54    =  1.80143985094819840000e+16;  /* 0x43500000, 0x00000000 */
  static constexpr double ivln2hi  =  1.44269504072144627571e+00;  /* 0x3ff71547, 0x65200000 */
  static constexpr double ivln2lo  =  1.67517131648865118353e-10;  /* 0x3de705fc, 0x2eefa200 */
  static constexpr double zero     =  0.0;

  int32_t hx;
  uint32_t lx;
  tie(hx, lx) = extract_words(x);

  int32_t k = 0;
  if (hx < 0x00100000) {  /* x < 2**-1022  */
    if (((hx & 0x7fffffff) | lx) == 0) return -two54 / zero;  /* log(+-0)=-inf */
    if (hx < 0) return (x - x) / zero;  /* log(-#) = NaN */
    k -= 54;
    x *= two54;  /* subnormal number, scale up x */
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

  /*
   * f-hfsq must (for args near 1) be evaluated in extra precision
   * to avoid a large cancellation when x is near sqrt(2) or 1/sqrt(2).
   * This is fairly efficient since f-hfsq only depends on f, so can
   * be evaluated in parallel with R.  Not combining hfsq with R also
   * keeps R small (though not as small as a true `lo' term would be),
   * so that extra precision is not needed for terms involving R.
   *
   * Compiler bugs involving extra precision used to break Dekker's
   * theorem for spitting f-hfsq as hi+lo, unless double_t was used
   * or the multi-precision calculations were avoided when double_t
   * has extra precision.  These problems are now automatically
   * avoided as a side effect of the optimization of combining the
   * Dekker splitting step with the clear-low-bits step.
   *
   * y must (for args near sqrt(2) and 1/sqrt(2)) be added in extra
   * precision to avoid a very large cancellation when x is very near
   * these values.  Unlike the above cancellations, this problem is
   * specific to base 2.  It is strange that adding +-1 is so much
   * harder than adding +-ln2 or +-log10_2.
   *
   * This uses Dekker's theorem to normalize y+val_hi, so the
   * compiler bugs are back in some configurations, sigh.  And I
   * don't want to used double_t to avoid them, since that gives a
   * pessimization and the support for avoiding the pessimization
   * is not yet available.
   *
   * The multi-precision calculations for the multiplications are
   * routine.
   */
  const double hi = set_low_word(f - hfsq, 0);
  const double lo = (f - hi) - hfsq + r;
  double val_hi = hi * ivln2hi;
  double val_lo = (lo + hi) * ivln2lo + lo * ivln2hi;

  /* spadd(val_hi, val_lo, y), except for not using double_t: */
  const double w = y + val_hi;
  val_lo += (y - w) + val_hi;
  val_hi = w;

  return val_lo + val_hi;
}


_namespace_end(std)
