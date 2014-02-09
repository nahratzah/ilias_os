#ifndef SYS_PANIC_H
#define SYS_PANIC_H

#include <cdecl.h>

#ifdef __cplusplus
namespace kernel {
#endif


_cdecl_begin

void panic(const char*, ...) noexcept __attribute__((__noreturn__));
void warn(const char*, ...) noexcept;

_cdecl_end


#ifdef __cplusplus
} /* namespace kernel */
#endif

#endif /* SYS_PANIC_H */
