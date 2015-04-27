/* From: @(#)e_rem_pio2.c 1.4 95/01/18 */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 * Copyright (c) 2008 Steven G. Kargl, David Schultz, Bruce D. Evans.
 *
 * Developed at SunSoft, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 *
 * Optimized by Bruce D. Evans.
 */

//__FBSDID("$FreeBSD: src/lib/msun/ld80/e_rem_pio2l.h,v 1.3 2011/06/18 13:56:33 benl Exp $");

/* ld80 version of __ieee754_rem_pio2l(x,y)
 *
 * return the remainder of x rem pi/2 in y[0]+y[1]
 * use __kernel_rem_pio2()
 */

#include <cmath>
#include <cfloat>
#include <array>
#include "../private.h"

#if LDBL_MANT_DIG == 64
_namespace_begin(std)
namespace impl {


//VBS
//static inline __always_inline int
//__ieee754_rem_pio2l(long double x, long double *y)

int __ieee754_rem_pio2l(long double x, long double *y) noexcept {
  static constexpr auto BIAS = (LDBL_MAX_EXP - 1);

  /*
   * invpio2:  64 bits of 2/pi
   * pio2_1:   first  39 bits of pi/2
   * pio2_1t:  pi/2 - pio2_1
   * pio2_2:   second 39 bits of pi/2
   * pio2_2t:  pi/2 - (pio2_1+pio2_2)
   * pio2_3:   third  39 bits of pi/2
   * pio2_3t:  pi/2 - (pio2_1+pio2_2+pio2_3)
   */
  static constexpr double zero =  0.00000000000000000000e+00;  /* 0x00000000, 0x00000000 */
  static constexpr double two24 =  1.67772160000000000000e+07;  /* 0x41700000, 0x00000000 */
  static constexpr double pio2_1  =  1.57079632679597125389e+00;  /* 0x3FF921FB, 0x54444000 */
  static constexpr double pio2_2  = -1.07463465549783099519e-12;  /* -0x12e7b967674000.0p-92 */
  static constexpr double pio2_3  =  6.36831716351370313614e-25;  /*  0x18a2e037074000.0p-133 */

  static constexpr long double invpio2 =  6.36619772367581343076e-01L;  /*  0xa2f9836e4e44152a.0p-64 */
  static constexpr long double pio2_1t = -1.07463465549719416346e-12L;  /* -0x973dcb3b399d747f.0p-103 */
  static constexpr long double pio2_2t =  6.36831716351095013979e-25L;  /*  0xc51701b839a25205.0p-144 */
  static constexpr long double pio2_3t = -2.75299651904407171810e-37L;  /* -0xbb5bf6c7ddd660ce.0p-185 */

#if 0
	union IEEEl2bits u,u1;
	long double z,w,t,r,fn;
	double tx[3],ty[2];
	int e0,ex,i,j,nx,n;
	int16_t expsign;
#endif

  const IEEE_l2xbits u = IEEE_l2xbits_from_ldbl(x);
  const int16_t expsign = u.expsign;
  const int ex = expsign & 0x7fff;
  if (ex < BIAS + 25 || (ex == BIAS + 25 && (u.man >> 32) < 0xc90fdaa2)) {
    /* |x| ~< 2^25*(pi/2), medium size */
    /* Use a specialized rint() to get fn.  Assume round-to-nearest. */
    long double fn = x * invpio2 + 0x1.8p63;
    fn = fn - 0x1.8p63;
#ifdef HAVE_EFFICIENT_IRINT
    const int n  = irint(fn);
#else
    const int n  = fn;
#endif
    long double r = x - fn * pio2_1;
    long double w = fn * pio2_1t;  /* 1st round good to 102 bit */
    {
      const int j = ex;
      y[0] = r-w;
      IEEE_l2xbits u2 = IEEE_l2xbits_from_ldbl(y[0]);
      int ex1 = u2.expsign & 0x7fff;
      int i = j - ex1;
      if (i > 22) {  /* 2nd iteration needed, good to 141 */
        long double t = r;
        w = fn * pio2_2;
        r = t - w;
        w = fn * pio2_2t - ((t - r) - w);
        y[0] = r - w;
        u2 = IEEE_l2xbits_from_ldbl(y[0]);
        ex1 = u2.expsign & 0x7fff;
        i = j - ex1;
        if (i > 61) {  /* 3rd iteration need, 180 bits acc */
          t = r;  /* will cover all possible cases */
          w = fn * pio2_3;
          r = t - w;
          w = fn * pio2_3t - (t - r - w);
          y[0] = r - w;
        }
      }
    }
    y[1] = (r - y[0]) - w;
    return n;
  }
  /*
   * all other (large) arguments
   */
  if (ex == 0x7fff) {  /* x is inf or NaN */
    y[0] = y[1] = x - x;
    return 0;
  }
  /* set z = scalbn(|x|,ilogb(x)-23) */
  IEEE_l2xbits u1 = IEEE_l2xbits_from_ldbl(x);
  const int e0 = ex - BIAS - 23;  /* e0 = ilogb(|x|)-23; */
  u1.expsign = ex - e0;
  long double z = ldbl_from_IEEE_l2xbits(u1);
  array<double, 3> tx;
  for (int i = 0; i < 2; i++) {
    tx[i] = static_cast<double>(static_cast<int32_t>(z));
    z = (z - tx[i]) * two24;
  }
  tx[2] = z;
  int nx = 3;
  while (tx[nx - 1] == zero) nx--;  /* skip zero term */
  array<double, 2> ty;
  const int n = __kernel_rem_pio2(tx.data(), ty.data(), e0, nx, 2);
  const long double r = static_cast<long double>(ty[0]) + ty[1];
  const long double w = ty[1] - (r - ty[0]);
  if (expsign < 0) {
    y[0] = -r;
    y[1] = -w;
    return -n;
  }
  y[0] = r;
  y[1] = w;
  return n;
}


} /* namespace std::impl */
_namespace_end(std)
#endif // LDBL_MANT_DIG == 64
