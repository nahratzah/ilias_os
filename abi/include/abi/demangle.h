#ifndef _ABI_DEMANGLE_H_
#define _ABI_DEMANGLE_H_

#include <abi/abi.h>
#include <cdecl.h>

#ifdef __cplusplus
namespace __cxxabiv1 {
#endif

_cdecl_begin

char* __cxa_demangle(const char*, char*, _TYPES(size_t)*, int*) noexcept;
char* __cxa_demangle_gnu3(const char*) noexcept;

_cdecl_end

#ifdef __cplusplus
} /* namespace __cxxabiv1 */

using abi::__cxa_demangle;
using abi::__cxa_demangle_gnu3;
#endif /* __cplusplus */

#endif /* _ABI_DEMANGLE_H_ */
