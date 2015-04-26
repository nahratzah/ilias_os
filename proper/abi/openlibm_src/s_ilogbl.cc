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

//__FBSDID("$FreeBSD: src/lib/msun/src/s_ilogbl.c,v 1.2 2008/02/22 02:30:35 das Exp $");

#include <cmath>
#include <cfloat>
#include <climits>
#include "private.h"

using namespace _namespace(std);

int ilogbl(long double x) noexcept {
  IEEE_l2bits u = IEEE_l2bits_from_ldbl(x);
  if (u.exp == 0) {
    int b;
    if ((u.manl | u.manh) == 0) return (FP_ILOGB0);
    /* denormalized */
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
    return LDBL_MIN_EXP - b - 1;
  } else if (u.exp < (LDBL_MAX_EXP << 1) - 1) {
    return (u.exp - LDBL_MAX_EXP + 1);
  } else if (u.manl != 0 || u.manh != 0) {
    return FP_ILOGBNAN;
  } else {
    return INT_MAX;
  }
}
