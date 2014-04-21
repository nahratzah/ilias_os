#ifndef _STDIMPL_ONCE_FLAG_H_
#define _STDIMPL_ONCE_FLAG_H_

#include <cdecl.h>
#include <cstdbool>

_namespace_begin(std)

struct once_flag {
#ifdef __cplusplus
  atomic<bool> lock = { false };
  volatile bool done = false;
#else
  _Atomic(Bool) lock = { false };
  volatile Bool done = false;
#endif
};

_namespace_end(std)

#endif /* _STDIMPL_ONCE_FLAG_H_ */
