#ifndef KMEMORY_H
#define KMEMORY_H

#include <cdecl.h>
#include <cstddef_misc/size_ptrdiff.h>

#ifdef __cplusplus
namespace kernel {
#endif

_cdecl_begin


void* malloc(std::size_t) noexcept;
void free(void*) noexcept;
void* realloc(void*, std::size_t) noexcept;
void* calloc(std::size_t, std::size_t) noexcept;


_cdecl_end

#ifdef __cplusplus
} /* namespace kernel */
#endif /* __cplusplus */

#endif /* KMEMORY_H */
