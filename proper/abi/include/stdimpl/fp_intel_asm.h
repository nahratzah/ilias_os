#ifndef _STDIMPL_FP_INTEL_ASM_H_
#define _STDIMPL_FP_INTEL_ASM_H_

#include <cdecl.h>
#include <cstdint>

_namespace_begin(std)
_cdecl_begin


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
  asm volatile("stmxcsr %0" : "=m" (csr));
  return csr;
}

#if !defined(__i386__) && !defined(__amd64__) && !defined(__x86_64__)
# error "This is intel specific code."
#endif


_cdecl_end
_namespace_end(std)

#endif /* _STDIMPL_FP_INTEL_ASM_H_ */
