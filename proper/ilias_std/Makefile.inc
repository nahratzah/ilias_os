INCLUDES += -Iilias_std/include
ILIAS_STD_SRCS += ilias_std/src/ilias_linked_list.cc
ILIAS_STD_SRCS += ilias_std/src/ilias_linked_set.cc
ILIAS_STD_SRCS += ilias_std/src/ilias_stats.cc

SRCS += ${ILIAS_STD_SRCS}
SRCS_LOADER += ${ILIAS_STD_SRCS}
