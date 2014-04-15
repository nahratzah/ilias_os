#ifndef _STDIMPL_ALLOC_TRAITS_H_
#define _STDIMPL_ALLOC_TRAITS_H_

#include <cdecl.h>
#include <type_traits>
#include <limits>
#include <stdimpl/member_check.h>

_namespace_begin(std)

template<typename T> struct pointer_traits;  // See <memory>

namespace impl {
namespace allocator {


_MEMBER_TYPE_CHECK(pointer);
_MEMBER_TYPE_CHECK(const_pointer);
_MEMBER_TYPE_CHECK(void_pointer);
_MEMBER_TYPE_CHECK(const_void_pointer);
_MEMBER_TYPE_CHECK(difference_type);
_MEMBER_TYPE_CHECK(size_type);

_MEMBER_TYPE_CHECK(propagate_on_container_move_assignment);
_MEMBER_TYPE_CHECK(propagate_on_container_copy_assignment);
_MEMBER_TYPE_CHECK(propagate_on_container_swap);

_MEMBER_FUNCTION_CHECK(construct);
_MEMBER_FUNCTION_CHECK(destroy);
_MEMBER_FUNCTION_CHECK(max_size);
_MEMBER_FUNCTION_CHECK(allocate);
_MEMBER_FUNCTION_CHECK(select_on_container_copy_construction);


template<typename T>
struct alloc_traits_fallback {
  using value_type = T;
  using pointer = value_type*;
  using const_pointer = const value_type*;
};

template<typename Test, typename Alloc> using traits_select =
    conditional_t<Test::value,
                  Alloc,
                  alloc_traits_fallback<typename Alloc::value_type>>;

template<typename T> struct size_type_for_difference_type {
  using size_type = make_unsigned_t<T>;
};

template<typename P, typename CP> struct void_pointer_for_pointer {
  using void_pointer =
      typename pointer_traits<P>::template rebind<void>;
  using const_void_pointer =
      typename pointer_traits<CP>::template rebind<void>;
};

struct alloc_traits_propagate_defaults {
  using propagate_on_container_copy_assignment = false_type;
  using propagate_on_container_move_assignment = false_type;
  using propagate_on_container_swap = false_type;
};


template<typename Alloc, typename T, typename... Args>
auto construct(Alloc& alloc, T* p, Args&&... args)
    noexcept(noexcept(alloc.construct(p, forward<Args>(args)...))) ->
    enable_if_t<member_function_check_construct<Alloc, T*, Args&&...>::value,
                void> {
  alloc.construct(p, forward<Args>(args)...);
}

template<typename Alloc, typename T, typename... Args>
auto construct(Alloc& alloc, T* p, Args&&... args)
    noexcept(noexcept(
        new (static_cast<void*>(p)) T(forward<Args>(args)...))) ->
    enable_if_t<!member_function_check_construct<Alloc, T*, Args&&...>::value,
                void> {
  new (static_cast<void*>(p)) T(forward<Args>(args)...);
}

template<typename Alloc, typename T>
auto destroy(Alloc& alloc, T* p)
    noexcept(noexcept(alloc.destroy(p))) ->
    enable_if_t<member_function_check_destroy<Alloc, T*>::value,
                void> {
  alloc.destroy(p);
}

template<typename Alloc, typename T>
auto destroy(Alloc& alloc, T* p)
    noexcept(noexcept(p->~T())) ->
    enable_if_t<!member_function_check_destroy<Alloc, T*>::value, void> {
  if (p) p->~T();
}

template<typename Result, typename Alloc, typename Size, typename T>
auto allocate(Alloc& alloc, Size&& n, T&& hint)
    noexcept(noexcept(alloc.allocate(forward<Size>(n), forward<T>(hint)))) ->
    enable_if_t<member_function_check_allocate<Alloc, Size&&, T&&>::value,
                Result> {
  return alloc.allocate(forward<Size>(n), forward<T>(hint));
}

template<typename Result, typename Alloc, typename Size, typename T>
auto allocate(Alloc& alloc, Size&& n, T&&)
    noexcept(noexcept(alloc.allocate(forward<Size>(n)))) ->
    enable_if_t<!member_function_check_allocate<Alloc, Size&&, T&&>::value,
                Result> {
  return alloc.allocate(forward<Size>(n));
}

template<typename Result, typename Alloc>
auto max_size(const Alloc& alloc)
    noexcept(noexcept(alloc.max_size())) ->
    enable_if_t<member_function_check_max_size<Alloc>::value, Result> {
  return alloc.max_size();
}

template<typename Result, typename Alloc>
constexpr auto max_size(const Alloc&) noexcept ->
    enable_if_t<!member_function_check_max_size<Alloc>::value, Result> {
  return numeric_limits<Result>::max();
}

template<typename Alloc>
auto select_on_container_copy_construction(const Alloc& alloc)
    noexcept(noexcept(alloc.select_on_container_copy_construction())) ->
    enable_if_t<member_function_check_select_on_container_copy_construction<Alloc>::value,
                Alloc> {
  return alloc.select_on_container_copy_construction();
}

template<typename Alloc>
auto select_on_container_copy_construction(const Alloc& alloc)
    noexcept(noexcept(is_nothrow_copy_constructible<Alloc>::value)) ->
    enable_if_t<!member_function_check_select_on_container_copy_construction<Alloc>::value,
                Alloc> {
  return alloc;
}


template<typename, typename> struct rebind_immediately;  // undefined
template<typename U, typename T, typename... Args,
         template<typename, typename...> class Template>
struct rebind_immediately<U, Template<T, Args...>> {
  using type = Template<U, Args...>;
};

template<typename U, typename T> struct allocator_rebind {
  using type = typename T::template rebind<U>::other;
};

template<template<typename> class> struct rebind_matcher
{ using type = int; };

template<typename T, typename = int> struct has_rebind : false_type {};
template<typename T>
struct has_rebind<T, typename rebind_matcher<T::template rebind>::type>
: true_type {};

template<typename U, typename T> using resolve_rebind =
    typename conditional_t<has_rebind<T>::value,
        allocator_rebind<U, T>,
        rebind_immediately<U, T>>::type;


}} /* namespace std::impl::allocator */
_namespace_end(std)

#endif /* _STDIMPL_ALLOC_TRAITS_H_ */
