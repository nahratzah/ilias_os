#ifndef _STDIMPL_ONCE_FLAG_H_
#define _STDIMPL_ONCE_FLAG_H_

#include <cdecl.h>
#include <cstdint>
#ifdef __cplusplus
# include <type_traits>
#endif

_namespace_begin(std)

struct once_flag {
  uint64_t guard_ = 0;

#ifdef __cplusplus
  constexpr once_flag() noexcept = default;
  once_flag(const once_flag&) = delete;
  once_flag& operator=(const once_flag&) = delete;
#endif
};

#ifdef __cplusplus
static_assert(is_trivially_destructible<once_flag>::value,
              "Once flag must be trivially destructible.");
#endif

_namespace_end(std)

#endif /* _STDIMPL_ONCE_FLAG_H_ */
