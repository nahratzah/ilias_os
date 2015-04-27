/* @(#)s_fabs.c 5.1 93/09/24 */
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

/*
 * fabs(x) returns the absolute value of x.
 */

#include <cmath>
#include "private.h"

_namespace_begin(std)


double fabs(double x) noexcept {
  u_int32_t m, l;
  tie(m, l) = extract_words(x);
  m &= 0x7fffffff;
  return set_double_from_words(m, l);
}


_namespace_end(std)
