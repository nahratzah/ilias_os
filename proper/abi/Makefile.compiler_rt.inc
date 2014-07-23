# Scan all compiler_rt sources we care about.
COMPILER_RT_SRCS += $(wildcard abi/compiler_rt/lib/builtins/*.c)
COMPILER_RT_SRCS_32 += $(wildcard abi/compiler_rt/lib/builtins/i386/*.[cS])
COMPILER_RT_SRCS_64 += $(wildcard abi/compiler_rt/lib/builtins/x86_64/*.[cS])

# Exclude specific files.
COMPILER_RT_BLACKLIST += abi/compiler_rt/lib/builtins/apple_versioning.c
COMPILER_RT_BLACKLIST += abi/compiler_rt/lib/builtins/enable_execute_stack.c
COMPILER_RT_BLACKLIST += abi/compiler_rt/lib/builtins/eprintf.c
COMPILER_RT_BLACKLIST += abi/compiler_rt/lib/builtins/atomic.c

# Wildcard exclude floating point builtins (since we don't have FP support yet).
COMPILER_RT_BLACKLIST += $(wildcard abi/compiler_rt/lib/builtins/float*)
COMPILER_RT_BLACKLIST += $(wildcard abi/compiler_rt/lib/builtins/i386/float*)
COMPILER_RT_BLACKLIST += $(wildcard abi/compiler_rt/lib/builtins/x86_64/float*)

# Exclude specific floating point functions (not caught by wildcard match above).
COMPILER_RT_BLACKLIST += abi/compiler_rt/lib/builtins/muldc3.c
COMPILER_RT_BLACKLIST += abi/compiler_rt/lib/builtins/mulsc3.c
COMPILER_RT_BLACKLIST += abi/compiler_rt/lib/builtins/mulxc3.c
COMPILER_RT_BLACKLIST += abi/compiler_rt/lib/builtins/divdc3.c
COMPILER_RT_BLACKLIST += abi/compiler_rt/lib/builtins/divsc3.c
COMPILER_RT_BLACKLIST += abi/compiler_rt/lib/builtins/divxc3.c

# Exclude gcc personality (needs _Unwind implementation to be finished).
COMPILER_RT_BLACKLIST += abi/compiler_rt/lib/builtins/gcc_personality_v0.c

# Build sources list from scanned includes, excluding the blacklist.
SRCS += $(filter-out ${COMPILER_RT_BLACKLIST}, ${COMPILER_RT_SRCS})
SRCS_32 += $(filter-out ${COMPILER_RT_BLACKLIST}, ${COMPILER_RT_SRCS_32})
SRCS_64 += $(filter-out ${COMPILER_RT_BLACKLIST}, ${COMPILER_RT_SRCS_64})
SRCS_LOADER += $(filter-out ${COMPILER_RT_BLACKLIST}, ${COMPILER_RT_SRCS_32})

# Allow int_endiannes.h to succeed, by supplying non-detected macros.
COMPILER_FLAGS += -D_YUGA_LITTLE_ENDIAN=__LITTLE_ENDIAN__
COMPILER_FLAGS += -D_YUGA_BIG_ENDIAN=__BIG_ENDIAN__
