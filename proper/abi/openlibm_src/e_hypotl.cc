/* From: @(#)e_hypot.c 1.3 95/01/18 */
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

//__FBSDID("$FreeBSD: src/lib/msun/src/e_hypotl.c,v 1.3 2011/10/16 05:36:39 das Exp $");

/* long double version of hypot().  See e_hypot.c for most comments. */

#include <cmath>
#include <cfloat>
#include "private.h"

using namespace _namespace(std);

inline constexpr auto ESW(int16_t exp) noexcept -> int16_t {
  return LDBL_MAX_EXP - 1 + exp;
}

long double hypotl(long double x, long double y) noexcept {
  int16_t ha = get<0>(get_ldouble_words(x)) & 0x7fff;
  int16_t hb = get<0>(get_ldouble_words(y)) & 0x7fff;
  long double a, b;
  if (hb > ha) {
    a = y;
    b = x;
    swap(ha, hb);
  } else {
    a = x;
    b = y;
  }
  a = fabsl(a);
  b = fabsl(b);
  if (ha - hb > LDBL_MANT_DIG + 7)  /* x/y > 2**(LDBL_MANT_DIG+7) */
    return a + b;
  int16_t k = 0;
  if (ha > ESW(LDBL_MAX_EXP / 2 - 12)) {  /* a>2**(MAX_EXP/2-12) */
    if (ha >= ESW(LDBL_MAX_EXP)) {  /* Inf or NaN */
      /* Use original arg order iff result is NaN; quieten sNaNs. */
      long double w = fabsl(x + 0.0) - fabsl(y + 0.0);
      IEEE_l2bits uv = IEEE_l2bits_from_ldbl(a);
      mask_nbit_l(uv);
      if (uv.manh == 0 && uv.manl == 0) w = a;
      uv = IEEE_l2bits_from_ldbl(b);
      mask_nbit_l(uv);
      if (hb >= ESW(LDBL_MAX_EXP) && uv.manh == 0 && uv.manl == 0) w = b;
      return w;
    }
    /* scale a and b by 2**-(LDBL_MAX_EXP/2+88) */
    ha -= LDBL_MAX_EXP / 2 + 88;
    hb -= LDBL_MAX_EXP / 2 + 88;
    k += LDBL_MAX_EXP / 2 + 88;
    a = mod_ldbl_expsign(a, ha);
    b = mod_ldbl_expsign(b, hb);
  }
  if (hb < ESW(-(LDBL_MAX_EXP / 2 - 12))) {  /* b < 2**-(LDBL_MAX_EXP/2-12) */
    if (hb <= 0) {  /* subnormal b or 0 */
      uint32_t manh, manl;
      tie(ignore, manh, manl) = get_ldouble_words(b);
      if ((manh | manl) == 0) return a;
      const long double t1 = mod_ldbl_expsign(0.0l, ESW(LDBL_MAX_EXP - 2));  /* t1=2^(LDBL_MAX_EXP-2) */
      b *= t1;
      a *= t1;
      k -= LDBL_MAX_EXP - 2;
    } else {  /* scale a and b by 2^(LDBL_MAX_EXP/2+88) */
      ha += LDBL_MAX_EXP / 2 + 88;
      hb += LDBL_MAX_EXP / 2 + 88;
      k -= LDBL_MAX_EXP / 2 + 88;
      a = mod_ldbl_expsign(a, ha);
      b = mod_ldbl_expsign(b, hb);
    }
  }
  /* medium size a and b */
  long double w = a - b;
  if (w > b) {
    IEEE_l2bits uv = IEEE_l2bits_from_ldbl(a);
    uv.manl = 0;
    const long double t1 = ldbl_from_IEEE_l2bits(uv);
    const long double t2 = a - t1;
    w = sqrtl(t1 * t1 - (b * (-b) - t2 * (a + t1)));
  } else {
    a = a + a;
    IEEE_l2bits uv = IEEE_l2bits_from_ldbl(b);
    uv.manl = 0;
    const long double y1 = ldbl_from_IEEE_l2bits(uv);
    const long double y2 = b - y1;
    uv = IEEE_l2bits_from_ldbl(a);
    uv.manl = 0;
    const long double t1 = ldbl_from_IEEE_l2bits(uv);
    const long double t2 = a - t1;
    w = sqrtl(t1 * y1 - (w * (-w) -(t1 * y2 + t2 * b)));
  }
  if (k != 0) {
    const int16_t high = get<0>(get_ldouble_words(1.0l));
    const long double t1 = mod_ldbl_expsign(1.0l, high + k);
    return t1 * w;
  } else {
    return w;
  }
}
