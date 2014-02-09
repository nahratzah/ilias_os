.PHONY: all clean compiler_all

PROPER_OPTS += "CC=${.CURDIR}/compiler/build/install/bin/clang"
PROPER_OPTS += "CXX=${.CURDIR}/compiler/build/install/bin/clang++"

all: compiler_all
	cd proper && ${MAKE} ${PROPER_OPTS} all

compiler_all:
	cd compiler && ${MAKE} all

clean:
	cd compiler && ${MAKE} clean
	cd proper && ${MAKE} ${PROPER_OPTS} clean

cleandir:
	cd compiler && ${MAKE} clean
	cd proper && ${MAKE} ${PROPER_OPTS} cleandir

test: compiler_all
	cd proper && ${MAKE} ${PROPER_OPTS} test

retest: compiler_all
	cd proper && ${MAKE} ${PROPER_OPTS} retest

check: compiler_all
	cd proper && ${MAKE} ${PROPER_OPTS} check

recheck: compiler_all
	cd proper && ${MAKE} ${PROPER_OPTS} recheck
