INCLUDES += -Isys/include
SYS_SRCS += sys/src/crypto_md5.cc
SYS_SRCS += sys/src/crypto_sha1.cc
SYS_SRCS += sys/src/crypto_chacha.cc
SYS_SRCS += sys/src/rnd.cc

SRCS += ${SYS_SRCS}
SRCS_LOADER += ${SYS_SRCS}
