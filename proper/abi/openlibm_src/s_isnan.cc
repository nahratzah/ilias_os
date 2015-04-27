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
 * $FreeBSD: src/lib/msun/src/s_isnan.c,v 1.9 2010/06/12 17:32:05 das Exp $
 */

#include <cmath>
#include "private.h"

#undef isnan

_namespace_begin(std)


int __isnan(double d) noexcept {
  const IEEE_d2bits u = IEEE_d2bits_from_dbl(d);
  return (u.exp == 2047 && (u.manl != 0 || u.manh != 0));
}

int __isnanf(float f) noexcept {
  const IEEE_f2bits u = IEEE_f2bits_from_flt(f);
  return (u.exp == 255 && u.man != 0);
}

int __isnanl(long double e) noexcept {
  IEEE_l2bits u = IEEE_l2bits_from_ldbl(e);
  mask_nbit_l(u);
  return (u.exp == 32767 && (u.manl != 0 || u.manh != 0));
}

extern "C" int isnan(double d) noexcept {
  return __isnan(d);
}


_namespace_end(std)
