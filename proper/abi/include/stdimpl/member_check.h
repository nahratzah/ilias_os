#ifndef _STDIMPL_MEMBER_CHECK_H_
#define _STDIMPL_MEMBER_CHECK_H_

#include <cdecl.h>
#include <type_traits>
#include <utility>

_namespace_begin(std)
namespace impl {

template<typename... Args> struct _function_check_args;

#define _MEMBER_CHECK(member)						\
  template<typename T, typename = int>					\
  struct _member_check_##member##_impl					\
  : ::_namespace(std)::false_type {};					\
									\
  template<typename T>							\
  struct _member_check_##member##_impl<					\
      T, decltype(::_namespace(std)::declval<T>().member, 0)>		\
  : ::_namespace(std)::true_type {};					\
									\
  template<typename T> using member_check_##member =			\
      typename _member_check_##member##_impl<T>::type


#define _MEMBER_TYPE_CHECK(member)					\
  template<typename T, typename V = void>				\
  struct _member_type_check_##member##_impl				\
  : ::_namespace(std)::false_type {};					\
									\
  template<typename T>							\
  struct _member_type_check_##member##_impl<T,				\
    ::_namespace(std)::conditional_t<					\
        ::_namespace(std)::is_void<typename T::member>::value,		\
        void, void>>							\
  : ::_namespace(std)::true_type {					\
  };									\
									\
  template<typename T> using member_type_check_##member =		\
      typename _member_type_check_##member##_impl<T>::type


#define _STATIC_MEMBER_CHECK(member)					\
  template<typename T, typename = int>					\
  struct _static_member_check_##member##_impl				\
  : ::_namespace(std)::false_type {};					\
									\
  template<typename T>							\
  struct _static_member_check_##member##_impl<				\
      T, decltype(T::member, 0)>					\
  : ::_namespace(std)::true_type {};					\
									\
  template<typename T> using static_member_check_##member =		\
      typename _static_member_check_##member##_impl<T>::type


#define _FUNCTION_CHECK(function)					\
template<typename Args, typename = int>					\
struct _function_check_##function##_impl				\
: ::_namespace(std)::false_type {};					\
									\
template<typename... Args>						\
struct _function_check_##function##_impl<				\
    ::_namespace(std)::impl::_function_check_args<Args...>,		\
    decltype(function(declval<Args>()...), 0)>				\
: ::_namespace(std)::true_type {};					\
									\
template<typename... Args> using function_check_##function =		\
    typename _function_check_##function##_impl<				\
        ::_namespace(std)::impl::_function_check_args<Args...>>::type


#define _MEMBER_FUNCTION_CHECK(function)				\
template<typename T, typename Args, typename = int>			\
struct _member_function_check_##function##_impl				\
: ::_namespace(std)::false_type {};					\
									\
template<typename T, typename... Args>					\
struct _member_function_check_##function##_impl<			\
    T,									\
    ::_namespace(std)::impl::_function_check_args<Args...>,		\
    decltype(::_namespace(std)::declval<T>().function(			\
        declval<Args>()...), 0)>					\
: ::_namespace(std)::true_type {};					\
									\
template<typename T, typename... Args>					\
using member_function_check_##function =				\
    typename _member_function_check_##function##_impl<			\
        T,								\
        ::_namespace(std)::impl::_function_check_args<Args...>>::type


#define _STATIC_MEMBER_FUNCTION_CHECK(function)				\
template<typename T, typename Args, typename = int>			\
struct _static_member_function_check_##function##_impl			\
: ::_namespace(std)::false_type {};					\
									\
template<typename T, typename... Args>					\
struct _static_member_function_check_##function##_impl<			\
    T,									\
    ::_namespace(std)::impl::_function_check_args<Args...>,		\
    decltype(T::function(declval<Args>()...), 0)>			\
: ::_namespace(std)::true_type {};					\
									\
template<typename T, typename... Args>					\
using static_member_function_check_##function =				\
    typename _static_member_function_check_##function##_impl<		\
        T,								\
        ::_namespace(std)::impl::_function_check_args<Args...>>::type


_MEMBER_CHECK(member);
_MEMBER_TYPE_CHECK(type);
_STATIC_MEMBER_CHECK(static_member);
_FUNCTION_CHECK(function);
_MEMBER_FUNCTION_CHECK(member_function);
_STATIC_MEMBER_FUNCTION_CHECK(static_member_function);

struct yes_t {
  int member;
  using type = int;
  static int static_member;
  int member_function();
  static int static_member_function();
};
struct no_t {};

void function(yes_t);

static_assert(member_check_member<yes_t>::value,
              "_MEMBER_CHECK macro failure");
static_assert(!member_check_member<no_t>::value,
              "_MEMBER_CHECK macro failure");
static_assert(member_type_check_type<yes_t>::value,
              "_MEMBER_TYPE_CHECK macro failure");
static_assert(!member_type_check_type<no_t>::value,
              "_MEMBER_TYPE_CHECK macro failure");
static_assert(static_member_check_static_member<yes_t>::value,
              "_STATIC_MEMBER_CHECK macro failure");
static_assert(!static_member_check_static_member<no_t>::value,
              "_STATIC_MEMBER_CHECK macro failure");

static_assert(function_check_function<yes_t>::value,
              "_FUNCTION_CHECK macro failure");
static_assert(!function_check_function<no_t>::value,
              "_FUNCTION_CHECK macro failure");
static_assert(!function_check_function<yes_t*>::value,
              "_FUNCTION_CHECK macro failure");
static_assert(function_check_function<const yes_t>::value,
              "_FUNCTION_CHECK macro failure");
static_assert(!function_check_function<const no_t>::value,
              "_FUNCTION_CHECK macro failure");
static_assert(!function_check_function<const yes_t*>::value,
              "_FUNCTION_CHECK macro failure");
static_assert(!function_check_function<volatile yes_t>::value,
              "_FUNCTION_CHECK macro failure");
static_assert(!function_check_function<volatile no_t>::value,
              "_FUNCTION_CHECK macro failure");
static_assert(!function_check_function<volatile yes_t*>::value,
              "_FUNCTION_CHECK macro failure");
static_assert(!function_check_function<const volatile yes_t>::value,
              "_FUNCTION_CHECK macro failure");
static_assert(!function_check_function<const volatile no_t>::value,
              "_FUNCTION_CHECK macro failure");
static_assert(!function_check_function<const volatile yes_t*>::value,
              "_FUNCTION_CHECK macro failure");
static_assert(function_check_function<yes_t&>::value,
              "_FUNCTION_CHECK macro failure");
static_assert(!function_check_function<no_t&>::value,
              "_FUNCTION_CHECK macro failure");
static_assert(!function_check_function<yes_t*&>::value,
              "_FUNCTION_CHECK macro failure");
static_assert(function_check_function<const yes_t&>::value,
              "_FUNCTION_CHECK macro failure");
static_assert(!function_check_function<const no_t&>::value,
              "_FUNCTION_CHECK macro failure");
static_assert(!function_check_function<const yes_t*&>::value,
              "_FUNCTION_CHECK macro failure");
static_assert(!function_check_function<volatile yes_t&>::value,
              "_FUNCTION_CHECK macro failure");
static_assert(!function_check_function<volatile no_t&>::value,
              "_FUNCTION_CHECK macro failure");
static_assert(!function_check_function<volatile yes_t*&>::value,
              "_FUNCTION_CHECK macro failure");
static_assert(!function_check_function<const volatile yes_t&>::value,
              "_FUNCTION_CHECK macro failure");
static_assert(!function_check_function<const volatile no_t&>::value,
              "_FUNCTION_CHECK macro failure");
static_assert(!function_check_function<const volatile yes_t*&>::value,
              "_FUNCTION_CHECK macro failure");

static_assert(member_function_check_member_function<yes_t>::value,
              "_MEMBER_FUNCTION_CHECK macro failure");
static_assert(!member_function_check_member_function<yes_t, int>::value,
              "_MEMBER_FUNCTION_CHECK macro failure");
static_assert(!member_function_check_member_function<yes_t, int, bool>::value,
              "_MEMBER_FUNCTION_CHECK macro failure");
static_assert(!member_function_check_member_function<no_t>::value,
              "_MEMBER_FUNCTION_CHECK macro failure");

static_assert(static_member_function_check_static_member_function<yes_t>::value,
              "_STATIC_MEMBER_FUNCTION_CHECK macro failure");
static_assert(!static_member_function_check_static_member_function<yes_t, int>::value,
              "_STATIC_MEMBER_FUNCTION_CHECK macro failure");
static_assert(!static_member_function_check_static_member_function<yes_t, int,
                                                            bool>::value,
              "_STATIC_MEMBER_FUNCTION_CHECK macro failure");
static_assert(!static_member_function_check_static_member_function<no_t>::value,
              "_STATIC_MEMBER_FUNCTION_CHECK macro failure");

} /* namespace std::impl */
_namespace_end(std)

#endif /* _STDIMPL_MEMBER_CHECK_H_ */
