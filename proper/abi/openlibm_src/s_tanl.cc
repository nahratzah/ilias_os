/*-
 * Copyright (c) 2007 Steven G. Kargl
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice unmodified, this list of conditions, and the following
 *    disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//__FBSDID("$FreeBSD: src/lib/msun/src/s_tanl.c,v 1.3 2011/05/30 19:41:28 kargl Exp $");

/*
 * Limited testing on pseudorandom numbers drawn within [0:4e8] shows
 * an accuracy of <= 1.5 ULP where 247024 values of x out of 40 million
 * possibles resulted in tan(x) that exceeded 0.5 ULP (ie., 0.6%).
 */

#include <cmath>
#include <cfloat>
#include <array>
#include "private.h"

using namespace _namespace(std);

long double tanl(long double x) noexcept {
  using _namespace(std)::impl::__kernel_tanl;
  using _namespace(std)::impl::__ieee754_rem_pio2l;

  IEEE_l2bits z = IEEE_l2bits_from_ldbl(x);
  const int s = z.sign;
  z.sign = 0;

  /* If x = +-0 or x is subnormal, then tan(x) = x. */
  if (z.exp == 0)
    return x;

  /* If x = NaN or Inf, then tan(x) = NaN. */
  if (z.exp == 32767)
    return (x - x) / (x - x);

  /* Optimize the case where x is already within range. */
  if (ldbl_from_IEEE_l2bits(z) < M_PI_4) {
    const long double hi = __kernel_tanl(ldbl_from_IEEE_l2bits(z), 0, 0);
    return (s ? -hi : hi);
  }

  array<long double, 2> y;
  const auto e0 = __ieee754_rem_pio2l(x, y.data());
  long double hi = y[0];
  const long double lo = y[1];

  switch (e0 & 3) {
  case 0:
  case 2:
    hi = __kernel_tanl(hi, lo, 0);
    break;
  case 1:
  case 3:
     hi = __kernel_tanl(hi, lo, 1);
     break;
  }

  return hi;
}