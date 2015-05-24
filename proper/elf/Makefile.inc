INCLUDES += -Ielf/include
ELF_SRCS += elf/src/elf.cc

SRCS += ${ELF_SRCS}
SRCS_LOADER += ${ELF_SRCS}
