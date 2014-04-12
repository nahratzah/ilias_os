#ifndef _STDIMPL_POINTER_TRAITS_H_
#define _STDIMPL_POINTER_TRAITS_H_

#include <cdecl.h>
#include <stdimpl/member_check.h>

_namespace_begin(std)
namespace impl {
namespace pointer {


_MEMBER_TYPE_CHECK(element_type);
_MEMBER_TYPE_CHECK(difference_type);

template<typename> struct derive_pointer_element_type;  // undefined
template<typename T, typename... Any, template<typename, typename...> class P>
struct derive_pointer_element_type<P<T, Any...>> {
  using type = T;
};

template<typename P> struct use_pointer_element_type {
  using type = typename P::element_type;
};

template<typename P, bool = member_type_check_element_type<P>::value>
struct select_element_type_impl {
  using type = typename use_pointer_element_type<P>::type;
};
template<typename P>
struct select_element_type_impl<P, false> {
  using type = typename derive_pointer_element_type<P>::type;
};

template<typename P> using select_element_type =
    typename select_element_type_impl<P>::type;

template<typename T> struct difference_type_default {
  using difference_type = T;
};

struct cannot_instantiate_me {
  cannot_instantiate_me() = delete;
  cannot_instantiate_me(const cannot_instantiate_me&) = delete;
  cannot_instantiate_me& operator=(const cannot_instantiate_me&) = delete;
  ~cannot_instantiate_me() = delete;
};


template<typename, typename> struct rebind_immediately;  // undefined
template<typename U, typename T, typename... Args,
         template<typename, typename...> class Template>
struct rebind_immediately<U, Template<T, Args...>> {
  using type = Template<U, Args...>;
};

template<typename U, typename T> struct pointer_rebind {
  using type = typename T::template rebind<U>;
};

template<template<typename> class> struct rebind_matcher
{ using type = int; };

template<typename T, typename = int> struct has_rebind : false_type {};
template<typename T>
struct has_rebind<T, typename rebind_matcher<T::template rebind>::type>
: true_type {};

template<typename U, typename T> using resolve_rebind =
    typename conditional_t<has_rebind<T>::value,
                           pointer_rebind<U, T>,
                           rebind_immediately<U, T>>::type;


}} /* namespace std::impl::pointer */
_namespace_end(std)

#endif /* _STDIMPL_POINTER_TRAITS_H_ */
