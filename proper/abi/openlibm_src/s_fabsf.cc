/* s_fabsf.c -- float version of s_fabs.c.
 * Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com.
 */

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

//__FBSDID("$FreeBSD: src/lib/msun/src/s_fabsf.c,v 1.8 2008/02/22 02:30:35 das Exp $");

/*
 * fabsf(x) returns the absolute value of x.
 */

#include <cmath>
#include "private.h"

using namespace _namespace(std);

float fabsf(float x) noexcept {
  return mask_float(x, 0x7fffffff);
}
