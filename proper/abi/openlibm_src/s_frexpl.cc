/*-
 * Copyright (c) 2004-2005 David Schultz <das@FreeBSD.ORG>
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
 * $FreeBSD: src/lib/msun/src/s_frexpl.c,v 1.1 2005/03/07 04:54:51 das Exp $
 */

#include <cmath>
#include <cfloat>
#include "private.h"

using namespace _namespace(std);

static_assert(LDBL_MAX_EXP == 0x4000, "Unsupported long double format");

long double frexpl(long double x, int *ex) noexcept {
  IEEE_l2bits u = IEEE_l2bits_from_ldbl(x);

  switch (u.exp) {
  default:  /* normal */
    *ex = u.exp - 0x3ffe;
    u.exp = 0x3ffe;
    break;
  case 0:  /* 0 or subnormal */
    if ((u.manl | u.manh) == 0) {
      *ex = 0;
    } else {
      u = IEEE_l2bits_from_ldbl(x * 0x1.0p514);
      *ex = u.exp - 0x4200;
      u.exp = 0x3ffe;
    }
    break;
  case 0x7fff:  /* infinity or NaN; value of *ex is unspecified */
    break;
  }
  return ldbl_from_IEEE_l2bits(u);
}
