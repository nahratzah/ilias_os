#include <ilias/dwarf/registers.h>

_namespace_begin(ilias)
namespace dwarf {


#if defined(__amd64__) || defined(__x86_64__)
const char*const dwarf_reg_name[dwarf_reg_count] = {
  "rax",
  "rdx",
  "rcx",
  "rbx",
  "rsi",
  "rdi",
  "rbp",
  "rsp",
  "r8",
  "r9",
  "r10",
  "r11",
  "r12",
  "r13",
  "r14",
  "r15",
  "<return_address>",  // Not really a register name.
  "xmm0",
  "xmm1",
  "xmm2",
  "xmm3",
  "xmm4",
  "xmm5",
  "xmm6",
  "xmm7",
  "xmm8",
  "xmm9",
  "xmm10",
  "xmm11",
  "xmm12",
  "xmm13",
  "xmm14",
  "xmm15",
  "st0",
  "st1",
  "st2",
  "st3",
  "st4",
  "st5",
  "st6",
  "st7",
  "mm0",
  "mm1",
  "mm2",
  "mm3",
  "mm4",
  "mm5",
  "mm6",
  "mm7",
  "rflags",
  "es",
  "cs",
  "ss",
  "ds",
  "fs",
  "gs",
  nullptr,  // Reserved.
  nullptr,  // Reserved.
  "fs_base",
  "gs_base",
  nullptr,  // Reserved.
  nullptr,  // Reserved.
  "tr",
  "ldtr",
  "mxcsr",
  "fcw",
  "fsw",
};
#endif

#if defined(__i386__)
const char*const dwarf_reg_name[dwarf_reg_count] = {
};
#endif


} /* namespace ilias::dwarf */
_namespace_end(ilias)
