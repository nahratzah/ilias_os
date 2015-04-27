/* @(#)e_remainder.c 1.3 95/01/18 */
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

//__FBSDID("$FreeBSD: src/lib/msun/src/e_remainder.c,v 1.12 2008/03/30 20:47:42 das Exp $");

/* __ieee754_remainder(x,p)
 * Return :                  
 * 	returns  x REM p  =  x - [x/p]*p as if in infinite 
 * 	precise arithmetic, where [x/p] is the (infinite bit) 
 *	integer nearest x/p (in half way case choose the even one).
 * Method : 
 *	Based on fmod() return x-[x/p]chopped*p exactlp.
 */

#include <cmath>
#include "private.h"

_namespace_begin(std)


double remainder(double x, double p) noexcept {
  static constexpr double zero = 0.0;

  int32_t hx, hp;
  uint32_t lx, lp;
  tie(hx, lx) = extract_words(x);
  tie(hp, lp) = extract_words(p);
  const uint32_t sx = hx & 0x80000000;
  hp &= 0x7fffffff;
  hx &= 0x7fffffff;

  /* purge off exception values */
  if ((hp | lp) == 0) return (x * p) / (x * p);  /* p = 0 */
  if (hx >= 0x7ff00000 ||  /* x not finite */
      (hp >= 0x7ff00000 &&  /* p is NaN */
       ((hp - 0x7ff00000) | lp) != 0))
    return (static_cast<long double>(x) * p) /
           (static_cast<long double>(x) * p);

  if (hp <= 0x7fdfffff) x = fmod(x, p + p);  /* now x < 2p */
  if (((hx - hp) | (lx - lp)) == 0) return zero * x;
  x = fabs(x);
  p = fabs(p);
  if (hp < 0x00200000) {
    if (x + x > p) {
      x -= p;
      if (x + x >= p) x -= p;
    }
  } else {
    const double p_half = 0.5 * p;
    if (x > p_half) {
      x -= p;
      if (x >= p_half) x -= p;
    }
  }
  hx = get<0>(extract_words(x));
  if ((hx & 0x7fffffff) == 0) hx = 0;
  x = set_high_word(x, hx ^ sx);
  return x;
}


_namespace_end(std)
