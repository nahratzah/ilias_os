#ifndef _STDIMPL_ONCE_FLAG_H_
#define _STDIMPL_ONCE_FLAG_H_

#include <cdecl.h>
#ifdef __cplusplus
# include <atomic>
# include <type_traits>
#endif
#include <cstdbool>

_namespace_begin(std)

struct once_flag {
#ifdef __cplusplus
  atomic<bool> lock;
  volatile bool done;
#else
  _Atomic(Bool) lock;
  volatile Bool done;
#endif
};

#ifdef __cplusplus
static_assert(is_pod<once_flag>::value, "Once flag should be a POD type.");
#endif

_namespace_end(std)

#endif /* _STDIMPL_ONCE_FLAG_H_ */
