/* e_powf.c -- float version of e_pow.c.
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

//__FBSDID("$FreeBSD: src/lib/msun/src/e_powf.c,v 1.16 2011/10/21 06:26:07 das Exp $");

#include <cmath>
#include <cstdint>
#include <array>
#include "private.h"

using namespace _namespace(std);

float powf(float x, float y) noexcept {
  static constexpr array<float, 2> bp{{ 1.0, 1.5 }};
  static constexpr array<float, 2> dp_h{{ 0.0, 5.84960938e-01 }};  /* 0x3f15c000 */
  static constexpr array<float, 2> dp_l{{ 0.0, 1.56322085e-06 }};  /* 0x35d1cfdc */
  static constexpr float zero     =  0.0;
  static constexpr float one	 =  1.0;
  static constexpr float two	 =  2.0;
  static constexpr float two24	 =  16777216.0;	/* 0x4b800000 */
  static constexpr float huge	 =  1.0e30;
  static constexpr float tiny     =  1.0e-30;

  /* poly coefs for (3/2)*(log(x)-2s-2/3*s**3 */
  static constexpr float L1       =  6.0000002384e-01; /* 0x3f19999a */
  static constexpr float L2       =  4.2857143283e-01; /* 0x3edb6db7 */
  static constexpr float L3       =  3.3333334327e-01; /* 0x3eaaaaab */
  static constexpr float L4       =  2.7272811532e-01; /* 0x3e8ba305 */
  static constexpr float L5       =  2.3066075146e-01; /* 0x3e6c3255 */
  static constexpr float L6       =  2.0697501302e-01; /* 0x3e53f142 */
  static constexpr float P1       =  1.6666667163e-01; /* 0x3e2aaaab */
  static constexpr float P2       = -2.7777778450e-03; /* 0xbb360b61 */
  static constexpr float P3       =  6.6137559770e-05; /* 0x388ab355 */
  static constexpr float P4       = -1.6533901999e-06; /* 0xb5ddea0e */
  static constexpr float P5       =  4.1381369442e-08; /* 0x3331bb4c */
  static constexpr float lg2      =  6.9314718246e-01; /* 0x3f317218 */
  static constexpr float lg2_h    =  6.93145752e-01;   /* 0x3f317200 */
  static constexpr float lg2_l    =  1.42860654e-06;   /* 0x35bfbe8c */
  static constexpr float ovt      =  4.2995665694e-08; /* -(128-log2(ovfl+.5ulp)) */
  static constexpr float cp       =  9.6179670095e-01; /* 0x3f76384f =2/(3ln2) */
  static constexpr float cp_h     =  9.6191406250e-01; /* 0x3f764000 =12b cp */
  static constexpr float cp_l     = -1.1736857402e-04; /* 0xb8f623c6 =tail of cp_h */
  static constexpr float ivln2    =  1.4426950216e+00; /* 0x3fb8aa3b =1/ln2 */
  static constexpr float ivln2_h  =  1.4426879883e+00; /* 0x3fb8aa00 =16b 1/ln2*/
  static constexpr float ivln2_l  =  7.0526075433e-06; /* 0x36eca570 =1/ln2 tail*/

  const int32_t hx = get_float_as_int(x),
                hy = get_float_as_int(y);
  int32_t ix = (hx & 0x7fffffff);
  const int32_t iy = (hy & 0x7fffffff);

  /* y==zero: x**0 = 1 */
  if (iy == 0) return one;

  /* x == 1: 1 ** y = 1, even if y is NaN */
  if (hx == 0x3f800000) return one;

  /* y != zero: result is NaN if either arg is NaN */
  if (ix > 0x7f800000 || iy > 0x7f800000)
    return (x + 0.0F) + (y + 0.0F);

  /* determine if y is an odd int when x < 0
   * yisint = 0	... y is not an integer
   * yisint = 1	... y is an odd int
   * yisint = 2	... y is an even int
   */
  int32_t yisint = 0;
  if (hx < 0) {
    if (iy >= 0x4b800000) {
      yisint = 2;  /* even integer y */
    } else if (iy >= 0x3f800000) {
      int32_t k = (iy >> 23) - 0x7f;  /* exponent */
      int32_t j = iy >> (23 - k);
      if ((j << (23 - k)) == iy)
        yisint = 2 - (j & 1);
    }
  }

  /* special value of y */
  if (iy == 0x7f800000) {  /* y is +-inf */
    if (ix == 0x3f800000)
      return one;  /* (-1) ** +-inf is NaN */
    else if (ix > 0x3f800000)  /* (|x| > 1) ** +-inf = inf,0 */
      return (hy >= 0 ? y : zero);
    else  /* (|x| < 1) ** -,+inf = inf,0 */
      return (hy < 0 ? -y : zero);
  }
  if (iy == 0x3f800000) {  /* y is  +-1 */
    if (hy < 0)
      return one/x;
    else
      return x;
  }
  if (hy == 0x40000000)
    return x * x;  /* y is  2 */
  if (hy == 0x40400000)
    return x * x * x;  /* y is  3 */
  if (hy == 0x40800000) {  /* y is  4 */
    const float u = x * x;
    return u * u;
  }
  if (hy == 0x3f000000) {  /* y is  0.5 */
    if (hx >= 0)  /* x >= +0 */
      return sqrtf(x);
  }

  float ax = fabsf(x);
  /* special value of x */
  if (ix == 0x7f800000 || ix == 0 || ix == 0x3f800000) {
    float z = ax;  /* x is +-0,+-inf,+-1 */
    if (hy < 0)
      z = one / z;  /* z = (1/|x|) */
    if (hx < 0) {
      if (((ix - 0x3f800000) | yisint) == 0)
        z = (z - z) / (z - z);  /* (-1) ** non-int is NaN */
      else if( yisint == 1)
        z = -z;  /* (x < 0) ** odd = -(|x| ** odd) */
    }
    return z;
  }

  int32_t n = (static_cast<u_int32_t>(hx) >> 31) - 1;

  /* (x < 0) ** (non-int) is NaN */
  if ((n | yisint) == 0)
    return (x - x) / (x - x);

  /* s (sign of result -ve ** odd) = -1 else = 1 */
  const float sn = ((n | (yisint - 1)) == 0 ? -one : one);

  /* |y| is huge */
  float t1, t2;
  if (iy > 0x4d000000) {  /* if |y| > 2**27 */
    /* over/underflow if x is not close to one */
    if (ix < 0x3f7ffff8) return (hy < 0 ? sn * huge * huge : sn * tiny * tiny);
    if (ix > 0x3f800007) return (hy > 0 ? sn * huge * huge : sn * tiny * tiny);
    /* now |1-x| is tiny <= 2 ** -20, suffice to compute
     * log(x) by x-x^2/2+x^3/3-x^4/4 */
    const float t = ax - 1;  /* t has 20 trailing zeros */
    const float w = (t * t) * (0.5f - t * (0.333333333333f - t * 0.25f));
    const float u = ivln2_h * t;  /* ivln2_h has 16 sig. bits */
    const float v = t * ivln2_l - w * ivln2;
    t1 = mask_float(u + v, 0xfffff000);
    t2 = v - (t1 - u);
  } else {
    float s2, s_h, s_l, t_h, t_l;

    n = 0;
    /* take care subnormal number */
    if (ix < 0x00800000) {
      ax *= two24;
      n -= 24;
      ix = get_float_as_int(ax);
    }
    n += (ix >> 23) - 0x7f;
    const int j = ix & 0x007fffff;
    /* determine interval */
    ix = j | 0x3f800000;  /* normalize ix */
    int k;
    if (j <= 0x1cc471)  /* |x| < sqrt(3/2) */
      k = 0;
    else if (j < 0x5db3d7)  /* |x| < sqrt(3) */
      k = 1;
    else {
      k = 0;
      n += 1;
      ix -= 0x00800000;
    }
    ax = set_float_from_int(ix);

    /* compute s = s_h+s_l = (x-1) / (x+1) or (x-1.5) / (x+1.5) */
    float u = ax - bp[k];  /* bp[0]=1.0, bp[1]=1.5 */
    float v = one / (ax + bp[k]);
    const float s = u * v;
    s_h = mask_float(s, 0xfffff000);
    /* t_h=ax+bp[k] High */
    const int32_t is = ((ix >> 1) & 0xfffff000) | 0x20000000;
    t_h = set_float_from_int(is + 0x00400000 + (k << 21));
    t_l = ax - (t_h - bp[k]);
    s_l = v * ((u - s_h * t_h) - s_h * t_l);
    /* compute log(ax) */
    s2 = s * s;
    const float r =
        s2 * s2 *
        (L1 + s2 * (L2 + s2 * (L3 + s2 * (L4 + s2 * (L5 + s2 * L6))))) +
        s_l * (s_h + s);
    s2 = s_h * s_h;
    t_h = mask_float(3.0f + s2 + r, 0xfffff000);
    t_l = r - ((t_h - 3.0f) - s2);
    /* u+v = s*(1+...) */
    u = s_h * t_h;
    v = s_l * t_h + t_l * s;
    /* 2/(3log2)*(s+...) */
    const float p_h = mask_float(u + v, 0xfffff000);
    const float p_l = v - (p_h - u);
    const float z_h = cp_h * p_h;  /* cp_h+cp_l = 2/(3*log2) */
    const float z_l = cp_l * p_h + p_l * cp + dp_l[k];
    /* log2(ax) = (s+..)*2/(3*log2) = n + dp_h + z_h + z_l */
    const float t = static_cast<float>(n);
    t1 = mask_float(z_h + z_l + dp_h[k] + t, 0xfffff000);
    t2 = z_l - (t1 - t - dp_h[k] - z_h);
  }

  /* split up y into y1+y2 and compute (y1+y2)*(t1+t2) */
  const float y1 = mask_float(y, 0xfffff000);
  const float p_l = (y - y1) * t1 + y * t2;
  float p_h = y1 * t1;
  float z = p_l + p_h;
  int j = get_float_as_int(z);
  if (j > 0x43000000) {  /* if z > 128 */
    return sn * huge * huge;  /* overflow */
  } else if (j==0x43000000) {  /* if z == 128 */
    if (p_l + ovt > z - p_h)
      return sn * huge * huge;  /* overflow */
  } else if ((j & 0x7fffffff) > 0x43160000) {  /* z <= -150 */
    return sn * tiny * tiny;  /* underflow */
  } else if (static_cast<uint32_t>(j) == 0xc3160000) {  /* z == -150 */
    if (p_l <= z - p_h)
      return sn * tiny * tiny;  /* underflow */
  }
  /*
   * compute 2**(p_h+p_l)
   */
  const int32_t i = j & 0x7fffffff;
  int32_t k = (i >> 23) - 0x7f;
  n = 0;
  if (i > 0x3f000000) {  /* if |z| > 0.5, set n = [z+0.5] */
    n = j + (0x00800000 >> (k + 1));
    k = ((n & 0x7fffffff) >> 23) - 0x7f;  /* new k for n */
    const float t = set_float_from_int(n & ~ (0x007fffff >> k));
    n = ((n & 0x007fffff) | 0x00800000) >> (23 - k);
    if (j < 0) n = -n;
    p_h -= t;
  }
  float t = mask_float(p_l + p_h, 0xffff8000);
  const float u = t * lg2_h;
  const float v = (p_l - (t - p_h)) * lg2 + t * lg2_l;
  z = u + v;
  const float w = v - (z - u);
  t = z * z;
  t1 = z - t * (P1 + t * (P2 + t * (P3 + t * (P4 + t * P5))));
  const float r = (z * t1) / (t1 - two) - (w + z * w);
  z = one - (r - z);
  j = get_float_as_int(z);
  j += (n << 23);
  if ((j >> 23) <= 0)
    z = scalbnf(z, n);  /* subnormal output */
  else
    z = set_float_from_int(j);
  return sn * z;
}
