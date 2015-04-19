/*-
 * Copyright (c) 2008 David Schultz <das@FreeBSD.ORG>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD: src/lib/msun/ld80/invtrig.h,v 1.2 2008/08/02 03:56:22 das Exp $
 */
#ifndef _LD80_INVTRIG_H_
#define _LD80_INVTRIG_H_

#include <cfloat>
#include <cmath>
#include <array>
#include "../private.h"

namespace {


constexpr auto BIAS = LDBL_MAX_EXP - 1;
constexpr auto MANH_SIZE = LDBL_MANH_SIZE;

/* Approximation thresholds. */
constexpr auto ASIN_LINEAR = (BIAS - 32);  /* 2**-32 */
constexpr auto ACOS_CONST  = (BIAS - 65);  /* 2**-65 */
constexpr auto ATAN_CONST  = (BIAS + 65);  /* 2**65 */
constexpr auto ATAN_LINEAR = (BIAS - 32);  /* 2**-32 */

/* 0.95 */
constexpr auto THRESH = (0xe666666666666666ULL >> (64 - (MANH_SIZE - 1))) |
                        0x80000000U /* LDBL_NBIT */;

#if 0
/* Constants shared by the long double inverse trig functions. */
#define	pS0	_ItL_pS0
#define	pS1	_ItL_pS1
#define	pS2	_ItL_pS2
#define	pS3	_ItL_pS3
#define	pS4	_ItL_pS4
#define	pS5	_ItL_pS5
#define	pS6	_ItL_pS6
#define	qS1	_ItL_qS1
#define	qS2	_ItL_qS2
#define	qS3	_ItL_qS3
#define	qS4	_ItL_qS4
#define	qS5	_ItL_qS5
#define	atanhi	_ItL_atanhi
#define	atanlo	_ItL_atanlo
#define	aT	_ItL_aT
#define	pi_lo	_ItL_pi_lo
#endif

constexpr long double pS0 =  1.66666666666666666631e-01L;
constexpr long double pS1 = -4.16313987993683104320e-01L;
constexpr long double pS2 =  3.69068046323246813704e-01L;
constexpr long double pS3 = -1.36213932016738603108e-01L;
constexpr long double pS4 =  1.78324189708471965733e-02L;
constexpr long double pS5 = -2.19216428382605211588e-04L;
constexpr long double pS6 = -7.10526623669075243183e-06L;
constexpr long double qS1 = -2.94788392796209867269e+00L;
constexpr long double qS2 =  3.27309890266528636716e+00L;
constexpr long double qS3 = -1.68285799854822427013e+00L;
constexpr long double qS4 =  3.90699412641738801874e-01L;
constexpr long double qS5 = -3.14365703596053263322e-02L;

constexpr _namespace(std)::array<long double, 4> atanhi{{
  4.63647609000806116202e-01L,
  7.85398163397448309628e-01L,
  9.82793723247329067960e-01L,
  1.57079632679489661926e+00L
}};

constexpr _namespace(std)::array<long double, 4> atanlo{{
   1.18469937025062860669e-20L,
  -1.25413940316708300586e-20L,
   2.55232234165405176172e-20L,
  -2.50827880633416601173e-20L,
}};

constexpr _namespace(std)::array<long double, 13> aT{{
   3.33333333333333333017e-01L,
  -1.99999999999999632011e-01L,
   1.42857142857046531280e-01L,
  -1.11111111100562372733e-01L,
   9.09090902935647302252e-02L,
  -7.69230552476207730353e-02L,
   6.66661718042406260546e-02L,
  -5.88158892835030888692e-02L,
   5.25499891539726639379e-02L,
  -4.70119845393155721494e-02L,
   4.03539201366454414072e-02L,
  -2.91303858419364158725e-02L,
   1.24822046299269234080e-02L
}};

constexpr long double pi_lo = -5.01655761266833202345e-20L;

inline long double P(long double x) noexcept {
  return (x * (pS0 + x * (pS1 + x * (pS2 + x * (pS3 + x *
          (pS4 + x * (pS5 + x * pS6)))))));
}

inline long double Q(long double x) noexcept {
  return (1.0 + x * (qS1 + x * (qS2 + x * (qS3 + x * (qS4 + x * qS5)))));
}

inline long double T_even(long double x) noexcept {
  return (aT[0] + x * (aT[2] + x * (aT[4] + x * (aT[6] + x *
          (aT[8] + x * (aT[10] + x * aT[12]))))));
}

inline long double T_odd(long double x) noexcept {
  return (aT[1] + x * (aT[3] + x * (aT[5] + x * (aT[7] + x *
          (aT[9] + x * aT[11])))));
}

constexpr auto pio2_hi = atanhi[3];
constexpr auto pio2_lo = atanlo[3];
constexpr auto pio4_hi = atanhi[1];


} /* namespace <unnamed> */

#endif /* _LD80_INVTRIG_H_ */
