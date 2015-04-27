#ifndef _CMATH_INL_H_
#define _CMATH_INL_H_

#include <cmath>

_namespace_begin(std)


#ifdef __cplusplus

inline float acos(float x) noexcept {
  return acosf(x);
}
inline long double acos(long double x) noexcept {
  return acosl(x);
}

inline float acosh(float x) noexcept {
  return acoshf(x);
}
inline long double acosh(long double x) noexcept {
  return acoshl(x);
}

inline float asin(float x) noexcept {
  return asinf(x);
}
inline long double asin(long double x) noexcept {
  return asinl(x);
}

inline float asinh(float x) noexcept {
  return asinhf(x);
}
inline long double asinh(long double x) noexcept {
  return asinhl(x);
}

inline float atan(float x) noexcept {
  return atanf(x);
}
inline long double atan(long double x) noexcept {
  return atanl(x);
}

inline float atan2(float x, float y) noexcept {
  return atan2f(x, y);
}
inline long double atan2(long double x, long double y) noexcept {
  return atan2l(x, y);
}

inline float atanh(float x) noexcept {
  return atanhf(x);
}
inline long double atanh(long double x) noexcept {
  return atanhl(x);
}

inline float cbrt(float x) noexcept {
  return cbrtf(x);
}
inline long double cbrt(long double x) noexcept {
  return cbrtl(x);
}

inline float ceil(float x) noexcept {
  return ceilf(x);
}
inline long double ceil(long double x) noexcept {
  return ceill(x);
}

inline float copysign(float x, float y) noexcept {
  return copysignf(x, y);
}
inline long double copysign(long double x, long double y) noexcept {
  return copysignl(x, y);
}

inline float cos(float x) noexcept {
  return cosf(x);
}
inline long double cos(long double x) noexcept {
  return cosl(x);
}

inline float cosh(float x) noexcept {
  return coshf(x);
}
inline long double cosh(long double x) noexcept {
  return coshl(x);
}

inline float erf(float x) noexcept {
  return erff(x);
}
inline long double erf(long double x) noexcept {
  return erfl(x);
}

inline float erfc(float x) noexcept {
  return erfcf(x);
}
inline long double erfc(long double x) noexcept {
  return erfcl(x);
}

inline float exp(float x) noexcept {
  return expf(x);
}
inline long double exp(long double x) noexcept {
  return expl(x);
}

inline float exp2(float x) noexcept {
  return exp2f(x);
}
inline long double exp2(long double x) noexcept {
  return exp2l(x);
}

inline float expm1(float x) noexcept {
  return expm1f(x);
}
inline long double expm1(long double x) noexcept {
  return expm1l(x);
}

inline float fabs(float x) noexcept {
  return fabsf(x);
}
inline long double fabs(long double x) noexcept {
  return fabsl(x);
}

inline float fdim(float x, float y) noexcept {
  return fdimf(x, y);
}
inline long double fdim(long double x, long double y) noexcept {
  return fdiml(x, y);
}

inline float floor(float x) noexcept {
  return floorf(x);
}
inline long double floor(long double x) noexcept {
  return floorl(x);
}

inline float fma(float x, float y, float z) noexcept {
  return fmaf(x, y, z);
}
inline long double fma(long double x, long double y, long double z) noexcept {
  return fmal(x, y, z);
}

inline float fmax(float x, float y) noexcept {
  return fmaxf(x, y);
}
inline long double fmax(long double x, long double y) noexcept {
  return fmaxl(x, y);
}

inline float fmin(float x, float y) noexcept {
  return fminf(x, y);
}
inline long double fmin(long double x, long double y) noexcept {
  return fminl(x, y);
}

inline float fmod(float x, float y) noexcept {
  return fmodf(x, y);
}
inline long double fmod(long double x, long double y) noexcept {
  return fmodl(x, y);
}

inline float frexp(float x, int* y) noexcept {
  return frexpf(x, y);
}
inline long double frexp(long double x, int* y) noexcept {
  return frexpl(x, y);
}

inline float hypot(float x, float y) noexcept {
  return hypotf(x, y);
}
inline long double hypot(long double x, long double y) noexcept {
  return hypotl(x, y);
}

inline int ilogb(float x) noexcept {
  return ilogbf(x);
}
inline int ilogb(long double x) noexcept {
  return ilogbl(x);
}

inline float ldexp(float x, int y) noexcept {
  return ldexpf(x, y);
}
inline long double ldexp(long double x, int y) noexcept {
  return ldexpl(x, y);
}

inline float lgamma(float x) noexcept {
  return lgammaf(x);
}
inline long double lgamma(long double x) noexcept {
  return lgammal(x);
}

inline float lgamma_r(float x, int* signgam) noexcept {
  return lgammaf_r(x, signgam);
}
inline long double lgamma_r(long double x, int* signgam) noexcept {
  return lgammal_r(x, signgam);
}

inline long long llrint(float x) noexcept {
  return llrintf(x);
}
inline long long llrint(long double x) noexcept {
  return llrintl(x);
}

inline long long llround(float x) noexcept {
  return llroundf(x);
}
inline long long llround(long double x) noexcept {
  return llroundl(x);
}

inline float log(float x) noexcept {
  return logf(x);
}
inline long double log(long double x) noexcept {
  return logl(x);
}

inline float log10(float x) noexcept {
  return log10f(x);
}
inline long double log10(long double x) noexcept {
  return log10l(x);
}

inline float log1p(float x) noexcept {
  return log1pf(x);
}
inline long double log1p(long double x) noexcept {
  return log1pl(x);
}

inline float log2(float x) noexcept {
  return log2f(x);
}
inline long double log2(long double x) noexcept {
  return log2l(x);
}

inline float logb(float x) noexcept {
  return logbf(x);
}
inline long double logb(long double x) noexcept {
  return logbl(x);
}

inline long lrint(float x) noexcept {
  return lrintf(x);
}
inline long lrint(long double x) noexcept {
  return lrintl(x);
}

inline long lround(float x) noexcept {
  return lroundf(x);
}
inline long lround(long double x) noexcept {
  return lroundl(x);
}

inline float modf(float x, float* y) noexcept {
  return modff(x, y);
}
inline long double modf(long double x, long double* y) noexcept {
  return modfl(x, y);
}

inline float nearbyint(float x) noexcept {
  return nearbyintf(x);
}
inline long double nearbyint(long double x) noexcept {
  return nearbyintl(x);
}

inline float nextafter(float x, float y) noexcept {
  return nextafterf(x, y);
}
inline long double nextafter(long double x, long double y) noexcept {
  return nextafterl(x, y);
}

inline float nexttoward(float x, float y) noexcept {
  return nexttowardf(x, y);
}
inline long double nexttoward(long double x, long double y) noexcept {
  return nexttowardl(x, y);
}

inline float pow(float x, float y) noexcept {
  return powf(x, y);
}
inline long double pow(long double x, long double y) noexcept {
  return powl(x, y);
}

inline float remainder(float x, float y) noexcept {
  return remainderf(x, y);
}
inline long double remainder(long double x, long double y) noexcept {
  return remainderl(x, y);
}

inline float remquo(float x, float y, int* z) noexcept {
  return remquof(x, y, z);
}
inline long double remquo(long double x, long double y, int* z) noexcept {
  return remquol(x, y, z);
}

inline float rint(float x) noexcept {
  return rintf(x);
}
inline long double rint(long double x) noexcept {
  return rintl(x);
}

inline float round(float x) noexcept {
  return roundf(x);
}
inline long double round(long double x) noexcept {
  return roundl(x);
}

inline float scalbln(float x, long y) noexcept {
  return scalblnf(x, y);
}
inline long double scalbln(long double x, long y) noexcept {
  return scalblnl(x, y);
}

inline float scalbn(float x, int y) noexcept {
  return scalbnf(x, y);
}
inline long double scalbn(long double x, int y) noexcept {
  return scalbnl(x, y);
}

inline float sin(float x) noexcept {
  return sinf(x);
}
inline long double sin(long double x) noexcept {
  return sinl(x);
}

inline void sincos(float x, float* s, float* c) noexcept {
  sincosf(x, s, c);
}
inline void sincos(long double x, long double* s, long double* c) noexcept {
  sincosl(x, s, c);
}

inline float sinh(float x) noexcept {
  return sinhf(x);
}
inline long double sinh(long double x) noexcept {
  return sinhl(x);
}

inline float sqrt(float x) noexcept {
  return sqrtf(x);
}
inline long double sqrt(long double x) noexcept {
  return sqrtl(x);
}

inline float tan(float x) noexcept {
  return tanf(x);
}
inline long double tan(long double x) noexcept {
  return tanl(x);
}

inline float tanh(float x) noexcept {
  return tanhf(x);
}
inline long double tanh(long double x) noexcept {
  return tanhl(x);
}

inline float tgamma(float x) noexcept {
  return tgammaf(x);
}
inline long double tgamma(long double x) noexcept {
  return tgammal(x);
}

inline float trunc(float x) noexcept {
  return truncf(x);
}
inline long double trunc(long double x) noexcept {
  return truncl(x);
}

inline double abs(double x) noexcept {
  return fabs(x);
}
inline float abs(float x) noexcept {
  return fabs(x);
}
inline long double abs(long double x) noexcept {
  return fabs(x);
}

#endif // __cplusplus

inline float nanf(const char* s) noexcept {
  return __builtin_nanf(s);
}
inline double nan(const char* s) noexcept {
  return __builtin_nan(s);
}
inline long double nanl(const char* s) noexcept {
  return __builtin_nanl(s);
}


_namespace_end(std)

#endif /* _CMATH_INL_H_ */
