INCLUDES_LOADER += -Iloader_exported/include
INCLUDES += -Iloader_exported/include

LOADER_EXPORTED_SRCS += loader_exported/src/memorymap.cc
LOADER_EXPORTED_SRCS += loader_exported/src/ldexport.cc

SRCS += ${LOADER_EXPORTED_SRCS}
SRCS_LOADER += ${LOADER_EXPORTED_SRCS}
