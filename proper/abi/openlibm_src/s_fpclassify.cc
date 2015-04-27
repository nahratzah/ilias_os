/*-
 * Copyright (c) 2004 David Schultz <das@FreeBSD.ORG>
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
 */

#include <cmath>
#include "private.h"

_namespace_begin(std)


int __fpclassify(double d) noexcept {
  const IEEE_d2bits u = IEEE_d2bits_from_dbl(d);

  if (u.exp == 2047) {
    if (u.manl == 0 && u.manh == 0)
      return FP_INFINITE;
    else
      return FP_NAN;
  } else if (u.exp != 0) {
    return FP_NORMAL;
  } else if (u.manl == 0 && u.manh == 0) {
    return FP_ZERO;
  } else {
    return FP_SUBNORMAL;
  }
}

int __fpclassifyf(float f) noexcept {
  const IEEE_f2bits u = IEEE_f2bits_from_flt(f);

  if (u.exp == 255) {
    if (u.man == 0)
      return FP_INFINITE;
    else
      return FP_NAN;
  } else if (u.exp != 0) {
    return FP_NORMAL;
  } else if (u.man == 0) {
    return FP_ZERO;
  } else {
    return FP_SUBNORMAL;
  }
}

int __fpclassifyl(long double e) noexcept {
  IEEE_l2bits u = IEEE_l2bits_from_ldbl(e);
  mask_nbit_l(u);

  if (u.exp == 32767) {
    if (u.manl == 0 && u.manh == 0)
      return FP_INFINITE;
    else
      return FP_NAN;
  } else if (u.exp != 0) {
    return FP_NORMAL;
  } else if (u.manl == 0 && u.manh == 0) {
    return FP_ZERO;
  } else {
    return FP_SUBNORMAL;
  }
}


_namespace_end(std)
