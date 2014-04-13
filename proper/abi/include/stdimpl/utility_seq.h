#ifndef _STDIMPL_UTILITY_SEQ_H_
#define _STDIMPL_UTILITY_SEQ_H_

#include <cdecl.h>

_namespace_begin(std)
namespace impl {

template<typename, size_t> struct extend_sequence;  // undefined

template<template<typename X, X...> class Type, typename T, T... Seq,
         size_t Sz>
struct extend_sequence<Type<T, Seq...>, Sz> {
  struct current {
    using type = Type<T, Seq...>;
  };

  using successor = extend_sequence<Type<T, Seq..., T(sizeof...(Seq))>, Sz>;

  using type = typename conditional_t<(sizeof...(Seq) == Sz),
                                      current,
                                      successor>::type;
};

} /* namespace impl */
_namespace_end(std)

#endif /* _STDIMPL_UTILITY_SEQ_H_ */
