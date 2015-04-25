
/* @(#)e_lgamma.c 1.3 95/01/18 */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunSoft, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 *
 */

//__FBSDID("$FreeBSD: src/lib/msun/src/e_lgamma.c,v 1.9 2008/02/22 02:30:35 das Exp $");

/* __ieee754_lgamma(x)
 * Return the logarithm of the Gamma function of x.
 *
 * Method: call __ieee754_lgamma_r
 */

#include <cmath>
#include "private.h"

_namespace_begin(std)


double lgamma(double x) noexcept {
  return lgamma_r(x, &signgam);
}


_namespace_end(std)
