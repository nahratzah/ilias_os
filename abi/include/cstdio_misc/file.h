#ifndef _CSTDIO_MISC_FILE_H_
#define _CSTDIO_MISC_FILE_H_

#include <cdecl.h>

_namespace_begin(std)

#ifdef __cplusplus
class _file;
#else
struct _file;
#endif

typedef _file* FILE;

_namespace_end(std)

#endif /* _CSTDIO_MISC_FILE_H_ */
