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
 * $FreeBSD: src/lib/msun/i387/fenv.h,v 1.8 2011/10/10 15:43:09 das Exp $
 */

#ifndef	_STDIMPL_FENV_I386_H_
#define	_STDIMPL_FENV_I386_H_

#ifndef _FENV_INLINE
#define	_FENV_INLINE extern inline
#endif

#include <cdecl.h>
#include <cstdint>

_namespace_begin(std)
_cdecl_begin

/*
 * We pack the mxcsr into some reserved fields.
 */
struct fenv_t {
  uint16_t __control;
  uint16_t __mxcsr_hi;
  uint16_t __status;
  uint16_t __mxcsr_lo;
  uint32_t __tag;
  uint8_t __other[16];
};
#ifndef __cplusplus
typedef struct fenv_t fenv_t;
#endif

typedef	uint16_t	fexcept_t;

__attribute__((always_inline)) inline void __set_mxcsr(fenv_t* env,
                                                       uint32_t mxcsr)
    noexcept {
  env->__mxcsr_hi = mxcsr >> 16;
  env->__mxcsr_lo = mxcsr;
}

__attribute__((always_inline)) inline uint32_t __get_mxcsr(const fenv_t* env)
    noexcept {
  uint32_t mxcsr = env->__mxcsr_hi;
  mxcsr <<= 16;
  mxcsr |= env->__mxcsr_lo;
  return mxcsr;
}

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

/* After testing for SSE support once, we cache the result in __has_sse. */
#ifdef __SSE__
__attribute__((always_inline)) inline int __has_sse() noexcept {
  return 1;
}
#else
enum __sse_support { __SSE_UNK, __SSE_YES, __SSE_NO };
extern enum __sse_support __has_sse_flag;

__sse_support __test_sse() noexcept;

__attribute__((always_inline)) inline int __has_sse() noexcept {
  return (_predict_false(__has_sse_flag == __SSE_UNK) ?
          __test_sse() :
          __has_sse_flag) == __SSE_YES;
}
#endif

/* Default floating-point environment */
extern const fenv_t	__fe_dfl_env;
#define	FE_DFL_ENV	(&__fe_dfl_env)

__attribute__((always_inline)) inline void __fldcw(uint16_t cw) noexcept {
  asm volatile("fldcw %0" : : "m"(cw));
}
__attribute__((always_inline)) inline uint16_t __fnstcw() noexcept {
  uint16_t cw;
  asm volatile("fnstcw %0" : "=m"(cw));
  return cw;
}

__attribute__((always_inline)) inline void __fnclex() noexcept {
  asm volatile("fnclex");
}
__attribute__((always_inline)) inline void __fwait() noexcept {
  asm volatile("fwait");
}

__attribute__((always_inline)) inline void __fldenv(fenv_t env)
    noexcept {
  asm volatile("fldenv %0" : : "m"(env));
}
__attribute__((always_inline)) inline void __fldenvx(fenv_t env)
    noexcept {
  asm volatile("fldenv %0"
  :
  :            "m"(env)
  :            "st", "st(1)", "st(2)", "st(3)", "st(4)",
               "st(5)", "st(6)", "st(7)");
}
__attribute__((always_inline)) inline fenv_t __fnstenv() noexcept {
  fenv_t env;
  asm volatile("fnstenv %0" : "=m"(env));
  return env;
}

__attribute__((always_inline)) inline uint16_t __fnstsw() noexcept {
  uint16_t sw;
  asm volatile("fnstsw %0" : "=am"(sw));
  return sw;
}

__attribute__((always_inline)) inline void __ldmxcsr(uint32_t csr) noexcept {
  asm volatile("ldmxcsr %0" : : "m"(csr));
}
__attribute__((always_inline)) inline uint32_t __stmxcsr() noexcept {
  uint32_t csr;
  asm volatile("stmxcsr %0" : "=m"(csr));
  return csr;
}

_FENV_INLINE int feclearexcept(int excepts) noexcept {
  fenv_t env;
  uint32_t mxcsr;

  if (excepts == FE_ALL_EXCEPT) {
    __fnclex();
  } else {
    env = __fnstenv();
    env.__status &= ~excepts;
    __fldenv(env);
  }
  if (__has_sse()) {
    mxcsr = __stmxcsr();
    mxcsr &= ~excepts;
    __ldmxcsr(mxcsr);
  }
  return 0;
}

_FENV_INLINE int fegetexceptflag(fexcept_t* flagp, int excepts) noexcept {
  const uint16_t status = __fnstsw();
  const uint32_t mxcsr = (__has_sse() ? __stmxcsr() : 0U);
  *flagp = (mxcsr | status) & excepts;
  return 0;
}

_FENV_INLINE int fesetexceptflag(const fexcept_t* flagp, int excepts)
    noexcept {
  fenv_t env = __fnstenv();
  env.__status &= ~excepts;
  env.__status |= *flagp & excepts;
  __fldenv(env);

  if (__has_sse()) {
    uint32_t mxcsr = __stmxcsr();
    mxcsr &= ~excepts;
    mxcsr |= *flagp & excepts;
    __ldmxcsr(mxcsr);
  }

  return 0;
}

_FENV_INLINE int feraiseexcept(int excepts) noexcept {
  fexcept_t ex = excepts;

  fesetexceptflag(&ex, excepts);
  __fwait();
  return 0;
}

_FENV_INLINE int fetestexcept(int excepts) noexcept {
  const uint16_t status = __fnstsw();
  const uint32_t mxcsr = (__has_sse() ? __stmxcsr() : 0U);
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

  if (__has_sse()) {
    uint32_t mxcsr = __stmxcsr();
    mxcsr &= ~(_ROUND_MASK << _SSE_ROUND_SHIFT);
    mxcsr |= round << _SSE_ROUND_SHIFT;
    __ldmxcsr(mxcsr);
  }

  return 0;
}

_FENV_INLINE int fegetenv(fenv_t* envp) noexcept {
  *envp = __fnstenv();
  /*
   * fnstenv masks all exceptions, so we need to restore
   * the old control word to avoid this side effect.
   */
  __fldcw(envp->__control);
  if (__has_sse()) __set_mxcsr(envp, __stmxcsr());
  return 0;
}

_FENV_INLINE int feholdexcept(fenv_t* envp) noexcept {
  *envp = __fnstenv();
  __fnclex();
  if (__has_sse()) {
    uint32_t mxcsr = __stmxcsr();
    __set_mxcsr(envp, mxcsr);
    mxcsr &= ~FE_ALL_EXCEPT;
    mxcsr |= FE_ALL_EXCEPT << _SSE_EMASK_SHIFT;
    __ldmxcsr(mxcsr);
  }
  return 0;
}

_FENV_INLINE int fesetenv(const fenv_t *envp) noexcept {
  fenv_t env = *envp;
  uint32_t mxcsr = __get_mxcsr(&env);
  __set_mxcsr(&env, 0xffffffffU);
  /*
   * XXX Using fldenvx() instead of fldenv() tells the compiler that this
   * instruction clobbers the i387 register stack.  This happens because
   * we restore the tag word from the saved environment.  Normally, this
   * would happen anyway and we wouldn't care, because the ABI allows
   * function calls to clobber the i387 regs.  However, fesetenv() is
   * inlined, so we need to be more careful.
   */
  __fldenvx(env);
  if (__has_sse()) __ldmxcsr(mxcsr);
  return (0);
}

_FENV_INLINE int feupdateenv(const fenv_t* envp) noexcept {
  uint16_t status = __fnstsw();
  uint32_t mxcsr = (__has_sse() ? __stmxcsr() : 0U);
  fesetenv(envp);
  feraiseexcept((mxcsr | status) & FE_ALL_EXCEPT);
  return 0;
}

_FENV_INLINE int feenableexcept(int mask) noexcept {
  mask &= FE_ALL_EXCEPT;
  uint16_t control = __fnstcw();
  uint32_t mxcsr = (__has_sse() ? __stmxcsr() : 0U);
  const uint32_t omask =
      ~(control | mxcsr >> _SSE_EMASK_SHIFT) & FE_ALL_EXCEPT;
  control &= ~mask;
  __fldcw(control);
  if (__has_sse()) {
    mxcsr &= ~(mask << _SSE_EMASK_SHIFT);
    __ldmxcsr(mxcsr);
  }
  return omask;
}

_FENV_INLINE int fedisableexcept(int mask) noexcept {
  mask &= FE_ALL_EXCEPT;
  uint16_t control = __fnstcw();
  uint32_t mxcsr = (__has_sse() ? __stmxcsr() : 0U);
  const uint32_t omask =
      ~(control | mxcsr >> _SSE_EMASK_SHIFT) & FE_ALL_EXCEPT;
  control |= mask;
  __fldcw(control);
  if (__has_sse()) {
    mxcsr |= mask << _SSE_EMASK_SHIFT;
    __ldmxcsr(mxcsr);
  }
  return omask;
}

/* We currently provide no external definition of fegetexcept(). */
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

#endif	/* !_FENV_H_ */
