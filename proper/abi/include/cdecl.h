#ifndef CDECL_H
#define CDECL_H

#if !defined(__cplusplus)
# define _namespace_begin(X)	/* C does not have namespaces */
# define _namespace_end(X)	/* C does not have namespaces */
# define _namespace(X)		/* C does not have namespaces */
#elif defined(_TEST)
# define _namespace_begin(X)	namespace test_##X {
# define _namespace_end(X)	}
# define _namespace(X)		test_##X
#else
# define _namespace_begin(X)	namespace X {
# define _namespace_end(X)	}
# define _namespace(X)		X
#endif

#if defined(__cplusplus)
# define _cdecl_begin	extern "C" {
# define _cdecl_end	} /* extern "C" */
#else
# define _cdecl_begin	/* empty */
# define _cdecl_end	/* empty */
#endif

#ifdef __cplusplus
# define _predict_true(x)	__builtin_expect(bool(x), true)
# define _predict_false(x)	__builtin_expect(bool(x), false)
#else
# define _predict_true(x)	__builtin_expect(((x) != 0), 1)
# define _predict_false(x)	__builtin_expect(((x) != 0), 0)
#endif

#ifndef _LIBC_EXPORT
# define _LIBC_EXPORT		/* External function requires no linkage spec. */
#endif /* _LIBC_EXPORT */

#ifndef __cplusplus
# define noexcept		/* C is always noexcept. */
#endif

#ifndef __cplusplus
# define __restrict		restrict
#endif

#if defined(_SINGLE_THREADED)
# define thread_local		/* No thread local variables. */
#endif


#endif /* CDECL_H */
