#ifndef CDECL_H
#define CDECL_H


#ifdef __cplusplus
# define _cdecl_begin	extern "C" {
# define _cdecl_end	} /* extern "C" */
#else
# define _cdecl_begin	/* empty */
# define _cdecl_end	/* empty */
#endif

#define _predict_true(x)	__builtin_expect(((x) != 0), 1)
#define _predict_false(x)	__builtin_expect(((x) != 0), 0)

#ifndef _LIBC_EXPORT
# define _LIBC_EXPORT		/* External function requires no linkage spec. */
#endif /* _LIBC_EXPORT */

#ifndef __cplusplus
# define noexcept		/* C is always noexcept. */
#endif

#ifndef __cplusplus
# define __restrict		restrict
#endif


#endif /* CDECL_H */
