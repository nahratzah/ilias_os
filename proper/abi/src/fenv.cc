#define _FENV_INLINE  /* Compile here */
#include <fenv.h>

_namespace_begin(std)


#if defined(__amd64__) || defined(__x86_64__)
#define __INITIAL_FPUCW__      0x037f
#define __INITIAL_FPUCW_I386__ 0x037f
#define __INITIAL_NPXCW__      __INITIAL_FPUCW_I386__
#define __INITIAL_MXCSR__      0x1f80
#define __INITIAL_MXCSR_MASK__ 0xffbf

const fenv_t __fe_dfl_env = {
  { 0xffff0000 | __INITIAL_FPUCW__,
    0xffff0000,
    0xffffffff,
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff }
  },
  __INITIAL_MXCSR__
};
#endif

#if defined(__i386__)
#define __INITIAL_NPXCW__ 0x127f
#define __INITIAL_MXCSR__ 0x1f80

const fenv_t __fe_dfl_env = {
  __INITIAL_NPXCW__,
  0x0000,
  0x0000,
  0x1f80,
  0xffffffff,
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff }
};
#endif


#if defined(__i386__) && !defined(__SSE__)
__sse_support __has_sse_flag = __SSE_UNK;

__sse_support __test_sse() noexcept {
  uint32_t d;
  asm("cpuid"
  :   "=d"(d)
  :   "a"(1), "c"(0)
  :   "eax", "ebx", "ecx");
  __has_sse_flag = ((d & 0x02000000U) != 0U ? __SSE_YES : __SSE_NO);
  return __has_sse_flag;
}
#endif


_namespace_end(std)
