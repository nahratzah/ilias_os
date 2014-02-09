#ifndef CSTDDEF_MISC_INTPTR_H
#define CSTDDEF_MISC_INTPTR_H

#include <abi/abi.h>
#include <cdecl.h>

_namespace_begin(std)


typedef _TYPES(uintptr_t)	uintptr_t;
typedef _TYPES(intptr_t)	intptr_t;


_namespace_end(std)


#ifndef _TEST
using std::uintptr_t;
using std::intptr_t;
#endif


#endif /* CSTDDEF_MISC_INTPTR_H */
