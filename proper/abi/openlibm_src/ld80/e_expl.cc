/*	$OpenBSD: e_expl.c,v 1.3 2013/11/12 20:35:19 martynas Exp $	*/

/*
 * Copyright (c) 2008 Stephen L. Moshier <steve@moshier.net>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*							expl.c
 *
 *	Exponential function, long double precision
 *
 *
 *
 * SYNOPSIS:
 *
 * long double x, y, expl();
 *
 * y = expl( x );
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns e (2.71828...) raised to the x power.
 *
 * Range reduction is accomplished by separating the argument
 * into an integer k and fraction f such that
 *
 *     x    k  f
 *    e  = 2  e.
 *
 * A Pade' form of degree 2/3 is used to approximate exp(f) - 1
 * in the basic range [-0.5 ln 2, 0.5 ln 2].
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      +-10000     50000       1.12e-19    2.81e-20
 *
 *
 * Error amplification in the exponential function can be
 * a serious matter.  The error propagation involves
 * exp( X(1+delta) ) = exp(X) ( 1 + X*delta + ... ),
 * which shows that a 1 lsb error in representing X produces
 * a relative error of X times 1 lsb in the function.
 * While the routine gives an accurate result for arguments
 * that are exactly represented by a long double precision
 * computer number, the result contains amplified roundoff
 * error for large arguments not exactly represented.
 *
 *
 * ERROR MESSAGES:
 *
 *   message         condition      value returned
 * exp underflow    x < MINLOG         0.0
 * exp overflow     x > MAXLOG         MAXNUM
 *
 */

/*	Exponential function	*/

#include <cmath>
#include <cfloat>
#include <array>
#include "../private.h"
#include "../polevll.h"

using namespace _namespace(std);

long double expl(long double x) noexcept {
  static constexpr array<long double, 3> P{{
   1.2617719307481059087798E-4L,
   3.0299440770744196129956E-2L,
   9.9999999999999999991025E-1L,
  }};
  static constexpr array<long double, 4> Q{{
   3.0019850513866445504159E-6L,
   2.5244834034968410419224E-3L,
   2.2726554820815502876593E-1L,
   2.0000000000000000000897E0L,
  }};
  static constexpr long double C1 = 6.9314575195312500000000E-1L;
  static constexpr long double C2 = 1.4286068203094172321215E-6L;
  static constexpr long double MAXLOGL = 1.1356523406294143949492E4L;
  static constexpr long double MINLOGL = -1.13994985314888605586758E4L;
  static constexpr long double LOG2EL = 1.4426950408889634073599E0L;

  if (isnan(x)) return x;
  if (x > MAXLOGL) return INFINITY;
  if (x < MINLOGL) return 0.0L;

  /* Express e**x = e**g 2**n
   *   = e**g e**( n loge(2) )
   *   = e**( g + n loge(2) )
   */
  const long double px_1 = floor(LOG2EL * x + 0.5L); /* floor() truncates toward -infinity. */
  const int n = px_1;
  x -= px_1 * C1;
  x -= px_1 * C2;

  /* rational approximation for exponential
   * of the fractional part:
   * e**x =  1 + 2x P(x**2)/( Q(x**2) - P(x**2) )
   */
  const long double xx = x * x;
  const long double px = x * __polevll(xx, P.begin(), P.end());
  x = px / (__polevll(xx, Q.begin(), Q.end()) - px);
  x = 1.0L + ldexp(x, 1);

  return ldexp(x, n);
}
