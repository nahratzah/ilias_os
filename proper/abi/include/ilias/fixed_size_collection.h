#ifndef _ILIAS_FIXED_SIZE_COLLECTION_H_
#define _ILIAS_FIXED_SIZE_COLLECTION_H_

#include <cdecl.h>
#include <array>

_namespace_begin(ilias)


/*
 * Allow collections to declare fixed size nesting.
 *
 * This type will derive from integral_constant<size_t, N>,
 * such that, if N != 0, for each instance of T, T::size() will return N.
 */
template<typename>
struct fixed_size_collection
: _namespace(std)::integral_constant<size_t, 0>
{};

template<typename T, size_t N>
struct fixed_size_collection<_namespace(std)::array<T, N>>
: _namespace(std)::integral_constant<size_t, N>
{};

template<typename T, size_t N>
struct fixed_size_collection<T[N]>
: _namespace(std)::integral_constant<size_t, N>
{};


_namespace_end(ilias)

#endif /* _ILIAS_FIXED_SIZE_COLLECTION_H_ */
