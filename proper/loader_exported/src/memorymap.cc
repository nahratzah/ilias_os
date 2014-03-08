#include <loader/memorymap.h>

_namespace_begin(std)

template class vector<loader::memorymap::range,
	              abi::ext::relptr_allocator<loader::memorymap::range>>;

_namespace_end(std)
