INCLUDES += -Iarch/include
ARCH_SRCS += arch/src/cpuid.cc
ARCH_SRCS += arch/src/arch_i386_gdt.cc
ARCH_SRCS_I386 += arch/src/arch_i386_paging.cc
ARCH_SRCS_I386 += arch/src/arch_i386_sse.cc

SRCS += ${ARCH_SRCS}
SRCS_32 += ${ARCH_SRCS_I386}
SRCS_LOADER += ${ARCH_SRCS}
SRCS_LOADER += ${ARCH_SRCS_I386}
