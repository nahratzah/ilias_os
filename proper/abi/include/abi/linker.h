#ifndef _ABI_LINKER_H_
#define _ABI_LINKER_H_

#include <cdecl.h>
#include <abi/abi.h>

namespace __cxxabiv1 {

_cdecl_begin

#if defined(_KERNEL) || defined(_LOADER)
struct __eh_frame_type;
extern void* __dso_handle;
#endif

struct __eh_frame_data {
  __eh_frame_type* base;
  uintptr_t size;
};

struct __cxa_guard;

__eh_frame_data __resolve_eh_frame(void*) noexcept;

int __cxa_atexit(void (*)(void*) noexcept, void*, void*) noexcept;
void __cxa_finalize(void*) noexcept;
void __cxa_finalize_0() noexcept;
void __cxa_finalize_dso(const void*) noexcept;
void __cxa_pure_virtual() noexcept __attribute__((__noreturn__));
void __cxa_deleted_virtual() noexcept __attribute__((__noreturn__));
int __cxa_guard_acquire(__cxa_guard*) noexcept;
void __cxa_guard_release(__cxa_guard*) noexcept;
void __cxa_guard_abort(__cxa_guard*) noexcept;
void __cxa_throw_bad_array_new_length();

_cdecl_end

} /* namespace __cxxabiv1 */

#ifdef __cplusplus
using __cxxabiv1::__cxa_atexit;
using __cxxabiv1::__cxa_finalize;
using __cxxabiv1::__cxa_finalize_0;
using __cxxabiv1::__cxa_finalize_dso;
using __cxxabiv1::__cxa_pure_virtual;
using __cxxabiv1::__cxa_deleted_virtual;
using __cxxabiv1::__cxa_guard_acquire;
using __cxxabiv1::__cxa_guard_release;
using __cxxabiv1::__cxa_guard_abort;
using __cxxabiv1::__cxa_throw_bad_array_new_length;
using __cxxabiv1::__cxa_guard;
#endif /* __cplusplus */

#endif /* _ABI_LINKER_H_ */
