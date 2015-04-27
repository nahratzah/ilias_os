/* s_nextafterf.c -- float version of s_nextafter.c.
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

//__FBSDID("$FreeBSD: src/lib/msun/src/s_nextafterf.c,v 1.11 2008/02/22 02:30:35 das Exp $");

#include <cmath>
#include "private.h"

using namespace _namespace(std);

float nextafterf(float x, float y) noexcept {
#if 0
	volatile float t;
	int32_t hx,hy,ix,iy;
#endif

  int32_t hx = get_float_as_int(x);
  int32_t hy = get_float_as_int(y);
  const int32_t ix = hx & 0x7fffffff;  /* |x| */
  const int32_t iy = hy & 0x7fffffff;  /* |y| */

  if ((ix>0x7f800000) ||  /* x is nan */
      (iy>0x7f800000))  /* y is nan */
    return x + y;
  if (x == y) return y;  /* x=y, return y */
  if (ix == 0) {  /* x == 0 */
    x = set_float_from_int((hy & 0x80000000) | 1);  /* return +-minsubnormal */
    volatile float t = x * x;
    if (t == x)  /* raise underflow flag */
      return t;
    else
      return x;
  }
  if (hx >= 0) {  /* x > 0 */
    if (hx > hy)  /* x > y, x -= ulp */
      hx -= 1;
    else  /* x < y, x += ulp */
      hx += 1;
  } else {  /* x < 0 */
    if (hy >= 0 || hx > hy)  /* x < y, x -= ulp */
      hx -= 1;
    else  /* x > y, x += ulp */
      hx += 1;
  }
  hy = hx & 0x7f800000;
  if (hy >= 0x7f800000) return x + x;  /* overflow  */
  if (hy < 0x00800000) {  /* underflow */
    volatile float t = x * x;
    if (t != x)  /* raise underflow flag */
      return set_float_from_int(hx);
  }
  return set_float_from_int(hx);
}
