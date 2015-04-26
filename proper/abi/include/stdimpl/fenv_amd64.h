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
 * $FreeBSD: src/lib/msun/amd64/fenv.h,v 1.8 2011/10/10 15:43:09 das Exp $
 */

#ifndef	_STDIMPL_FENV_AMD64_H_
#define	_STDIMPL_FENV_AMD64_H_

#ifndef _FENV_INLINE
# define _FENV_INLINE extern inline
#endif

#include <cdecl.h>
#include <cstdint>
#include "fp_intel_asm.h"

_namespace_begin(std)
_cdecl_begin

struct __fenv_t__x87 {
  uint32_t __control;
  uint32_t __status;
  uint32_t __tag;
  uint8_t __other[16];
};
struct fenv_t {
  struct __fenv_t__x87 __x87;
  uint32_t __mxcsr;
};
#ifndef __cplusplus
typedef struct __fenv_t__x87 __fenv_t__x87;
typedef struct fenv_t fenv_t;
#endif

typedef	uint16_t fexcept_t;

/* Exception flags */
#define	FE_INVALID	0x01
#define	FE_DENORMAL	0x02
#define	FE_DIVBYZERO	0x04
#define	FE_OVERFLOW	0x08
#define	FE_UNDERFLOW	0x10
#define	FE_INEXACT	0x20
#define	FE_ALL_EXCEPT	(FE_DIVBYZERO | FE_DENORMAL | FE_INEXACT | \
			 FE_INVALID | FE_OVERFLOW | FE_UNDERFLOW)

/* Rounding modes */
#define	FE_TONEAREST	0x0000
#define	FE_DOWNWARD	0x0400
#define	FE_UPWARD	0x0800
#define	FE_TOWARDZERO	0x0c00
#define	_ROUND_MASK	(FE_TONEAREST | FE_DOWNWARD | \
			 FE_UPWARD | FE_TOWARDZERO)

/*
 * As compared to the x87 control word, the SSE unit's control word
 * has the rounding control bits offset by 3 and the exception mask
 * bits offset by 7.
 */
#define	_SSE_ROUND_SHIFT	3
#define	_SSE_EMASK_SHIFT	7

/* Default floating-point environment */
extern const fenv_t __fe_dfl_env;
#define	FE_DFL_ENV	(&__fe_dfl_env)

__attribute__((always_inline)) inline void __fldenv(const __fenv_t__x87& env)
    noexcept {
  asm volatile("fldenv %0" : : "m"(env));
}
__attribute__((always_inline)) inline void __fldenvx(const __fenv_t__x87& env)
    noexcept {
  asm volatile("fldenv %0"
  :
  :            "m"(env)
  :            "st", "st(1)", "st(2)", "st(3)", "st(4)",
               "st(5)", "st(6)", "st(7)");
}
__attribute__((always_inline)) inline __fenv_t__x87 __fnstenv() noexcept {
  __fenv_t__x87 env;
  asm volatile("fnstenv %0" : "=m"(env));
  return env;
}

_FENV_INLINE int feclearexcept(int excepts) noexcept {
  fenv_t env;

  if (excepts == FE_ALL_EXCEPT) {
    __fnclex();
  } else {
    env.__x87 = __fnstenv();
    env.__x87.__status &= ~excepts;
    __fldenv(env.__x87);
  }
  env.__mxcsr = __stmxcsr();
  env.__mxcsr &= ~excepts;
  __ldmxcsr(env.__mxcsr);
  return 0;
}

_FENV_INLINE int fegetexceptflag(fexcept_t* flagp, int excepts) noexcept {
  uint32_t mxcsr;
  uint16_t status;

  mxcsr = __stmxcsr();
  status = __fnstsw();
  *flagp = (mxcsr | status) & excepts;
  return 0;
}

_FENV_INLINE int fesetexceptflag(const fexcept_t* flagp, int excepts)
    noexcept {
  fenv_t env;

  env.__x87 = __fnstenv();
  env.__x87.__status &= ~excepts;
  env.__x87.__status |= *flagp & excepts;
  __fldenv(env.__x87);

  env.__mxcsr = __stmxcsr();
  env.__mxcsr &= ~excepts;
  env.__mxcsr |= *flagp & excepts;
  __ldmxcsr(env.__mxcsr);

  return 0;
}

_FENV_INLINE int feraiseexcept(int excepts) noexcept {
  fexcept_t ex = excepts;

  fesetexceptflag(&ex, excepts);
  __fwait();
  return 0;
}

_FENV_INLINE int fetestexcept(int excepts) noexcept {
  const uint32_t mxcsr = __stmxcsr();
  const uint16_t status = __fnstsw();
  return ((status | mxcsr) & excepts);
}

_FENV_INLINE int fegetround() noexcept {
  /*
   * We assume that the x87 and the SSE unit agree on the
   * rounding mode.  Reading the control word on the x87 turns
   * out to be about 5 times faster than reading it on the SSE
   * unit on an Opteron 244.
   */
  const uint16_t control = __fnstcw();
  return (control & _ROUND_MASK);
}

_FENV_INLINE int fesetround(int round) noexcept {
  if (round & ~_ROUND_MASK) return -1;

  uint16_t control = __fnstcw();
  control &= ~_ROUND_MASK;
  control |= round;
  __fldcw(control);

  uint32_t mxcsr = __stmxcsr();
  mxcsr &= ~(_ROUND_MASK << _SSE_ROUND_SHIFT);
  mxcsr |= round << _SSE_ROUND_SHIFT;
  __ldmxcsr(mxcsr);

  return 0;
}

_FENV_INLINE int fegetenv(fenv_t* envp) noexcept {
  envp->__x87 = __fnstenv();
  envp->__mxcsr = __stmxcsr();
  /*
   * fnstenv masks all exceptions, so we need to restore the
   * control word to avoid this side effect.
   */
  __fldcw(envp->__x87.__control);
  return 0;
}

_FENV_INLINE int feholdexcept(fenv_t* envp) noexcept {
  uint32_t mxcsr = __stmxcsr();
  envp->__x87 = __fnstenv();
  __fnclex();
  envp->__mxcsr = mxcsr;
  mxcsr &= ~FE_ALL_EXCEPT;
  mxcsr |= FE_ALL_EXCEPT << _SSE_EMASK_SHIFT;
  __ldmxcsr(mxcsr);
  return 0;
}

_FENV_INLINE int fesetenv(const fenv_t* envp) noexcept {
  /*
   * XXX Using fldenvx() instead of fldenv() tells the compiler that this
   * instruction clobbers the i387 register stack.  This happens because
   * we restore the tag word from the saved environment.  Normally, this
   * would happen anyway and we wouldn't care, because the ABI allows
   * function calls to clobber the i387 regs.  However, fesetenv() is
   * inlined, so we need to be more careful.
   */
  __fldenvx(envp->__x87);
  __ldmxcsr(envp->__mxcsr);
  return 0;
}

_FENV_INLINE int feupdateenv(const fenv_t* envp) noexcept {
  const uint16_t status = __fnstsw();
  uint32_t mxcsr = __stmxcsr();
  fesetenv(envp);
  feraiseexcept((mxcsr | status) & FE_ALL_EXCEPT);
  return 0;
}

_FENV_INLINE int feenableexcept(int mask) noexcept {
  mask &= FE_ALL_EXCEPT;
  uint16_t control = __fnstcw();
  uint32_t mxcsr = __stmxcsr();
  const uint32_t omask =
      ~(control | mxcsr >> _SSE_EMASK_SHIFT) & FE_ALL_EXCEPT;
  control &= ~mask;
  __fldcw(control);
  mxcsr &= ~(mask << _SSE_EMASK_SHIFT);
  __ldmxcsr(mxcsr);
  return omask;
}

_FENV_INLINE int fedisableexcept(int mask) noexcept {
  mask &= FE_ALL_EXCEPT;
  uint16_t control = __fnstcw();
  uint32_t mxcsr = __stmxcsr();
  const uint32_t omask =
      ~(control | mxcsr >> _SSE_EMASK_SHIFT) & FE_ALL_EXCEPT;
  control |= mask;
  __fldcw(control);
  mxcsr |= mask << _SSE_EMASK_SHIFT;
  __ldmxcsr(mxcsr);
  return omask;
}

_FENV_INLINE int fegetexcept() noexcept {
  /*
   * We assume that the masks for the x87 and the SSE unit are
   * the same.
   */
  const uint16_t control = __fnstcw();
  return (~control & FE_ALL_EXCEPT);
}

_cdecl_end
_namespace_end(std)

#endif	/* _STDIMPL_FENV_AMD64_H_ */
