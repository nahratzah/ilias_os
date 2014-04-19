#ifndef _TUPLE_FWD_H_
#define _TUPLE_FWD_H_

#include <cdecl.h>
#include <cstddef>

_namespace_begin(std)

template<size_t I, class T> class tuple_element;  // undefined.

template<typename T> class tuple_size;  // undefined.
template<typename T> class tuple_size<const T>
: public tuple_size<T> {};
template<typename T> class tuple_size<volatile T>
: public tuple_size<T> {};
template<typename T> class tuple_size<const volatile T>
: public tuple_size<T> {};

_namespace_end(std)

#endif /* _TUPLE_FWD_H_ */
