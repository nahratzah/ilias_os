/*-
 * Copyright (c) 2003 Peter Wemm.
 * Copyright (c) 1990 Andrew Moore, Talke Studio
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
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *      from: @(#) ieeefp.h     1.0 (Berkeley) 9/23/93
 * $FreeBSD: src/sys/amd64/include/ieeefp.h,v 1.14 2005/04/12 23:12:00 jhb Exp $
 */
#ifndef _STDIMPL_IEEEFP_AMD64_H_
#define _STDIMPL_IEEEFP_AMD64_H_

#include <cdecl.h>
#include "fp_intel_asm.h"

#ifndef _IEEEFP_INLINE
#define _IEEEFP_INLINE extern inline
#endif

_namespace_begin(std)
_cdecl_begin


/*
 * FP rounding modes
 */
enum __fp_rnd_t {
  FP_RN = 0,  /* round to nearest */
  FP_RM,  /* round down to -inf */
  FP_RP,  /* round up to +inf */
  FP_RZ  /* truncate */
};
typedef enum __fp_rnd_t fp_rnd_t;

/*
 * FP precision modes
 */
enum __fp_prec_t {
  FP_PS = 0,  /* 24 bit (single precision) */
  FP_PRS,  /* reserved */
  FP_PD,  /* 53 bit (double precision) */
  FP_PE,  /* 64 bit (extended precision) */
};
typedef enum __fp_prec_t fp_prec_t;

typedef int fp_except_t;

/*
 * FP exception masks
 */
#define FP_X_INV   0x01  /* invalid operation */
#define FP_X_DNML  0x02  /* denormal */
#define FP_X_DZ    0x04  /* zero divide */
#define FP_X_OFL   0x08  /* overflow */
#define FP_X_UFL   0x10  /* underflow */
#define FP_X_IMP   0x20  /* (im)precision */
#define FP_X_STK   0x40  /* stack fault */

/*
 * FP registers
 */
#define FP_MSKS_REG  0  /* exception masks */
#define FP_PRC_REG   0  /* precision */
#define FP_RND_REG   0  /* direction */
#define FP_STKY_REG  1  /* sticky flags */

/*
 * FP register bit field masks
 */
#define FP_MSKS_FLD  0x003f  /* exception masks field */
#define FP_PRC_FLD   0x0300  /* precision control field */
#define FP_RND_FLD   0x0c00  /* round control field */
#define FP_STKY_FLD  0x003f  /* sticky flags field */

/*
 * SSE mxcsr register bit field masks
 */
#define SSE_STKY_FLD  0x003f  /* exception flags */
#define SSE_DAZ_FLD   0x0040  /* denormals are zero */
#define SSE_MSKS_FLD  0x1f80  /* exception masks field */
#define SSE_RND_FLD   0x6000  /* rounding control */
#define SSE_FZ_FLD    0x8000  /* flush to zero on overflow */

/*
 * FP register bit field offsets
 */
#define FP_MSKS_OFF   0  /* exception masks offset */
#define FP_PRC_OFF    8  /* precision control offset */
#define FP_RND_OFF   10  /* round control offset */
#define FP_STKY_OFF   0  /* sticky flags offset */

/*
 * SSE mxcsr register bit field offsets
 */
#define SSE_STKY_OFF   0  /* exception flags offset */
#define SSE_DAZ_OFF    6  /* DAZ exception mask offset */
#define SSE_MSKS_OFF   7  /* other exception masks offset */
#define SSE_RND_OFF   13  /* rounding control offset */
#define SSE_FZ_OFF    15  /* flush to zero offset */

/*
 * Get/set rounding control
 */
_IEEEFP_INLINE fp_rnd_t fpgetround() noexcept {
  return (fp_rnd_t)((__fnstcw() & FP_RND_FLD) >> FP_RND_OFF);
}

_IEEEFP_INLINE fp_rnd_t fpsetround(fp_rnd_t m) noexcept {
  uint16_t cw = __fnstcw();
  fp_rnd_t p = (fp_rnd_t)((cw & FP_RND_FLD) >> FP_RND_OFF);
  cw &= ~FP_RND_FLD;
  cw |= (m << FP_RND_OFF) & FP_RND_FLD;
  __fldcw(cw);
  uint32_t mxcsr = __stmxcsr();
  mxcsr &= ~SSE_RND_FLD;
  mxcsr |= (m << SSE_RND_OFF) & SSE_RND_FLD;
  __ldmxcsr(mxcsr);
  return p;
}

/*
 * Get/set precision for fadd/fsub/fsqrt etc x87 instructions.
 * There is no equivalent SSE mode or control.
 */
_IEEEFP_INLINE fp_prec_t fpgetprec() noexcept {
  return (fp_prec_t)((__fnstcw() & FP_PRC_FLD) >> FP_PRC_OFF);
}

_IEEEFP_INLINE fp_prec_t fpsetprec(fp_prec_t m) noexcept {
  uint16_t cw = __fnstcw();
  fp_prec_t p = (fp_prec_t)((cw & FP_PRC_FLD) >> FP_PRC_OFF);
  cw &= ~FP_PRC_FLD;
  cw |= (m << FP_PRC_OFF) & FP_PRC_FLD;
  __fldcw(cw);
  return p;
}

/*
 * Look at the exception masks.
 * Note that x87 masks are the inverse of the fp*() functions API.
 * ie: mask = 1 means disable for x87 and SSE, but
 * for the fp*() api, mask = 1 means enabled.
 */
_IEEEFP_INLINE fp_except_t fpgetmask() noexcept {
  return ~__fnstcw() & FP_MSKS_FLD;
}

_IEEEFP_INLINE fp_except_t fpsetmask(fp_except_t m) noexcept {
  uint16_t cw = __fnstcw();
  fp_except_t p = (fp_except_t)(~cw & FP_MSKS_FLD);
  cw &= ~FP_MSKS_FLD;
  cw |= ~m & FP_MSKS_FLD;
  __fldcw(cw);
  uint32_t mxcsr = __stmxcsr();
  /* XXX should we clear non-ieee SSE_DAZ_FLD and SSE_FZ_FLD ? */
  mxcsr &= ~SSE_MSKS_FLD;
  mxcsr |= (~m << SSE_MSKS_OFF) & SSE_MSKS_FLD;
  __ldmxcsr(mxcsr);
  return p;
}

/* See which sticky exceptions are pending and reset them. */
_IEEEFP_INLINE fp_except_t fpgetsticky() noexcept {
  uint16_t sw = __fnstsw();
  fp_except_t ex = sw & FP_STKY_FLD;
  uint32_t mxcsr = __stmxcsr();
  ex |= mxcsr & SSE_STKY_FLD;
  return ex;
}


_cdecl_end
_namespace_end(std)

#endif /* _STDIMPL_IEEEFP_AMD64_H_ */
