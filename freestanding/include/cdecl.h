#ifndef CDECL_H
#define CDECL_H


#ifdef __cplusplus
# define _cdecl_begin	extern "C" {
# define _cdecl_end	} /* extern "C" */
#else
# define _cdecl_begin	/* empty */
# define _cdecl_end	/* empty */
#endif


#endif /* CDECL_H */
