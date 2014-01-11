COMPILER_FLAGS +=	-ffreestanding					\
			-nostdinc					\
			-fno-stack-protector				\
			-integrated-as					\
			-O3						\
			-D_KERNEL=1					\
			-Wno-tautological-compare

CFLAGS_32 +=		-target i386
CFLAGS_64 +=		-target amd64					\
			-mcmodel=large					\
			-mno-red-zone

CXXFLAGS +=		${COMPILER_FLAGS}				\
			-std=c++11
CFLAGS +=		${COMPILER_FLAGS}
LDFLAGS +=		-nostdlib					\
			-nodefaultlibs

AS ?= as
CC ?= clang
CXX ?= clang++
LD ?= ld
GZIP ?= gzip
OBJCOPY ?= objcopy
RM ?= rm

.SUFFIXES: .o32 .o64 .ol .cc .cpp .c .s .S .kernel .kgz .kgzo
.PHONY: loader clean cleandir

OBJS_32 += ${SRCS:N*.h:R:S/$/.o32/}
OBJS_64 += ${SRCS:N*.h:R:S/$/.o64/}
OBJS_32 += ${SRCS_32:N*.h:R:S/$/.o32/}
OBJS_64 += ${SRCS_64:N*.h:R:S/$/.o64/}
OBJS_LOADER += ${SRCS_LOADER:N*.h:R:S/$/.ol/}

clean:
	${RM} -f ${OBJS_32} ${OBJS_64} ${OBJS_LOADER} ${CLEAN_FILES}

.ifnmake clean
. for o in ${OBJS_32} ${OBJS_64} ${OBJS_LOADER}
.  if exists(${o}.d)
.   include "${o}.d"
.  elif exists($o)
    .PHONY: $o
.  endif
. endfor
.endif

DEP_FILES = ${OBJS_32:S/$/.d/} ${OBJS_64:S/$/.d/} ${OBJS_LOADER:S/$/.d/}

cleandir: clean
	${RM} -f ${DEP_FILES}


# Build rules for compilation of loader objects.
.cc.ol:
	${CXX} ${CFLAGS_32} ${CFLAGS_LOADER} ${CXXFLAGS} ${INCLUDES} ${INCLUDES_LOADER} -MD -MP -MF $@.d -o $@ -c $<
.cpp.ol:
	${CXX} ${CFLAGS_32} ${CFLAGS_LOADER} ${CXXFLAGS} ${INCLUDES} ${INCLUDES_LOADER} -MD -MP -MF $@.d -o $@ -c $<
.c.ol:
	${CC} ${CFLAGS_32} ${CFLAGS_LOADER} ${CFLAGS} ${INCLUDES} ${INCLUDES_LOADER} -MD -MP -MF $@.d -o $@ -c $<
.s.ol:
	${CC} ${CFLAGS_32} ${CFLAGS_LOADER} ${CFLAGS} ${INCLUDES} ${INCLUDES_LOADER} -o $@ -c $<
.S.ol:
	${CC} ${CFLAGS_32} ${CFLAGS_LOADER} ${CFLAGS} ${INCLUDES} ${INCLUDES_LOADER} -MD -MP -MF $@.d -o $@ -c $<

# Rules for compiling 32/64-bit objects (the actual kernel).
.cc.o32:
	${CXX} ${CFLAGS_32} ${CXXFLAGS} ${INCLUDES} -MD -MP -MF $@.d -o $@ -c $<
.cc.o64:
	${CXX} ${CFLAGS_64} ${CXXFLAGS} ${INCLUDES} -MD -MP -MF $@.d -o $@ -c $<
.cpp.o32:
	${CXX} ${CFLAGS_32} ${CXXFLAGS} ${INCLUDES} -MD -MP -MF $@.d -o $@ -c $<
.cpp.o64:
	${CXX} ${CFLAGS_64} ${CXXFLAGS} ${INCLUDES} -MD -MP -MF $@.d -o $@ -c $<
.c.o32:
	${CC} ${CFLAGS_32} ${CFLAGS} ${INCLUDES} -MD -MP -MF $@.d -o $@ -c $<
.c.o64:
	${CC} ${CFLAGS_64} ${CFLAGS} ${INCLUDES} -MD -MP -MF $@.d -o $@ -c $<
.s.o32:
	${CC} ${CFLAGS_32} ${CFLAGS} ${INCLUDES} -o $@ -c $<
.s.o64:
	${CC} ${CFLAGS_64} ${CFLAGS} ${INCLUDES} -o $@ -c $<
.S.o32:
	${CC} ${CFLAGS_32} ${CFLAGS} ${INCLUDES} -MD -MP -MF $@.d -o $@ -c $<
.S.o64:
	${CC} ${CFLAGS_64} ${CFLAGS} ${INCLUDES} -MD -MP -MF $@.d -o $@ -c $<
.kernel.kgz:
	${GZIP} -9 -c $< > $@
.kgz.kgzo:
	${OBJCOPY} --input-target binary --output-target elf32-i386 --binary-architecture i386 $< $@
