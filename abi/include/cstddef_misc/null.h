#ifndef CSTDDEF_MISC_NULL_H
#define CSTDDEF_MISC_NULL_H

#ifdef __cplusplus
# ifndef _TEST
#   define NULL			nullptr
# endif

_namespace_begin(std)
using nullptr_t = decltype(nullptr);
_namespace_end(std)
#else
# ifndef _TEST
#   define NULL			__nullptr
# endif
#endif

#endif /* CSTDDEF_MISC_NULL_H */
