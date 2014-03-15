INCLUDES += -Iabi/include
ABI_SRCS += abi/src/abi_assert.cc
ABI_SRCS += abi/src/abi_demangle.cc
ABI_SRCS += abi/src/abi_dynamic_cast.cc
ABI_SRCS += abi/src/abi_eh.cc
ABI_SRCS += abi/src/abi_errno.cc
ABI_SRCS += abi/src/abi_ext_bigint.cc
ABI_SRCS += abi/src/abi_ext_c_string_ptr.cc
ABI_SRCS += abi/src/abi_ext_heap.cc
ABI_SRCS += abi/src/abi_ext_printf.cc
ABI_SRCS += abi/src/abi_ext_reader.cc
ABI_SRCS += abi/src/abi_hashcode.cc
ABI_SRCS += abi/src/abi_linker.cc
ABI_SRCS += abi/src/abi_memory.cc
ABI_SRCS += abi/src/abi_ref_cstr.cc
ABI_SRCS += abi/src/abi_semaphore.cc
ABI_SRCS += abi/src/abi_typeinfo.cc
ABI_SRCS += abi/src/atomic.cc
ABI_SRCS += abi/src/cstdlib.cc
ABI_SRCS += abi/src/cstdio.cc
ABI_SRCS += abi/src/cstring.cc
ABI_SRCS += abi/src/cwchar.cc
ABI_SRCS += abi/src/exception.cc
ABI_SRCS += abi/src/ios.cc
ABI_SRCS += abi/src/stdexcept.cc
ABI_SRCS += abi/src/new.cc
ABI_SRCS += abi/src/string.cc
ABI_SRCS += abi/src/streambuf.cc
ABI_SRCS += abi/src/system_error.cc
ABI_SRCS += abi/src/typeinfo.cc
ABI_SRCS += abi/src/vector.cc

SRCS_32 += abi/i386/divdi3.s
SRCS_32 += abi/i386/moddi3.s
SRCS_32 += abi/i386/udivdi3.s
SRCS_32 += abi/i386/umoddi3.s

SRCS += ${ABI_SRCS}
SRCS_LOADER += ${ABI_SRCS}

SRCS_LOADER += abi/i386/divdi3.s
SRCS_LOADER += abi/i386/moddi3.s
SRCS_LOADER += abi/i386/udivdi3.s
SRCS_LOADER += abi/i386/umoddi3.s

include abi/test/Makefile.inc
