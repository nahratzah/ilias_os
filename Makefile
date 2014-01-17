.PHONY: all

OBJS_LOADER += 32.kgzo 64.kgzo
all: kernel

# Import lists of sources.
.include "abi/Makefile.inc"
.include "freestanding/Makefile.inc"
.include "c_util/Makefile.inc"
.include "sys/Makefile.inc"
.include "contrib/freestanding/libcxx/Makefile.inc"

# Import build rules.
.include "Makefile.inc"

32.kernel: ${OBJS_32} i386.ld
	${LD} -T i386.ld ${LDFLAGS} -o $@ ${OBJS_32}
64.kernel: ${OBJS_64} amd64.ld
	${LD} -T amd64.ld ${LDFLAGS} -o $@ ${OBJS_64}
kernel: ${OBJS_LOADER} loader.ld
	ld -o $@ -T loader.ld ${OBJS_LOADER}

32.kgz: 32.kernel
64.kgz: 64.kernel
32.kgzo: 32.kgz
64.kgzo: 64.kgz

CLEAN_FILES += 32.kgz 64.kgz 32.kernel 64.kernel kernel
