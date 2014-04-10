#ifndef _STDIMPL_FUNCTIONAL_COMPARATORS_H_
#define _STDIMPL_FUNCTIONAL_COMPARATORS_H_

#include <cdecl.h>

_namespace_begin(std)


template<typename T = void> struct equal_to {
  constexpr bool operator()(const T& x, const T& y) const { return x == y; }
  using first_argument_type = T;
  using second_argument_type = T;
  using result_type = bool;
};

template<typename T = void> struct not_equal_to {
  constexpr bool operator()(const T& x, const T& y) const { return x != y; }
  using first_argument_type = T;
  using second_argument_type = T;
  using result_type = bool;
};

template<typename T = void> struct greater {
  constexpr bool operator()(const T& x, const T& y) const { return x > y; }
  using first_argument_type = T;
  using second_argument_type = T;
  using result_type = bool;
};

template<typename T = void> struct less {
  constexpr bool operator()(const T& x, const T& y) const { return x < y; }
  using first_argument_type = T;
  using second_argument_type = T;
  using result_type = bool;
};

template<typename T = void> struct greater_equal {
  constexpr bool operator()(const T& x, const T& y) const { return x >= y; }
  using first_argument_type = T;
  using second_argument_type = T;
  using result_type = bool;
};

template<typename T = void> struct less_equal {
  constexpr bool operator()(const T& x, const T& y) const { return x <= y; }
  using first_argument_type = T;
  using second_argument_type = T;
  using result_type = bool;
};

template<> struct equal_to<void> {
  template<typename T, typename U>
  auto operator()(T&& x, U&& y) const ->
      decltype(forward<T>(x) == forward<U>(y));
  using is_transparent = true_type;
};

template<> struct not_equal_to<void> {
  template<typename T, typename U>
  auto operator()(T&& x, U&& y) const ->
      decltype(forward<T>(x) != forward<U>(y));
  using is_transparent = true_type;
};

template<> struct greater<void> {
  template<typename T, typename U>
  auto operator()(T&& x, U&& y) const ->
      decltype(forward<T>(x) > forward<U>(y));
  using is_transparent = true_type;
};

template<> struct less<void> {
  template<typename T, typename U>
  auto operator()(T&& x, U&& y) const ->
      decltype(forward<T>(x) < forward<U>(y));
  using is_transparent = true_type;
};

template<> struct greater_equal<void> {
  template<typename T, typename U>
  auto operator()(T&& x, U&& y) const ->
      decltype(forward<T>(x) >= forward<U>(y));
  using is_transparent = true_type;
};

template<> struct less_equal<void> {
  template<typename T, typename U>
  auto operator()(T&& x, U&& y) const ->
      decltype(forward<T>(x) <= forward<U>(y));
  using is_transparent = true_type;
};


_namespace_end(std)

#include <stdimpl/functional_comparators-inl.h>

#endif /* _STDIMPL_FUNCTIONAL_COMPARATORS_H_ */
