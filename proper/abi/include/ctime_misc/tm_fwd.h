#ifndef _CTIME_MISC_TM_FWD_H_
#define _CTIME_MISC_TM_FWD_H_

#include <cdecl.h>

_namespace_begin(std)
_cdecl_begin
struct tm;
_cdecl_end
_namespace_end(std)

#if defined(__cplusplus) && !defined(_TEST)
using _namespace(std)::tm;
#endif

#endif /* _CTIME_MISC_TM_FWD_H_ */
