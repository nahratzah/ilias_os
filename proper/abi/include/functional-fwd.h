#ifndef _FUNCTIONAL_FWD_H_
#define _FUNCTIONAL_FWD_H_

#include <cdecl.h>

_namespace_begin(std)

template<typename> class reference_wrapper;

template<typename T> void ref(const T&&) = delete;
template<typename T> void cref(const T&&) = delete;

template<typename T>
reference_wrapper<T> ref(T& v) noexcept {
  return reference_wrapper<T>(v);
}

template<typename T>
reference_wrapper<const T> cref(const T& v) noexcept {
  return reference_wrapper<const T>(v);
}

template<typename T>
reference_wrapper<T> ref(reference_wrapper<T> rw) noexcept {
  return reference_wrapper<T>(rw.get());
}

template<typename T>
reference_wrapper<const T> cref(reference_wrapper<T> rw) noexcept {
  return reference_wrapper<const T>(rw.get());
}

_namespace_end(std)

#endif /* _FUNCTIONAL_FWD_H_ */
