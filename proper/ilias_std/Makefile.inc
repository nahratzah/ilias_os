INCLUDES += -Iilias_std/include
ILIAS_STD_SRCS += ilias_std/src/ilias_hazard.cc
ILIAS_STD_SRCS += ilias_std/src/ilias_linked_list.cc

SRCS += ${ILIAS_STD_SRCS}
SRCS_LOADER += ${ILIAS_STD_SRCS}