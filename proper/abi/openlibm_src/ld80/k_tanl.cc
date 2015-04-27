/* From: @(#)k_tan.c 1.5 04/04/22 SMI */

/*
 * ====================================================
 * Copyright 2004 Sun Microsystems, Inc.  All Rights Reserved.
 * Copyright (c) 2008 Steven G. Kargl, David Schultz, Bruce D. Evans.
 *
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 */

//__FBSDID("$FreeBSD: src/lib/msun/ld80/k_tanl.c,v 1.3 2008/02/18 15:39:52 bde Exp $");

/*
 * ld80 version of k_tan.c.  See ../src/k_tan.c for most comments.
 */

#include <cmath>
#include "../private.h"

_namespace_begin(std)
namespace impl {


long double __kernel_tanl(long double x, long double y, int iy) noexcept {
  /*
   * Domain [-0.67434, 0.67434], range ~[-2.25e-22, 1.921e-22]
   * |tan(x)/x - t(x)| < 2**-71.9
   *
   * See k_cosl.c for more details about the polynomial.
   */
  static constexpr long double T3 =   0.333333333333333333180L;  /*  0xaaaaaaaaaaaaaaa5.0p-65 */
  static constexpr long double T5 =   0.133333333333333372290L;  /*  0x88888888888893c3.0p-66 */
  static constexpr long double T7 =   0.0539682539682504975744L;  /*  0xdd0dd0dd0dc13ba2.0p-68 */
  static constexpr long double pio4 = 0.785398163397448309628L;  /*  0xc90fdaa22168c235.0p-64 */
  static constexpr long double pio4lo = -1.25413940316708300586e-20L;  /* -0xece675d1fc8f8cbb.0p-130 */

  static constexpr double T9  =  0.021869488536312216;  /*  0x1664f4882cc1c2.0p-58 */
  static constexpr double T11 =  0.0088632355256619590;  /*  0x1226e355c17612.0p-59 */
  static constexpr double T13 =  0.0035921281113786528;  /*  0x1d6d3d185d7ff8.0p-61 */
  static constexpr double T15 =  0.0014558334756312418;  /*  0x17da354aa3f96b.0p-62 */
  static constexpr double T17 =  0.00059003538700862256;  /*  0x13559358685b83.0p-63 */
  static constexpr double T19 =  0.00023907843576635544;  /*  0x1f56242026b5be.0p-65 */
  static constexpr double T21 =  0.000097154625656538905;  /*  0x1977efc26806f4.0p-66 */
  static constexpr double T23 =  0.000038440165747303162;  /*  0x14275a09b3ceac.0p-67 */
  static constexpr double T25 =  0.000018082171885432524;  /*  0x12f5e563e5487e.0p-68 */
  static constexpr double T27 =  0.0000024196006108814377;  /*  0x144c0d80cc6896.0p-71 */
  static constexpr double T29 =  0.0000078293456938132840;  /*  0x106b59141a6cb3.0p-69 */
  static constexpr double T31 = -0.0000032609076735050182;  /* -0x1b5abef3ba4b59.0p-71 */
  static constexpr double T33 =  0.0000023261313142559411;  /*  0x13835436c0c87f.0p-71 */

#if 0
	long double z, r, v, w, s;
	long double osign;
	int i;
#endif

  iy = (iy == 1 ? -1 : 1);  /* XXX recover original interface */
  const long double osign = (x >= 0 ? 1.0 : -1.0);  /* XXX slow, probably wrong for -0 */
  int i;
  if (fabsl(x) >= 0.67434) {
    if (x < 0) {
      x = -x;
      y = -y;
    }
    const long double z = pio4 - x;
    const long double w = pio4lo - y;
    x = z + w;
    y = 0.0;
    i = 1;
  } else {
    i = 0;
  }
  long double z = x * x;
  long double w = z * z;
  long double r = T5 + w * (T9 + w * (T13 + w * (T17 + w * (T21 + w * (T25 + w * (T29 + w * T33))))));
  long double v = z * (T7 + w * (T11 + w * (T15 + w * (T19 + w * (T23 + w * (T27 + w * T31))))));
  long double s = z * x;
  r = y + z * (s * (r + v) + y);
  r += T3 * s;
  w = x + r;
  if (i == 1) {
    v = static_cast<long double>(iy);
    return osign * (v - 2.0 * (x - (w * w / (w + v) - r)));
  }
  if (iy == 1) {
    return w;
  } else {
    /*
     * if allow error up to 2 ulp, simply return
     * -1.0 / (x+r) here
     */
    /* compute -1.0 / (x+r) accurately */
    z = w;
    z = z + 0x1p32 - 0x1p32;
    v = r - (z - x);  /* z+v = r+x */
    const long double a = -1.0 / w;  /* a = -1.0/w */
    const long double t = a + 0x1p32 - 0x1p32;
    s = 1.0 + t * z;
    return t + a * (s + t * v);
  }
}


} /* namespace impl */
_namespace_end(std)
