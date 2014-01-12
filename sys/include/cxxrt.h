#ifndef _CXXRT_H_
#define _CXXRT_H_

#include <cdecl.h>


#ifdef __cplusplus
namespace kernel {
#endif /* __cplusplus */

_cdecl_begin

extern void *__dso_handle;

int __cxa_at_exit(void (*)(void*) noexcept, void*, void*) noexcept;
void __cxa_finalize(void*) noexcept;
void __cxa_finalize_0() noexcept;

_cdecl_end

#ifdef __cplusplus
} /* namespace kernel */

using kernel::__cxa_at_exit;
using kernel::__cxa_finalize;
using kernel::__cxa_finalize_0;
#endif /* __cplusplus */

#endif /* _CXXRT_H_ */
