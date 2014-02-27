TEST += abi/test/dynamic_cast.cc
TEST += abi/test/abi_ext/reader.cc
TEST += abi/test/cstring/memcmp.cc
TEST += abi/test/cstring/memset.cc
TEST += abi/test/cstring/strlen.cc
TEST += abi/test/cstring/memcpy.cc
TEST += abi/test/cstring/memchr.cc
TEST += abi/test/cstring/memrchr.cc
TEST += abi/test/cstring/memccpy.cc
TEST += abi/test/cstring/memmove.cc
TEST += abi/test/cstring/stpcpy.cc

abi/test/dynamic_cast.test: abi/src/abi_dynamic_cast.o_test abi/src/typeinfo.o_test abi/src/abi_typeinfo.o_test abi/test/dynamic_cast.o_test
abi/test/abi_ext/reader.test: abi/test/abi_ext/reader.o_test
abi/test/cstring/memcmp.test: abi/src/cstring.o_test abi/src/abi_errno.cc abi/test/cstring/memcmp.o_test
abi/test/cstring/memset.test: abi/src/cstring.o_test abi/src/abi_errno.cc abi/test/cstring/memset.o_test
abi/test/cstring/strlen.test: abi/src/cstring.o_test abi/src/abi_errno.cc abi/test/cstring/strlen.o_test
abi/test/cstring/memcpy.test: abi/src/cstring.o_test abi/src/abi_errno.cc abi/test/cstring/memcpy.o_test
abi/test/cstring/memchr.test: abi/src/cstring.o_test abi/src/abi_errno.cc abi/test/cstring/memchr.o_test
abi/test/cstring/memrchr.test: abi/src/cstring.o_test abi/src/abi_errno.cc abi/test/cstring/memrchr.o_test
abi/test/cstring/memccpy.test: abi/src/cstring.o_test abi/src/abi_errno.cc abi/test/cstring/memccpy.o_test
abi/test/cstring/memmove.test: abi/src/cstring.o_test abi/src/abi_errno.cc abi/test/cstring/memmove.o_test
abi/test/cstring/stpcpy.test: abi/src/cstring.o_test abi/src/abi_errno.cc abi/test/cstring/stpcpy.o_test
