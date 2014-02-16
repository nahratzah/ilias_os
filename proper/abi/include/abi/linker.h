#ifndef _ABI_LINKER_H_
#define _ABI_LINKER_H_

#include <cdecl.h>
#include <abi/abi.h>
#include <abi/semaphore.h>


namespace __cxxabiv1 {

_cdecl_begin

extern void *__dso_handle;

int __cxa_at_exit(void (*)(void*) noexcept, void*, void*) noexcept;
void __cxa_finalize(void*) noexcept;
void __cxa_finalize_0() noexcept;
void __cxa_finalize_dso(const void*) noexcept;

_cdecl_end

} /* namespace __cxxabiv1 */

#ifdef __cplusplus
using __cxxabiv1::__cxa_at_exit;
using __cxxabiv1::__cxa_finalize;
using __cxxabiv1::__cxa_finalize_0;
#endif /* __cplusplus */

#endif /* _ABI_LINKER_H_ */
