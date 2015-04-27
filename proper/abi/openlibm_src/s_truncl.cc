/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 *
 * From: @(#)s_floor.c 5.1 93/09/24
 */

//__FBSDID("$FreeBSD: src/lib/msun/src/s_truncl.c,v 1.9 2008/02/14 15:10:34 bde Exp $");

/*
 * truncl(x)
 * Return x rounded toward 0 to integral value
 * Method:
 *	Bit twiddling.
 * Exception:
 *	Inexact flag raised if x not equal to truncl(x).
 */

#include <cmath>
#include <cfloat>
#include <array>
#include "private.h"

using namespace _namespace(std);


long double truncl(long double x) noexcept {
  static constexpr int MANH_SIZE = LDBL_MANH_SIZE;
  static constexpr long double huge = 1.0e300;
  static constexpr array<float, 2> zero{{ 0.0, -0.0 }};

  IEEE_l2bits u = IEEE_l2bits_from_ldbl(x);
  const int e = u.exp - LDBL_MAX_EXP + 1;

  if (e < MANH_SIZE - 1) {
    if (e < 0) {  /* raise inexact if x != 0 */
      if (huge + x > 0.0)
        u = IEEE_l2bits_from_ldbl(zero[u.sign]);
    } else {
      uint64_t m = ((1llu << MANH_SIZE) - 1) >> (e + 1);
      if (((u.manh & m) | u.manl) == 0) return x;  /* x is integral */
      if (huge + x > 0.0) {  /* raise inexact flag */
        u.manh &= ~m;
        u.manl = 0;
      }
    }
  } else if (e < LDBL_MANT_DIG - 1) {
    uint64_t m = static_cast<uint64_t>(-1) >> (64 - LDBL_MANT_DIG + e + 1);
    if ((u.manl & m) == 0) return x;  /* x is integral */
    if (huge + x > 0.0)  /* raise inexact flag */
      u.manl &= ~m;
  }
  return ldbl_from_IEEE_l2bits(u);
}
