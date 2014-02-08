#ifndef CSTDDEF_MISC_SIZE_PTRDIFF_H
#define CSTDDEF_MISC_SIZE_PTRDIFF_H

#include <abi/abi.h>
#include <cdecl.h>

_namespace_begin(std)


typedef _TYPES(size_t)		size_t;
typedef _TYPES(ssize_t)		ssize_t;
typedef _TYPES(ptrdiff_t)	ptrdiff_t;


_namespace_end(std)


#if defined(__cplusplus) && !defined(_TEST)
using std::size_t;
using std::ssize_t;
using std::ptrdiff_t;
#endif  /* __cplusplus && !_TEST */


#endif /* CSTDDEF_MISC_SIZE_PTRDIFF_H */
