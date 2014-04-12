#ifndef _FUNCTIONAL_FWD_H_
#define _FUNCTIONAL_FWD_H_

#include <cdecl.h>

_namespace_begin(std)

template<typename T> class reference_wrapper;

template<typename T> reference_wrapper<T> ref(T&) noexcept;
template<typename T> reference_wrapper<const T> ref(const T&) noexcept;
template<typename T> void ref(const T&&) = delete;
template<typename T> void cref(const T&&) = delete;

template<typename T> reference_wrapper<T> ref(reference_wrapper<T>) noexcept;
template<typename T> reference_wrapper<const T> cref(reference_wrapper<T>)
    noexcept;

_namespace_end(std)

#endif /* _FUNCTIONAL_FWD_H_ */
