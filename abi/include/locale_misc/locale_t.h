#ifndef _LOCALE_MISC_LOCALE_T_H_
#define _LOCALE_MISC_LOCALE_T_H_

#include <cdecl.h>

_namespace_begin(std)

#ifdef __cplusplus
class locale;  // Implemented in locale.h
typedef locale* locale_t;
#else
struct __locale;  // Opaque type.
typedef struct __locale* locale_t;
#endif

_namespace_end(std)

#endif /* _LOCALE_MISC_LOCALE_T_H_ */
