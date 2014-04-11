#ifndef _LOADER_ABI_H_
#define _LOADER_ABI_H_

#include <abi/abi.h>
#include <tuple>

namespace loader {

_namespace(std)::tuple<void*, _TYPES(size_t)> heap_malloc(_TYPES(size_t))
    noexcept;

constexpr bool heap_free(void*, _TYPES(size_t)) noexcept { return false; }

} /* namespace loader */

#endif /* _LOADER_ABI_H_ */
