/* e_hypotf.c -- float version of e_hypot.c.
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

//__FBSDID("$FreeBSD: src/lib/msun/src/e_hypotf.c,v 1.14 2011/10/15 07:00:28 das Exp $");

#include <cmath>
#include "private.h"

using namespace _namespace(std);

float hypotf(float x, float y) noexcept {
  int32_t ha = get_float_as_int(x) & 0x7fffffff;
  int32_t hb = get_float_as_int(y) & 0x7fffffff;
  float a, b;
  if (hb > ha) {
    a = y;
    b = x;
    swap(ha, hb);
  } else {
    a = x;
    b = y;
  }
  a = fabsf(a);
  b = fabsf(b);
  if ((ha - hb) > 0xf000000)  /* x/y > 2**30 */
    return a + b;
  int32_t k = 0;
  if (ha > 0x58800000) {  /* a>2**50 */
    if (ha >= 0x7f800000) {  /* Inf or NaN */
      /* Use original arg order iff result is NaN; quieten sNaNs. */
      float w = fabsf(x + 0.0F) - fabsf(y + 0.0F);
      if (ha == 0x7f800000) w = a;
      if (hb == 0x7f800000) w = b;
      return w;
    }
    /* scale a and b by 2**-68 */
    ha -= 0x22000000;
    hb -= 0x22000000;
    k += 68;
    a = set_float_from_int(ha);
    b = set_float_from_int(hb);
  }
  if (hb < 0x26800000) {  /* b < 2**-50 */
    if (hb <= 0x007fffff) {  /* subnormal b or 0 */
      if (hb == 0) return a;
      const float t1 = set_float_from_int(0x7e800000);  /* t1=2^126 */
      b *= t1;
      a *= t1;
      k -= 126;
    } else {  /* scale a and b by 2^68 */
      ha += 0x22000000;  /* a *= 2^68 */
      hb += 0x22000000;  /* b *= 2^68 */
      k -= 68;
      a = set_float_from_int(ha);
      b = set_float_from_int(hb);
    }
  }
  /* medium size a and b */
  float w = a - b;
  if (w > b) {
    const float t1 = set_float_from_int(ha & 0xfffff000);
    const float t2 = a - t1;
    w = sqrtf(t1 * t1 - (b * (-b) - t2 * (a + t1)));
  } else {
    a = a + a;
    const float y1 = set_float_from_int(hb & 0xfffff000);
    const float y2 = b - y1;
    const float t1 = set_float_from_int((ha + 0x00800000) & 0xfffff000);
    const float t2 = a - t1;
    w = sqrtf(t1 * y1 - (w * (-w) - (t1 * y2 + t2 * b)));
  }
  if (k != 0) {
    const float t1 = set_float_from_int(0x3f800000 + (k << 23));
    return t1 * w;
  } else {
    return w;
  }
}