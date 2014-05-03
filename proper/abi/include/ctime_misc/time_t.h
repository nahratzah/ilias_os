#ifndef _CTIME_MISC_TIME_T_H_
#define _CTIME_MISC_TIME_T_H_

#include <cdecl.h>
#include <abi/abi.h>

_namespace_begin(std)
_cdecl_begin
typedef _TYPES(time_t) time_t;
_cdecl_end
_namespace_end(std)

#ifdef __cplusplus
using _namespace(std)::time_t;
#endif

#endif /* _CTIME_MISC_TIME_T_H_ */
