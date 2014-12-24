INCLUDES += -Icyptr/include
CYPTR_SRCS += cyptr/src/ilias_cyptr_impl_tstamp.cc
CYPTR_SRCS += cyptr/src/ilias_cyptr_impl_generation.cc
CYPTR_SRCS += cyptr/src/ilias_cyptr_impl_basic_obj.cc
CYPTR_SRCS += cyptr/src/ilias_cyptr_impl_edge.cc
CYPTR_SRCS += cyptr/src/ilias_cyptr_impl_background.cc
CYPTR_SRCS += cyptr/src/ilias_cyptr_background.cc

SRCS += ${CYPTR_SRCS}
# SRCS_LOADER += ${CYPTR_SRCS}
