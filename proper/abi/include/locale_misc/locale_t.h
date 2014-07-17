#ifndef _LOCALE_MISC_LOCALE_T_H_
#define _LOCALE_MISC_LOCALE_T_H_

#include <cdecl.h>

_namespace_begin(std)

struct __locale;  // Opaque type.
#ifdef __cplusplus
using locale_t = const __locale*;
#else
typedef const struct __locale* locale_t;
#endif

_namespace_end(std)

#if defined(__cplusplus) && !defined(_TEST)
using _namespace(std)::locale_t;
#endif

#endif /* _LOCALE_MISC_LOCALE_T_H_ */
