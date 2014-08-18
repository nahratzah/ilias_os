INCLUDES_LOADER += -Iloader/include

SRCS_LOADER += loader/src/loader.s
SRCS_LOADER += loader/src/amd64.cc
SRCS_LOADER += loader/src/loader_kernel.cc
SRCS_LOADER += loader/src/loader_support.cc
SRCS_LOADER += loader/src/loader_x86_video.cc
SRCS_LOADER += loader/src/loader_abi.cc
SRCS_LOADER += loader/src/loader_panic.cc
SRCS_LOADER += loader/src/loader_multiboot.cc
SRCS_LOADER += loader/src/ldexport_init.cc
SRCS_LOADER += loader/src/loader_page.cc
SRCS_LOADER += loader/src/main.cc
