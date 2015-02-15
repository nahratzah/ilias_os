#ifndef _CSTDIO_MISC_OFF_T_H_
#define _CSTDIO_MISC_OFF_T_H_

#include <cdecl.h>
#include <abi/abi.h>

_namespace_begin(std)
_cdecl_begin

typedef _TYPES(off_t) off_t;

_cdecl_end
_namespace_end(std)

#ifdef __cplusplus
using std::off_t;
#endif

#endif /* _CSTDIO_MISC_OFF_T_H_ */
