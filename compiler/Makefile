.PHONY: all llvm clang libcxx

BUILDDIR = build

INSTALL_PREFIX = "${BUILDDIR}/install"
CMAKE ?= cmake
RM ?= rm

LLVM_OPTS ?= -DCMAKE_BUILD_TYPE=Release
CLANG_OPTS ?= -DCMAKE_BUILD_TYPE=Release
LLVM_OPTS += -DLLVM_ENABLE_CXX11:BOOL=OFF
LLVM_OPTS += -DLLVM_ENABLE_LIBCXX:BOOL=OFF
LLVM_OPTS += -DLLVM_ENABLE_PIC:BOOL=OFF
LLVM_OPTS += -DLLVM_FORCE_USE_OLD_TOOLCHAIN:BOOL=ON
LLVM_OPTS += -DLLVM_EXTERNAL_CLANG_SOURCE_DIR:PATH=../../clang
LLVM_OPTS += -DLLVM_EXTERNAL_LLD_SOURCE_DIR:PATH=../../lld

CC ?= cc
CXX ?= c++
CFLAGS ?= -I/usr/local/include
CXXFLAGS ?= -I/usr/local/include
LDFLAGS ?= -L/usr/local/lib

all: llvm binutils

clean:
	${RM} -rf "${BUILDDIR}"

${BUILDDIR}/llvm.done_dir:
	[ -d "${BUILDDIR}/llvm" ] || mkdir -p "${BUILDDIR}/llvm"
	touch ${BUILDDIR}/llvm.done_dir

${BUILDDIR}/clang.done_dir:
	[ -d "${BUILDDIR}/clang" ] || mkdir -p "${BUILDDIR}/clang"
	touch ${BUILDDIR}/clang.done_dir

${BUILDDIR}/libcxx.done_dir:
	[ -d "${BUILDDIR}/libcxx" ] || mkdir -p "${BUILDDIR}/libcxx"
	touch ${BUILDDIR}/libcxx.done_dir

${BUILDDIR}/binutils.done_dir:
	[ -d "${BUILDDIR}/binutils" ] || mkdir -p "${BUILDDIR}/binutils"
	touch ${BUILDDIR}/binutils.done_dir


${BUILDDIR}/binutils.done_configure: ${BUILDDIR}/binutils.done_dir
	cd "${BUILDDIR}/binutils" &&                                          \
	  env CC="${CC}" CXX="${CXX}"                                         \
	      CFLAGS="${CFLAGS}" CXXFLAGS="${CXXFLAGS}" LDFLAGS="${LDFLAGS}"  \
	    ../../binutils/configure --prefix="$(abspath ${INSTALL_PREFIX})" --disable-werror --enable-ld=yes --with-gnu-ld --disable-gdb --disable-nls --target=amd64-unknown-linux
	touch "${BUILDDIR}/binutils.done_configure"

${BUILDDIR}/binutils.done_build: ${BUILDDIR}/binutils.done_configure
	cd "${BUILDDIR}/binutils" && ${MAKE} -j1
	touch "${BUILDDIR}/binutils.done_build"

${BUILDDIR}/binutils.done_install: ${BUILDDIR}/binutils.done_build
	cd "${BUILDDIR}/binutils" && ${MAKE} -j1 install
	touch "${BUILDDIR}/binutils.done_install"


${BUILDDIR}/llvm.done_configure: ${BUILDDIR}/llvm.done_dir # ${BUILDDIR}/libcxx.done_install
	cd "${BUILDDIR}/llvm" &&                                              \
	  env CC="${CC}" CXX="${CXX}"                                         \
	      CFLAGS="${CFLAGS}" CXXFLAGS="${CXXFLAGS}" LDFLAGS="${LDFLAGS}"  \
	    ${CMAKE} "-DCMAKE_INSTALL_PREFIX=../../${INSTALL_PREFIX}" ${LLVM_OPTS} ../../llvm
	touch "${BUILDDIR}/llvm.done_configure"

${BUILDDIR}/llvm.done_build: ${BUILDDIR}/llvm.done_configure
	cd "${BUILDDIR}/llvm" && ${MAKE}
	touch "${BUILDDIR}/llvm.done_build"

${BUILDDIR}/llvm.done_install: ${BUILDDIR}/llvm.done_build
	cd "${BUILDDIR}/llvm" && ${MAKE} install
	touch "${BUILDDIR}/llvm.done_install"


${BUILDDIR}/clang.done_configure: ${BUILDDIR}/clang.done_dir ${BUILDDIR}/llvm.done_install
	cd "${BUILDDIR}/clang" &&                                             \
	  env CC="${CC}" CXX="${CXX}"                                         \
	      CFLAGS="${CFLAGS}" CXXFLAGS="${CXXFLAGS}" LDFLAGS="${LDFLAGS}"  \
	    ${CMAKE}                                                          \
	      "-DLLVM_CONFIG=../llvm/bin/llvm-config"                         \
	      "-DCMAKE_INSTALL_PREFIX=../../${INSTALL_PREFIX}" ${CLANG_OPTS} ../../clang
	touch "${BUILDDIR}/clang.done_configure"

${BUILDDIR}/clang.done_build: ${BUILDDIR}/clang.done_configure
	cd "${BUILDDIR}/clang" && ${MAKE}
	touch "${BUILDDIR}/clang.done_build"

${BUILDDIR}/clang.done_install: ${BUILDDIR}/clang.done_build
	cd "${BUILDDIR}/clang" && ${MAKE} install
	touch "${BUILDDIR}/clang.done_install"


${BUILDDIR}/libcxx.done_configure: ${BUILDDIR}/libcxx.done_dir ${LIBCXX_ALL_FILES}
	cd "${BUILDDIR}/libcxx" &&                                            \
	  env CC="${CC}" CXX="${CXX}"                                         \
	      CFLAGS="${CFLAGS}" CXXFLAGS="${CXXFLAGS}" LDFLAGS="${LDFLAGS}"  \
	    ${CMAKE} "-DCMAKE_INSTALL_PREFIX=../../${INSTALL_PREFIX}" ../../libcxx
	touch "${BUILDDIR}/libcxx.done_configure"

${BUILDDIR}/libcxx.done_build: ${BUILDDIR}/libcxx.done_configure
	cd "${BUILDDIR}/libcxx" && ${MAKE}
	touch "${BUILDDIR}/libcxx.done_build"

${BUILDDIR}/libcxx.done_install: ${BUILDDIR}/libcxx.done_build
	cd "${BUILDDIR}/libcxx" && ${MAKE} install
	touch "${BUILDDIR}/libcxx.done_install"


libcxx: ${BUILDDIR}/libcxx.done_install
llvm: ${BUILDDIR}/llvm.done_install
clang: ${BUILDDIR}/clang.done_install
binutils: ${BUILDDIR}/binutils.done_install
