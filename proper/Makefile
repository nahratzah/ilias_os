.PHONY: all

OBJS_LOADER += 32.kgzo
OBJS_LOADER += 64.kgzo
all: kernel

# Import lists of sources.
include abi/Makefile.inc
include abi/Makefile.openlibm
include cyptr/Makefile.inc
include loader/Makefile.inc
include loader_exported/Makefile.inc
include arch/Makefile.inc
include pmap/Makefile.inc
include vm/Makefile.inc
include sys/Makefile.inc

include contrib/Makefile.ilias_async
#include "c_util/Makefile.inc"

# Import build rules.
include Makefile.rules

32.kernel: ${OBJS_32} i386.ld
	${LD} -z max-page-size=0x1000 -T i386.ld ${LDFLAGS} -o $@ ${OBJS_32}
64.kernel: ${OBJS_64} amd64.ld
	${LD} -z max-page-size=0x1000 -T amd64.ld ${LDFLAGS} -o $@ ${OBJS_64}
kernel: ${OBJS_LOADER} loader.ld
	${LD} -z max-page-size=0x1000 -o $@ -T loader.ld ${OBJS_LOADER}

32.kgz: 32.kernel
64.kgz: 64.kernel
32.kgzo: 32.kgz
64.kgzo: 64.kgz

CLEAN_FILES += 32.kgz 64.kgz 32.kernel 64.kernel kernel
