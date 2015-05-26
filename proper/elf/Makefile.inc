INCLUDES += -Ielf/include
ELF_SRCS += elf/src/elf_error.cc
ELF_SRCS += elf/src/elf_header.cc
ELF_SRCS += elf/src/elf_section.cc
ELF_SRCS += elf/src/elf_progheader.cc

SRCS += ${ELF_SRCS}
SRCS_LOADER += ${ELF_SRCS}
