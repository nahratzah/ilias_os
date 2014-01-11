HDRS +=	include/util/c_string_ptr-inl.h					\
	include/util/c_string_ptr.h					\
	include/util/string_piece-inl.h					\
	include/util/string_piece.h					\
	include/util/cpp/function.h					\
	include/util/cpp/function-inl.h					\
	include/util/cpp/identifier.h					\
	include/util/cpp/name-inl.h					\
	include/util/cpp/name.h						\
	include/util/cpp/namespace-inl.h				\
	include/util/cpp/namespace.h					\
	include/util/cpp/template-inl.h					\
	include/util/cpp/template.h

SRCS +=	src/c_string_ptr.cc						\
	src/string_piece.cc						\
	src/cpp_function.cc						\
	src/cpp_identifier.cc						\
	src/cpp_name.cc							\
	src/cpp_namespace.cc						\
	src/cpp_template.cc

CXXOPTS += -Iinclude -std=c++11

LIB = util

.include <bsd.lib.mk>
