/*-
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/* @(#)gamma.c	8.1 (Berkeley) 6/4/93 */
//__FBSDID("$FreeBSD: src/lib/msun/bsdsrc/b_tgamma.c,v 1.10 2008/02/22 02:26:51 das Exp $");

/*
 * This code by P. McIlroy, Oct 1992;
 *
 * The financial support of UUNET Communications Services is greatfully
 * acknowledged.
 */

#include <cmath>
#include "private.h"

/* METHOD:
 * x < 0: Use reflection formula, G(x) = pi/(sin(pi*x)*x*G(x))
 *	At negative integers, return NaN and raise invalid.
 *
 * x < 6.5:
 *	Use argument reduction G(x+1) = xG(x) to reach the
 *	range [1.066124,2.066124].  Use a rational
 *	approximation centered at the minimum (x0+1) to
 *	ensure monotonicity.
 *
 * x >= 6.5: Use the asymptotic approximation (Stirling's formula)
 *	adjusted for equal-ripples:
 *
 *	log(G(x)) ~= (x-.5)*(log(x)-1) + .5(log(2*pi)-1) + 1/x*P(1/(x*x))
 *
 *	Keep extra precision in multiplying (x-.5)(log(x)-1), to
 *	avoid premature round-off.
 *
 * Special values:
 *	-Inf:			return NaN and raise invalid;
 *	negative integer:	return NaN and raise invalid;
 *	other x ~< 177.79:	return +-0 and raise underflow;
 *	+-0:			return +-Inf and raise divide-by-zero;
 *	finite x ~> 171.63:	return +Inf and raise overflow;
 *	+Inf:			return +Inf;
 *	NaN:			return NaN.
 *
 * Accuracy: tgamma(x) is accurate to within
 *	x > 0:  error provably < 0.9ulp.
 *	Maximum observed in 1,000,000 trials was .87ulp.
 *	x < 0:
 *	Maximum observed error < 4ulp in 1,000,000 trials.
 */

_namespace_begin(std)


using impl::Double;
using impl::__exp__D;
using impl::__log__D;
using impl::TRUNC;


namespace {


static constexpr double zero = 0., one = 1.0, tiny = 1e-300;

/*
 * Rational approximation, A0 + x*x*P(x)/Q(x), on the interval
 * [1.066.., 2.066..] accurate to 4.25e-19.
 */
constexpr double LEFT = -.3955078125;  /* left boundary for rat. approx */
constexpr double x0 = .461632144968362356785;  /* xmin - 1 */


/*
 * returns (z+c)^2 * P(z)/Q(z) + a0
 */
inline Double ratfun_gam(double z, double c) noexcept {
  static constexpr double a0_hi = 0.88560319441088874992;
  static constexpr double a0_lo = -.00000000000000004996427036469019695;

  static constexpr double P0 =  6.21389571821820863029017800727e-01;
  static constexpr double P1 =  2.65757198651533466104979197553e-01;
  static constexpr double P2 =  5.53859446429917461063308081748e-03;
  static constexpr double P3 =  1.38456698304096573887145282811e-03;
  static constexpr double P4 =  2.40659950032711365819348969808e-03;
  static constexpr double Q0 =  1.45019531250000000000000000000e+00;
  static constexpr double Q1 =  1.06258521948016171343454061571e+00;
  static constexpr double Q2 = -2.07474561943859936441469926649e-01;
  static constexpr double Q3 = -1.46734131782005422506287573015e-01;
  static constexpr double Q4 =  3.07878176156175520361557573779e-02;
  static constexpr double Q5 =  5.12449347980666221336054633184e-03;
  static constexpr double Q6 = -1.76012741431666995019222898833e-03;
  static constexpr double Q7 =  9.35021023573788935372153030556e-05;
  static constexpr double Q8 =  6.13275507472443958924745652239e-06;

  double q = Q0 + z * (Q1 + z * (Q2 + z * (Q3 + z * (Q4 + z * (Q5 + z * (Q6 + z * (Q7 + z * Q8)))))));
  double p = P0 + z * (P1 + z * (P2 + z * (P3 + z * P4)));

  /* return r.a + r.b = a0 + (z+c)^2*p/q, with r.a truncated to 26 bits. */
  p = p/q;
  Double t;
  t.a = TRUNC(z);  /* t ~= z + c */
  t.b = (z - t.a) + c;
  t.b *= (t.a + z);
  q = (t.a *= t.a);  /* t = (z+c)^2 */
  t.a = TRUNC(t.a);
  t.b += (q - t.a);
  Double r;
  r.a = TRUNC(p);  /* r = P/Q */
  r.b = p - r.a;
  t.b = t.b * p + t.a * r.b + a0_lo;
  t.a *= r.a;  /* t = (z+c)^2*(P/Q) */
  r.a = TRUNC(t.a + a0_hi);
  r.b = ((a0_hi - r.a) + t.a) + t.b;
  return r;  /* r = a0 + t */
}

/*
 * Accurate to max(ulp(1/128) absolute, 2^-66 relative) error.
 */
inline Double large_gam(double x) noexcept {
  /*
   * Constants for large x approximation (x in [6, Inf])
   * (Accurate to 2.8*10^-19 absolute)
   */
  static constexpr double lns2pi_hi = 0.418945312500000;
  static constexpr double lns2pi_lo = -.000006779295327258219670263595;
  static constexpr double Pa0 =  8.33333333333333148296162562474e-02;
  static constexpr double Pa1 = -2.77777777774548123579378966497e-03;
  static constexpr double Pa2 =  7.93650778754435631476282786423e-04;
  static constexpr double Pa3 = -5.95235082566672847950717262222e-04;
  static constexpr double Pa4 =  8.41428560346653702135821806252e-04;
  static constexpr double Pa5 = -1.89773526463879200348872089421e-03;
  static constexpr double Pa6 =  5.69394463439411649408050664078e-03;
  static constexpr double Pa7 = -1.44705562421428915453880392761e-02;

  const double z = one / (x * x);
  double p = Pa0 + z * (Pa1 + z * (Pa2 + z * (Pa3 + z * (Pa4 + z * (Pa5 + z * (Pa6 + z * Pa7))))));
  p /= x;

  Double u = __log__D(x);
  u.a -= one;
  Double v;
  v.a = TRUNC(x -= .5);
  v.b = x - v.a;
  Double t;
  t.a = v.a * u.a;  /* t = (x-.5)*(log(x)-1) */
  t.b = v.b * u.a + x * u.b;
  /* return t.a + t.b + lns2pi_hi + lns2pi_lo + p */
  t.b += lns2pi_lo;
  t.b += p;
  u.a = lns2pi_hi + t.b;
  u.a += t.a;
  u.b = t.a - u.a;
  u.b += lns2pi_hi;
  u.b += t.b;
  return u;
}

/*
 * Good to < 1 ulp.  (provably .90 ulp; .87 ulp on 1,000,000 runs.)
 * It also has correct monotonicity.
 */
inline double small_gam(double x) noexcept {
  double y = x - one;
  double ym1 = y - one;
  if (y <= 1.0 + (LEFT + x0)) {
    const Double yy = ratfun_gam(y - x0, 0);
    return yy.a + yy.b;
  }
  Double r;
  r.a = TRUNC(y);
  Double yy;
  yy.a = r.a - one;
  y = ym1;
  yy.b = r.b = y - yy.a;
  /* Argument reduction: G(x+1) = x*G(x) */
  for (ym1 = y - one; ym1 > LEFT + x0; y = ym1--, yy.a--) {
    const double t = r.a * yy.a;
    r.b = r.a * yy.b + y * r.b;
    r.a = TRUNC(t);
    r.b += (t - r.a);
  }
  /* Return r*tgamma(y). */
  yy = ratfun_gam(y - x0, 0);
  y = r.b * (yy.a + yy.b) + r.a * yy.b;
  y += yy.a * r.a;
  return y;
}

/*
 * Good on (0, 1+x0+LEFT].  Accurate to 1ulp.
 */
inline double smaller_gam(double x) noexcept {
  double t, d;
  Double xx;
  if (x < x0 + LEFT) {
    t = TRUNC(x);
    d = (t + x) * (x - t);
    t *= t;
    xx.a = TRUNC(t + x);
    xx.b = x - xx.a;
    xx.b += t;
    xx.b += d;
    t = one - x0;
    t += x;
    d = one - x0;
    d -= t;
    d += x;
    x = xx.a + xx.b;
  } else {
    xx.a = TRUNC(x);
    xx.b = x - xx.a;
    t = x - x0;
    d = -x0 - t;
    d += x;
  }
  Double r;
  r = ratfun_gam(t, d);
  d = TRUNC(r.a / x);
  r.a -= d * xx.a;
  r.a -= d * xx.b;
  r.a += r.b;
  return d + r.a / x;
}

inline double neg_gam(double x) noexcept {
  double y = ceil(x);
  if (y == x)  /* Negative integer. */
    return (x - x) / zero;
  double z = y - x;
  if (z > 0.5)
    z = one - z;
  y *= 0.5;
  const int sgn = (y == ceil(y) ? -1 : 1);
  if (z < .25)
    z = sin(M_PI * z);
  else
    z = cos(M_PI * (0.5 - z));
  /* Special case: G(1-x) = Inf; G(x) may be nonzero. */
  if (x < -170) {
    if (x < -190) return static_cast<double>(sgn) * tiny * tiny;
    y = one - x;  /* exact: 128 < |x| < 255 */
    Double lg = large_gam(y);
    const Double lsine = __log__D(M_PI / z);  /* = TRUNC(log(u)) + small */
    lg.a -= lsine.a;  /* exact (opposite signs) */
    lg.b -= lsine.b;
    y = -(lg.a + lg.b);
    z = (y + lg.a) + lg.b;
    y = __exp__D(y, z);
    if (sgn < 0) y = -y;
    return y;
  }
  y = one - x;
  if (one - y == x)
    y = tgamma(y);
  else  /* 1-x is inexact */
    y = -x * tgamma(-x);
  if (sgn < 0) y = -y;
  return M_PI / (y*z);
}


} /* namespace std::<unnamed> */


double tgamma(double x) noexcept {
  if (x >= 6) {
    if (x > 171.63)
      return x / zero;
    const Double u = large_gam(x);
    return __exp__D(u.a, u.b);
  } else if (x >= 1.0 + LEFT + x0) {
    return small_gam(x);
  } else if (x > 1.e-17) {
    return smaller_gam(x);
  } else if (x > -1.e-17) {
    Double u;
    if (x != 0.0)
      u.a = one - tiny;  /* raise inexact */
    return one / x;
  } else if (!isfinite(x)) {
    return x - x;  /* x is NaN or -Inf */
  } else {
    return neg_gam(x);
  }
}


_namespace_end(std)
