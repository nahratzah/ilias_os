INCLUDES += -Iarch/include
ARCH_SRCS += arch/src/cpuid.cc

SRCS += ${ARCH_SRCS}
SRCS_LOADER += ${ARCH_SRCS}
