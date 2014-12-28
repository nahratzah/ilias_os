#ifndef _ILIAS_DWARF_REGISTERS_H_
#define _ILIAS_DWARF_REGISTERS_H_

#include <cdecl.h>

_namespace_begin(ilias)
namespace dwarf {


#if defined(__amd64__) || defined(__x86_64__)
enum class dwarf_reg : unsigned int {
  rax            =  0,
  rdx            =  1,
  rcx            =  2,
  rbx            =  3,
  rsi            =  4,
  rdi            =  5,
  rbp            =  6,
  rsp            =  7,
  r8             =  8,
  r9             =  9,
  r10            = 10,
  r11            = 11,
  r12            = 12,
  r13            = 13,
  r14            = 14,
  r15            = 15,
  return_address = 16,
  xmm0           = 17,
  xmm1           = 18,
  xmm2           = 19,
  xmm3           = 20,
  xmm4           = 21,
  xmm5           = 22,
  xmm6           = 23,
  xmm7           = 24,
  xmm8           = 25,
  xmm9           = 26,
  xmm10          = 27,
  xmm11          = 28,
  xmm12          = 29,
  xmm13          = 30,
  xmm14          = 31,
  xmm15          = 32,
  st0            = 33,
  st1            = 34,
  st2            = 35,
  st3            = 36,
  st4            = 37,
  st5            = 38,
  st6            = 39,
  st7            = 40,
  mm0            = 41,
  mm1            = 42,
  mm2            = 43,
  mm3            = 44,
  mm4            = 45,
  mm5            = 46,
  mm6            = 47,
  mm7            = 48,
  rflags         = 49,
  es             = 50,
  cs             = 51,
  ss             = 52,
  ds             = 53,
  fs             = 54,
  gs             = 55,
  /* Reserved: 56, 57. */
  fs_base        = 58,
  gs_base        = 59,
  /* Reserved: 60, 61. */
  tr             = 62,
  ldtr           = 63,
  mxcsr          = 64,
  fcw            = 65,
  fsw            = 66,
};

constexpr unsigned int dwarf_reg_count = 67;

enum class pointer_encoding_spec : unsigned char {
  usleb128   = 0x01,  // Values stored as uleb128/sleb128
  usdata2    = 0x02,  // Values stored as 2 byte integers
  usdata4    = 0x03,  // Values stored as 4 byte integers
  usdata8    = 0x04,  // Values stored as 8 byte integers
  signedval  = 0x08,  // Values are signed
  pcrel      = 0x10,  // Values are PC relative
  textrel    = 0x20,  // Values are text section relative
  datarel    = 0x30,  // Values are data section relative
  funcrel    = 0x40,  // Values are relative to start of function
};
#endif

#if defined(__i386__)
enum class dwarf_reg : unsigned int {
  // XXX write me
};

constexpr unsigned int dwarf_reg_count = 0;  // XXX actual value
#endif

extern const char*const dwarf_reg_name[dwarf_reg_count];


} /* namespace ilias::dwarf */
_namespace_end(ilias)

#endif /* _ILIAS_DWARF_REGISTERS_H_ */
