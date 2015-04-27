/*
 * From: @(#)s_ilogb.c 5.1 93/09/24
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 */

#include <cmath>
#include <cfloat>
#include <climits>
#include "private.h"

using namespace _namespace(std);

long double logbl(long double x) noexcept {
  IEEE_l2bits u = IEEE_l2bits_from_ldbl(x);
  if (u.exp == 0) {
    if ((u.manl | u.manh) == 0) {  /* x == 0 */
      u.sign = 1;
      return 1.0L / ldbl_from_IEEE_l2bits(u);
    }
    /* denormalized */
    int b;
    if (u.manh == 0) {
      unsigned long m = 1lu << (LDBL_MANL_SIZE - 1);
      for (b = LDBL_MANH_SIZE; !(u.manl & m); m >>= 1)
        b++;
    } else {
      unsigned long m = 1lu << (LDBL_MANH_SIZE - 1);
      for (b = 0; !(u.manh & m); m >>= 1)
        b++;
    }
#ifdef LDBL_IMPLICIT_NBIT
    b++;
#endif
    return static_cast<long double>(LDBL_MIN_EXP - b - 1);
  }
  if (u.exp < (LDBL_MAX_EXP << 1) - 1)	/* normal */
    return static_cast<long double>(u.exp - LDBL_MAX_EXP + 1);
  else  /* +/- inf or nan */
    return x * x;
}
