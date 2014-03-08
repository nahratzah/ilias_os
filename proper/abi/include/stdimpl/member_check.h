#ifndef _STDIMPL_MEMBER_CHECK_H_
#define _STDIMPL_MEMBER_CHECK_H_

#include <cdecl.h>
#include <type_traits>

_namespace_begin(std)
namespace impl {

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


_MEMBER_CHECK(member);
_MEMBER_TYPE_CHECK(type);
_STATIC_MEMBER_CHECK(static_member);

struct yes_t {
  int member;
  using type = int;
  static int static_member;
};
struct no_t {};

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

} /* namespace std::impl */
_namespace_end(std)

#endif /* _STDIMPL_MEMBER_CHECK_H_ */
