.PHONY: all llvm clang libcxx

BUILDDIR = ${.CURDIR}/build
LLVM_ALL_FILES != find llvm -type f | grep -v ' '
CLANG_ALL_FILES != find clang -type f | grep -v ' '

INSTALL_PREFIX = "${BUILDDIR}/install"
CMAKE ?= cmake
RM ?= rm

LLVM_OPTS += -DLLVM_ENABLE_CXX11:BOOL=ON
LLVM_OPTS += -DLLVM_ENABLE_LIBCXX:BOOL=ON

CC ?= cc
CXX ?= c++

all: llvm clang

clean:
	${RM} -rf "${BUILDDIR}"

${BUILDDIR}:
	[ -d "${BUILDDIR}" ] || mkdir "${BUILDDIR}"

${BUILDDIR}/llvm: ${BUILDDIR}
	[ -d "${BUILDDIR}/llvm" ] || mkdir "${BUILDDIR}/llvm"

${BUILDDIR}/clang: ${BUILDDIR}
	[ -d "${BUILDDIR}/clang" ] || mkdir "${BUILDDIR}/clang"

${BUILDDIR}/libcxx: ${BUILDDIR}
	[ -d "${BUILDDIR}/libcxx" ] || mkdir "${BUILDDIR}/libcxx"


${BUILDDIR}/llvm.done_configure: ${BUILDDIR}/llvm ${BUILDDIR}/libcxx.done_install ${LLVM_ALL_FILES}
	cd "${BUILDDIR}/llvm" &&                                              \
	  env CC="${CC}" CXX="${CXX}"                                         \
	    ${CMAKE} "-DCMAKE_INSTALL_PREFIX=${INSTALL_PREFIX}" ${LLVM_OPTS} ${.CURDIR}/llvm
	touch "${BUILDDIR}/llvm.done_configure"

${BUILDDIR}/llvm.done_build: ${BUILDDIR}/llvm.done_configure
	cd "${BUILDDIR}/llvm" && ${MAKE}
	touch "${BUILDDIR}/llvm.done_build"

${BUILDDIR}/llvm.done_install: ${BUILDDIR}/llvm.done_build
	cd "${BUILDDIR}/llvm" && ${MAKE} install
	touch "${BUILDDIR}/llvm.done_install"


${BUILDDIR}/clang.done_configure: ${BUILDDIR}/clang ${BUILDDIR}/llvm.done_install ${CLANG_ALL_FILES}
	cd "${BUILDDIR}/clang" &&                                             \
	  env CC="${CC}" CXX="${CXX}"                                         \
	      "LLVM_CONFIG=${BUILDDIR}/llvm/bin/llvm-config"                  \
	    ${CMAKE} "-DCMAKE_INSTALL_PREFIX=${INSTALL_PREFIX}" ${.CURDIR}/clang
	touch "${BUILDDIR}/clang.done_configure"

${BUILDDIR}/clang.done_build: ${BUILDDIR}/clang.done_configure
	cd "${BUILDDIR}/clang" && ${MAKE}
	touch "${BUILDDIR}/clang.done_build"

${BUILDDIR}/clang.done_install: ${BUILDDIR}/clang.done_build
	cd "${BUILDDIR}/clang" && ${MAKE} install
	touch "${BUILDDIR}/clang.done_install"


${BUILDDIR}/libcxx.done_configure: ${BUILDDIR}/libcxx ${LIBCXX_ALL_FILES}
	cd "${BUILDDIR}/libcxx" &&                                            \
	  env CC="${CC}" CXX="${CXX}"                                         \
	    ${CMAKE} "-DCMAKE_INSTALL_PREFIX=${INSTALL_PREFIX}" ${.CURDIR}/libcxx
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
