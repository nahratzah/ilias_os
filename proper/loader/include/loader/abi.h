#include <abi/abi.h>

namespace loader {

void* heap_malloc(_TYPES(size_t)*, _TYPES(size_t)) noexcept;

constexpr bool heap_free(void*, _TYPES(size_t)) noexcept { return false; }

} /* namespace loader */
