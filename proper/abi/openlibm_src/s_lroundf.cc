/*-
 * Copyright (c) 2005 David Schultz <das@FreeBSD.ORG>
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
 */

#include <cmath>
#include <climits>
#include <fenv.h>
#include "private.h"

namespace {

/*
 * If float has more precision than long, the endpoints long(min|max) are
 * of the form xxx.5; they are "out of range" because lround() rounds away
 * from 0.  On the other hand, if float has less precision than long, then
 * all values that are out of range are integral, so we might as well assume
 * that everything is in range.  At compile time, INRANGE(x) should reduce to
 * two floating-point comparisons in the former case, or TRUE otherwise.
 */
constexpr bool inrange(float x) noexcept {
  constexpr float min = LONG_MIN - 0.5;
  constexpr float max = LONG_MAX + 0.5;

  return max - LONG_MAX != 0.5 ||
         (x > min && x < max);
}

} /* namespace <unnamed> */

using namespace _namespace(std);

long lroundf(float x) noexcept {
  if (inrange(x)) {
    x = round(x);
    return static_cast<long>(x);
  } else {
    feraiseexcept(FE_INVALID);
    return LONG_MAX;
  }
}
