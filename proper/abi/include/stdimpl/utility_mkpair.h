#ifndef _STDIMPL_UTILITY_MKPAIR_H_
#define _STDIMPL_UTILITY_MKPAIR_H_

#include <cdecl.h>
#include <type_traits>
#include <functional-fwd.h>

_namespace_begin(std)
namespace impl {


template<typename T> struct _pair_elem {
  using type = T;
};

template<typename T> struct _pair_elem<reference_wrapper<T>> {
  using type = typename reference_wrapper<T>::type&;
};

template<typename T> struct pair_elem {
  using type = typename _pair_elem<decay_t<T>>::type;
};

template<typename T> using pair_elem_t = typename pair_elem<T>::type;


} /* namespace std::impl */
_namespace_end(std)

#endif /* _STDIMPL_UTILITY_MKPAIR_H_ */
