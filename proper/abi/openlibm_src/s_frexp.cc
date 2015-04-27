/* @(#)s_frexp.c 5.1 93/09/24 */
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

//__FBSDID("$FreeBSD: src/lib/msun/src/s_frexp.c,v 1.11 2008/02/22 02:30:35 das Exp $");

/*
 * for non-zero x
 *	x = frexp(arg,&exp);
 * return a double fp quantity x such that 0.5 <= |x| <1.0
 * and the corresponding binary exponent "exp". That is
 *	arg = x*2^exp.
 * If arg is inf, 0.0, or NaN, then frexp(arg,&exp) returns arg
 * with *exp=0.
 */

#include <cmath>
#include "private.h"

_namespace_begin(std)


double frexp(double x, int *eptr) noexcept {
  static constexpr double two54 =  1.80143985094819840000e+16; /* 0x43500000, 0x00000000 */

  int32_t hx, lx;
  tie(hx, lx) = extract_words(x);
  int32_t ix = hx & 0x7fffffff;

  *eptr = 0;
  if (ix >= 0x7ff00000 || ((ix | lx) == 0)) return x;  /* 0,inf,nan */
  if (ix < 0x00100000) {  /* subnormal */
    x *= two54;
    hx = get<0>(extract_words(x));
    ix = hx & 0x7fffffff;
    *eptr = -54;
  }
  *eptr += (ix >> 20) - 1022;
  hx = (hx & 0x800fffff) | 0x3fe00000;
  x = set_high_word(x, hx);
  return x;
}


_namespace_end(std)
