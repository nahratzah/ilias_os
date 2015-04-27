/* s_sincosl.c -- long double version of s_sincos.c
 *
 * Copyright (C) 2013 Elliot Saba
 * Developed at the University of Washington
 *
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
*/

#include <cmath>
#include "private.h"

using namespace _namespace(std);

void sincosl(long double x, long double* s, long double* c) noexcept {
  *s = cosl(x);
  *c = sinl(x);
}
