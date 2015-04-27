/*	$OpenBSD: e_tgammal.c,v 1.4 2013/11/12 20:35:19 martynas Exp $	*/

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

/*							tgammal.c
 *
 *	Gamma function
 *
 *
 *
 * SYNOPSIS:
 *
 * long double x, y, tgammal();
 *
 * y = tgammal( x );
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns gamma function of the argument.  The result is correctly
 * signed.  This variable is also filled in by the logarithmic gamma
 * function lgamma().
 *
 * Arguments |x| <= 13 are reduced by recurrence and the function
 * approximated by a rational function of degree 7/8 in the
 * interval (2,3).  Large arguments are handled by Stirling's
 * formula. Large negative arguments are made positive using
 * a reflection formula.
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE     -40,+40      10000       3.6e-19     7.9e-20
 *    IEEE    -1755,+1755   10000       4.8e-18     6.5e-19
 *
 * Accuracy for large arguments is dominated by error in powl().
 *
 */

#include <cmath>
#include <cfloat>
#include <array>
#include "../private.h"
#include "../polevll.h"

using namespace _namespace(std);

namespace {


/* Gamma function computed by Stirling's formula.
 */
inline long double stirf(long double x) noexcept {
  /* Stirling's formula for the gamma function
   * tgamma(x) = sqrt(2 pi) x^(x-.5) exp(-x) (1 + 1/x P(1/x))
   * z(x) = x
   * 13 <= x <= 1024
   * Relative error
   * n=8, d=0
   * Peak error =  9.44e-21
   * Relative error spread =  8.8e-4
   */
  static array<long double, 9> STIR{{
     7.147391378143610789273E-4L,
    -2.363848809501759061727E-5L,
    -5.950237554056330156018E-4L,
     6.989332260623193171870E-5L,
     7.840334842744753003862E-4L,
    -2.294719747873185405699E-4L,
    -2.681327161876304418288E-3L,
     3.472222222230075327854E-3L,
     8.333333333333331800504E-2L,
  }};
  static constexpr long double MAXSTIR = 1024.0L;
  static constexpr long double SQTPI = 2.50662827463100050242E0L;

#if 0
	long double y, w, v;
#endif

  long double w = 1.0L / x;
  /* For large x, use rational coefficients from the analytical expansion.  */
  if (x > 1024.0L) {
    w = (((((6.97281375836585777429E-5L * w +
             7.84039221720066627474E-4L) * w -
            2.29472093621399176955E-4L) * w -
           2.68132716049382716049E-3L) * w +
          3.47222222222222222222E-3L) * w +
         8.33333333333333333333E-2L) * w +
        1.0L;
  } else {
    w = 1.0L + w * __polevll(w, STIR.begin(), STIR.end());
  }
  long double y = expl(x);
  if (x > MAXSTIR) {  /* Avoid overflow in pow() */
    const long double v = powl(x, 0.5L * x - 0.25L);
    y = v * (v / y);
  } else {
    y = powl(x, x - 0.5L) / y;
  }
  y = SQTPI * y * w;
  return y;
}


} /* namespace <unnamed> */


long double tgammal(long double x) noexcept {
  /*
   * tgamma(x+2)  = tgamma(x+2) P(x)/Q(x)
   * 0 <= x <= 1
   * Relative error
   * n=7, d=8
   * Peak error =  1.83e-20
   * Relative error spread =  8.4e-23
   */
  static array<long double, 8> P{{
     4.212760487471622013093E-5L,
     4.542931960608009155600E-4L,
     4.092666828394035500949E-3L,
     2.385363243461108252554E-2L,
     1.113062816019361559013E-1L,
     3.629515436640239168939E-1L,
     8.378004301573126728826E-1L,
     1.000000000000000000009E0L,
  }};
  static array<long double, 9> Q{{
    -1.397148517476170440917E-5L,
     2.346584059160635244282E-4L,
    -1.237799246653152231188E-3L,
    -7.955933682494738320586E-4L,
     2.773706565840072979165E-2L,
    -4.633887671244534213831E-2L,
    -2.243510905670329164562E-1L,
     4.150160950588455434583E-1L,
     9.999999999999999999908E-1L,
  }};
  static constexpr long double MAXGAML = 1755.455L;

  /* 1/tgamma(x) = z P(z)
   * z(x) = 1/x
   * 0 < x < 0.03125
   * Peak relative error 4.2e-23
   */
  static array<long double, 9> S{{
    -1.193945051381510095614E-3L,
     7.220599478036909672331E-3L,
    -9.622023360406271645744E-3L,
    -4.219773360705915470089E-2L,
     1.665386113720805206758E-1L,
    -4.200263503403344054473E-2L,
    -6.558780715202540684668E-1L,
     5.772156649015328608253E-1L,
     1.000000000000000000000E0L,
  }};

  /* 1/tgamma(-x) = z P(z)
   * z(x) = 1/x
   * 0 < x < 0.03125
   * Peak relative error 5.16e-23
   * Relative error spread =  2.5e-24
   */
  static array<long double, 9> SN{{
     1.133374167243894382010E-3L,
     7.220837261893170325704E-3L,
     9.621911155035976733706E-3L,
    -4.219773343731191721664E-2L,
    -1.665386113944413519335E-1L,
    -4.200263503402112910504E-2L,
     6.558780715202536547116E-1L,
     5.772156649015328608727E-1L,
    -1.000000000000000000000E0L,
  }};
  static constexpr long double PIL = 3.1415926535897932384626L;

#if 0
	long double p, q, z;
	int i;
#endif

  if (isnan(x)) return NAN;
  if (x == INFINITY) return INFINITY;
  if (x == -INFINITY) return x - x;
  if (x == 0.0L) return 1.0L / x;
  long double q = fabsl(x);

  if (q > 13.0L) {
    int sign = 1;
    if (q > MAXGAML) return sign * INFINITY;  /* goverf */
    long double z;
    if (x < 0.0L) {
      long double p = floorl(q);
      if (p == q) return (x - x) / (x - x);
      const int i = p;
      if ((i & 1) == 0) sign = -1;
      z = q - p;
      if (z > 0.5L) {
        p += 1.0L;
        z = q - p;
      }
      z = q * sinl(PIL * z);
      z = fabsl(z) * stirf(q);
      if (z <= PIL/LDBL_MAX) return sign * INFINITY;  /* goverf */
      z = PIL / z;
    } else {
      z = stirf(x);
    }
    return sign * z;
  }

  long double z = 1.0L;
  while (x >= 3.0L) {
    x -= 1.0L;
    z *= x;
  }

  while (x < -0.03125L) {
    z /= x;
    x += 1.0L;
  }

  if (x <= 0.03125L) {  /* small */
    if (x == 0.0L) {
      return (x - x) / (x - x);
    } else {
      if (x < 0.0L) {
        x = -x;
        q = z / (x * __polevll(x, SN.begin(), SN.end()));
      } else {
        q = z / (x * __polevll(x, S.begin(), S.end()));
      }
    }
    return q;
  }

  while (x < 2.0L) {
    z /= x;
    x += 1.0L;
  }

  if (x == 2.0L) return z;

  x -= 2.0L;
  const long double p = __polevll(x, P.begin(), P.end());
  q = __polevll(x, Q.begin(), Q.end());
  z = z * p / q;
  return z;
}
