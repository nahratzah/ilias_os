/*-
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 * Copyright (c) 2009-2011, Bruce D. Evans, Steven G. Kargl, David Schultz.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 *
 * The argument reduction and testing for exceptional cases was
 * written by Steven G. Kargl with input from Bruce D. Evans
 * and David A. Schultz.
 */

//__FBSDID("$FreeBSD: src/lib/msun/src/s_cbrtl.c,v 1.1 2011/03/12 19:37:35 kargl Exp $");

#include <cmath>
#include <cfloat>
#include <ieeefp.h>
#include "private.h"

using namespace _namespace(std);

long double cbrtl(long double x) noexcept {
  static constexpr int BIAS = LDBL_MAX_EXP - 1;
  static constexpr uint32_t B1 = 709958130;  /* B1 = (127-127.0/3-0.03306235651)*2**23 */

#if 0
	union IEEEl2bits u, v;
	long double r, s, t, w;
	double dr, dt, dx;
	float ft, fx;
	u_int32_t hx;
	u_int16_t expsign;
	int k;
#endif

  IEEE_l2bits u = IEEE_l2bits_from_ldbl(x);
  const uint16_t expsign = IEEE_l2xbits_from_ldbl(x).expsign;
  int k = expsign & 0x7fff;

  /*
   * If x = +-Inf, then cbrt(x) = +-Inf.
   * If x = NaN, then cbrt(x) = NaN.
   */
  if (k == BIAS + LDBL_MAX_EXP) return x + x;

#ifdef __i386__
  fp_prec_t oprec;

  oprec = fpgetprec();
  if (oprec != FP_PE) fpsetprec(FP_PE);
#endif

  if (k == 0) {
    /* If x = +-0, then cbrt(x) = +-0. */
    if ((u.manh | u.manl) == 0) {
#ifdef __i386__
      if (oprec != FP_PE) fpsetprec(oprec);
#endif
      return (x);
    }
    /* Adjust subnormal numbers. */
    u = IEEE_l2bits_from_ldbl(ldbl_from_IEEE_l2bits(u) * 0x1.0p514);
    k = u.exp;
    k -= BIAS + 514;
  } else {
    k -= BIAS;
  }

  {
    IEEE_l2xbits ux = IEEE_l2xbits_from_ldbl(ldbl_from_IEEE_l2bits(u));
    ux.expsign = BIAS;
    u = IEEE_l2bits_from_ldbl(ldbl_from_IEEE_l2xbits(ux));
  }
  IEEE_l2bits v = IEEE_l2bits_from_ldbl(1);

  x = ldbl_from_IEEE_l2bits(u);
  switch (k % 3) {
  case 1:
  case -2:
    x = 2 * x;
    k--;
    break;
  case 2:
  case -1:
    x = 4 * x;
    k -= 2;
    break;
  }
  {
    IEEE_l2xbits vx = IEEE_l2xbits_from_ldbl(ldbl_from_IEEE_l2bits(v));
    vx.expsign = (expsign & 0x8000) | (BIAS + k / 3);
    v = IEEE_l2bits_from_ldbl(ldbl_from_IEEE_l2xbits(vx));
  }

  /*
   * The following is the guts of s_cbrtf, with the handling of
   * special values removed and extra care for accuracy not taken,
   * but with most of the extra accuracy not discarded.
   */

  /* ~5-bit estimate: */
  const float fx = x;
  const uint32_t hx = get_float_as_int(fx);
  const float ft = set_float_from_int((hx & 0x7fffffff) / 3 + B1);

  /* ~16-bit estimate: */
  const double dx = x;
  double dt = ft;
  double dr = dt * dt * dt;
  dt = dt * (dx + dx + dr) / (dx + dr + dr);

  /* ~47-bit estimate: */
  dr = dt * dt * dt;
  dt = dt * (dx + dx + dr) / (dx + dr + dr);

#if LDBL_MANT_DIG == 64
  /*
   * dt is cbrtl(x) to ~47 bits (after x has been reduced to 1 <= x < 8).
   * Round it away from zero to 32 bits (32 so that t*t is exact, and
   * away from zero for technical reasons).
   */
  const double vd2 = 0x1.0p32;
  const double vd1 = 0x1.0p-31;
  const auto vd = static_cast<long double>(vd2) + vd1;

  long double t = dt + vd - 0x1.0p32;
#elif LDBL_MANT_DIG == 113
  /*
   * Round dt away from zero to 47 bits.  Since we don't trust the 47,
   * add 2 47-bit ulps instead of 1 to round up.  Rounding is slow and
   * might be avoidable in this case, since on most machines dt will
   * have been evaluated in 53-bit precision and the technical reasons
   * for rounding up might not apply to either case in cbrtl() since
   * dt is much more accurate than needed.
   */
  long double t = dt + 0x2.0p-46 + 0x1.0p60L - 0x1.0p60;
#else
# error "Unsupported long double format"
#endif

  /*
   * Final step Newton iteration to 64 or 113 bits with
   * error < 0.667 ulps
   */
  long double s = t * t;  /* t*t is exact */
  long double r = x / s;  /* error <= 0.5 ulps; |r| < |t| */
  long double w = t + t;  /* t+t is exact */
  r = (r - t) / (w + r);  /* r-t is exact; w+r ~= 3*t */
  t = t + t * r;  /* error <= 0.5 + 0.5/3 + epsilon */

  t *= ldbl_from_IEEE_l2bits(v);
#ifdef __i386__
  if (oprec != FP_PE) fpsetprec(oprec);
#endif
  return (t);
}
