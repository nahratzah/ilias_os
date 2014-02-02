INCLUDES += -Iabi/include
SRCS +=	abi/src/abi_assert.cc
SRCS +=	abi/src/abi_demangle.cc
SRCS +=	abi/src/abi_dynamic_cast.cc
SRCS +=	abi/src/abi_eh.cc
SRCS += abi/src/abi_errno.cc
SRCS += abi/src/abi_ext_string_piece.cc
SRCS += abi/src/abi_ext_c_string_ptr.cc
SRCS += abi/src/abi_ext_printf.cc
SRCS += abi/src/abi_hashcode.cc
SRCS += abi/src/abi_memory.cc
SRCS +=	abi/src/abi_ref_cstr.cc
SRCS +=	abi/src/abi_semaphore.cc
SRCS +=	abi/src/abi_typeinfo.cc
SRCS += abi/src/cstdlib.cc
SRCS += abi/src/cstdio.cc
SRCS += abi/src/cstring.cc
SRCS += abi/src/cwchar.cc
SRCS += abi/src/stdexcept.cc
SRCS += abi/src/new.cc
SRCS +=	abi/src/typeinfo.cc

SRCS +=	abi/src/exception.cc

.include "test/Makefile.inc"
