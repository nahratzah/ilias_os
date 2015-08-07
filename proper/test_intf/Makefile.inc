TEST_INCLUDES += -Itest_intf/include
TEST_SRCS += test_intf/src/test_registry.cc
TEST_SRCS += test_intf/src/test_case.cc

INCLUDES += ${TEST_INCLUDES}
SRCS += ${TEST_SRCS}
SRCS_LOADER += ${TEST_SRCS}
