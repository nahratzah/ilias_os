#ifndef _STDIMPL_UTILITY_GET_H_
#define _STDIMPL_UTILITY_GET_H_

#include <cdecl.h>
#include <type_traits>

_namespace_begin(std)
namespace impl {


template<size_t I> struct pair_getter;

template<> struct pair_getter<0> {
  template<typename T>
  static constexpr auto get(T& p) noexcept ->
      add_lvalue_reference_t<typename T::first_type> {
    return p.first;
  }

  template<typename T>
  static constexpr auto get(const T& p) noexcept ->
      add_const_t<add_lvalue_reference_t<typename T::first_type>> {
    return p.first;
  }

  template<typename T>
  static constexpr auto get(T&& p) noexcept ->
      add_rvalue_reference_t<typename T::first_type> {
    using result_type = add_rvalue_reference_t<typename T::first_type>;

    /*
     * This is forward<T>, but since the header declaring forward
     * depends on us, we execute its step manually.
     */
    return static_cast<result_type>(p.first);
  }
};

template<> struct pair_getter<1> {
  template<typename T>
  static constexpr auto get(T& p) noexcept ->
      add_lvalue_reference_t<typename T::second_type> {
    return p.second;
  }

  template<typename T>
  static constexpr auto get(const T& p) noexcept ->
      add_const_t<add_lvalue_reference_t<typename T::second_type>> {
    return p.second;
  }

  template<typename T>
  static constexpr auto get(T&& p) noexcept ->
      add_rvalue_reference_t<typename T::second_type> {
    using result_type = add_rvalue_reference_t<typename T::second_type>;

    /*
     * This is forward<T>, but since the header declaring forward
     * depends on us, we execute its step manually.
     */
    return static_cast<result_type>(p.second);
  }
};


} /* namespace impl */
_namespace_end(std)

#endif /* _STDIMPL_UTILITY_GET_H_ */
