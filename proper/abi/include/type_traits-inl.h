#ifndef _TYPE_TRAITS_INL_H_
#define _TYPE_TRAITS_INL_H_

#include <type_traits>

_namespace_begin(std)


template<typename T> struct is_void
: is_same<remove_cv_t<T>, void> {};

template<typename T> struct is_null_pointer
: is_same<remove_cv_t<T>, decltype(nullptr)> {};

template<typename T> struct _is_integral : false_type {};
template<> struct _is_integral<bool> : true_type {};
template<> struct _is_integral<char> : true_type {};
template<> struct _is_integral<signed char> : true_type {};
template<> struct _is_integral<unsigned char> : true_type {};
template<> struct _is_integral<wchar_t> : true_type {};
template<> struct _is_integral<short> : true_type {};
template<> struct _is_integral<unsigned short> : true_type {};
template<> struct _is_integral<int> : true_type {};
template<> struct _is_integral<unsigned int> : true_type {};
template<> struct _is_integral<long> : true_type {};
template<> struct _is_integral<unsigned long> : true_type {};
template<> struct _is_integral<long long> : true_type {};
template<> struct _is_integral<unsigned long long> : true_type {};
template<typename T> struct is_integral : _is_integral<remove_cv_t<T>> {};

template<typename T> struct _is_floating_point : false_type {};
template<> struct _is_floating_point<float> : true_type {};
template<> struct _is_floating_point<double> : true_type {};
template<> struct _is_floating_point<long double> : true_type {};
template<typename T> struct is_floating_point
: _is_floating_point<remove_cv_t<T>> {};

template<typename T> struct _is_array : false_type {};
template<typename T, size_t N> struct _is_array<T[N]> : true_type {};
template<typename T> struct _is_array<T[]> : true_type {};
template<typename T> struct is_array : _is_array<remove_cv_t<T>> {};

template<typename T> struct _is_pointer : false_type {};
template<typename T> struct _is_pointer<T*> : true_type {};
template<typename T> struct is_pointer : _is_pointer<remove_cv_t<T>> {};

template<typename T> struct is_lvalue_reference : false_type {};
template<typename T> struct is_lvalue_reference<T&> : true_type {};
template<typename T> struct is_rvalue_reference : false_type {};
template<typename T> struct is_rvalue_reference<T&&> : true_type {};

template<typename T> struct _is_member_object_pointer : false_type {};
template<typename T, typename U> struct _is_member_object_pointer<T U::*>
: integral_constant<bool, !is_function<T>::value> {};
template<typename T> struct is_member_object_pointer
: _is_member_object_pointer<remove_cv_t<T>> {};

template<typename T> struct _is_member_function_pointer : false_type {};
template<typename T, typename U> struct _is_member_function_pointer<T U::*>
: is_function<T> {};
template<typename T> struct is_member_function_pointer
: _is_member_function_pointer<remove_cv_t<T>> {};

template<typename T> struct _is_enum
: integral_constant<bool, __is_enum(T)> {};
template<typename T> struct is_enum : _is_enum<remove_cv_t<T>> {};

template<typename T> struct _is_union
: integral_constant<bool, __is_union(T)> {};
template<typename T> struct is_union : _is_union<remove_cv_t<T>> {};

template<typename T> struct _is_class
: integral_constant<bool, __is_class(T)> {};
template<typename T> struct is_class : _is_class<remove_cv_t<T>> {};

template<typename T> struct _is_function : false_type {};
template<typename T, typename... Args> struct _is_function<T(Args...)>
: true_type {};
template<typename T> struct is_function : _is_function<remove_cv_t<T>> {};


template<typename T> struct is_reference : false_type {};
template<typename T> struct is_reference<T&> : true_type {};
template<typename T> struct is_reference<T&&> : true_type {};
template<typename T> struct is_arithmetic
: integral_constant<bool,
                    is_integral<T>::value || is_floating_point<T>::value> {};
template<typename T> struct is_fundamental;  // XXX
template<typename T> struct is_object;  // XXX
template<typename T> struct is_scalar;  // XXX
template<typename T> struct is_compound;  // XXX

template<typename T> struct _is_member_pointer : false_type {};
template<typename T, typename U> struct _is_member_pointer<T U::*>
: true_type {};
template<typename T> struct is_member_pointer
: _is_member_pointer<remove_cv_t<T>> {};


template<typename T> struct is_const : false_type {};
template<typename T> struct is_const<const T> : true_type {};

template<typename T> struct is_volatile : false_type {};
template<typename T> struct is_volatile<volatile T> : true_type {};

template<typename T> struct is_trivial;  // XXX

template<typename T> struct _is_trivially_copyable
: integral_constant<bool, __has_trivial_copy(T)> {};
template<typename T> struct is_trivially_copyable
: _is_trivially_copyable<remove_cv_t<T>> {};

template<typename T> struct is_standard_layout;  // XXX

template<typename T> struct _is_pod
: integral_constant<bool, __is_pod(T)> {};
template<typename T> struct is_pod : _is_pod<remove_cv_t<T>> {};

template<typename T> struct _is_literal_type
: integral_constant<bool, __is_literal(T)> {};
template<typename T> struct is_literal_type
: _is_literal_type<remove_cv_t<T>> {};

template<typename T> struct _is_empty
: integral_constant<bool, __is_empty(T)> {};
template<typename T> struct is_empty : _is_empty<remove_cv_t<T>> {};

template<typename T> struct is_polymorphic;  // XXX

template<typename T> struct _is_abstract
: integral_constant<bool, __is_abstract(T)> {};
template<typename T> struct is_abstract : _is_abstract<remove_cv_t<T>> {};


template<typename T, bool> struct _is_signed : false_type {};
template<typename T> struct _is_signed<T, true>
: integral_constant<bool, (T(-1) < T(0))> {};
template<typename T> struct is_signed
: _is_signed<remove_cv_t<T>, is_arithmetic<T>::value> {};

template<typename T, bool> struct _is_unsigned : false_type {};
template<typename T> struct _is_unsigned<T, true>
: integral_constant<bool, (T(0) < T(-1))> {};
template<typename T> struct is_unsigned
: _is_unsigned<remove_cv_t<T>, is_arithmetic<T>::value> {};


template<typename T, typename... Args> struct _is_constructible
: integral_constant<bool, __is_constructible(T, Args...)> {};
template<typename T, typename... Args> struct is_constructible
: _is_constructible<remove_cv_t<T>, Args...> {};

template<typename T> struct is_default_constructible
: is_constructible<T> {};

template<typename T> struct is_copy_constructible
: is_constructible<T, const remove_cv_t<remove_reference_t<T>>&> {};

template<typename T> struct is_move_constructible
: is_constructible<T, remove_cv_t<remove_reference_t<T>>&&> {};


template<typename T, typename U> struct _is_assignable_support
{ static T& t(); static U& u(); };
template<typename T, typename U, typename = int> struct _is_assignable
: false_type {};
template<typename T, typename U> struct _is_assignable<T, U,
    decltype((_is_assignable_support<T, U>::t() =
              _is_assignable_support<T, U>::u()), int(0))>
: true_type {};
template<typename T, typename U> struct is_assignable
: _is_assignable<remove_cv_t<T>, U> {};

template<typename T> struct is_copy_assignable
: is_assignable<T, const remove_cv_t<remove_reference_t<T>>&> {};

template<typename T> struct is_move_assignable
: is_assignable<T, remove_cv_t<remove_reference_t<T>>&&> {};

template<typename T> struct _is_destructible_support { static T& v(); };
template<typename T, typename = int> struct _is_destructible : false_type {};
template<typename T> struct _is_destructible<T,
    decltype(_is_destructible_support<T>::v().~T(), int(0))> : true_type {};
template<typename T> struct is_destructible
: _is_destructible<remove_cv_t<T>>::type {};


template<typename T, typename... Args> struct _is_trivially_constructible
: integral_constant<bool, __is_trivially_constructible(T, Args...)> {};
template<typename T, typename... Args> struct is_trivially_constructible
: _is_trivially_constructible<remove_cv_t<T>, Args...> {};

template<typename T> struct is_trivially_default_constructible
: is_trivially_constructible<T> {};

template<typename T> struct is_trivially_copy_constructible
: is_trivially_constructible<T, const remove_reference_t<T>&> {};

template<typename T> struct is_trivially_move_constructible
: is_trivially_constructible<T, remove_reference_t<T>&&> {};


template<typename T, typename U> struct _is_trivially_assignable
: integral_constant<bool, __is_trivially_assignable(T, U)> {};
template<typename T, typename U> struct is_trivially_assignable
: _is_trivially_assignable<remove_cv_t<T>, U> {};

template<typename T> struct is_trivially_copy_assignable
: is_trivially_assignable<T, const remove_reference_t<T>&> {};

template<typename T> struct is_trivially_move_assignable
: is_trivially_assignable<T, remove_reference_t<T>&&> {};

template<typename T> struct _is_trivially_destructible
: integral_constant<bool, __has_trivial_destructor(T)> {};
template<typename T> struct is_trivially_destructible
: _is_trivially_destructible<remove_cv_t<T>> {};


template<typename T, typename... Args> struct _is_nothrow_constructible
: integral_constant<bool, __is_nothrow_constructible(T, Args...)> {};
template<typename T, typename... Args> struct is_nothrow_constructible
: _is_nothrow_constructible<remove_cv_t<T>, Args...> {};

template<typename T> struct _is_nothrow_default_constructible
: is_nothrow_constructible<T> {};
template<typename T> struct is_nothrow_default_constructible
: _is_nothrow_default_constructible<remove_cv_t<T>> {};

template<typename T> struct _is_nothrow_copy_constructible
: _is_nothrow_constructible<T, add_lvalue_reference_t<add_const_t<T>>> {};
template<typename T> struct is_nothrow_copy_constructible
: _is_nothrow_copy_constructible<remove_cv_t<T>> {};

template<typename T> struct _is_nothrow_move_constructible
: is_nothrow_constructible<T, add_rvalue_reference_t<T>> {};
template<typename T> struct is_nothrow_move_constructible
: _is_nothrow_move_constructible<remove_cv_t<T>> {};


template<typename T, typename... Args> struct is_nothrow_assignable;  // XXX

template<typename T> struct _is_nothrow_copy_assignable
: integral_constant<bool,
                    __is_nothrow_assignable(T,
                        add_lvalue_reference_t<add_const_t<T>>)> {};
template<typename T> struct is_nothrow_copy_assignable
: _is_nothrow_copy_assignable<remove_cv_t<T>> {};

template<typename T> struct _is_nothrow_move_assignable
: integral_constant<bool,
                    __is_nothrow_assignable(T, add_rvalue_reference_t<T>)> {};
template<typename T> struct is_nothrow_move_assignable
: _is_nothrow_move_assignable<remove_cv_t<T>> {};

template<typename T> struct is_nothrow_destructible;  // XXX
template<typename T> struct has_virtual_destructor;  // XXX


template<typename T> struct _alignment_of
: integral_constant<size_t, alignof(T)> {};
template<typename T> struct alignment_of : _alignment_of<remove_cv_t<T>> {};

template<typename T> struct _rank : integral_constant<int, 0> {};
template<typename T> struct _rank<T[]>
: integral_constant<int, _rank<remove_cv_t<T>>::value + 1> {};
template<typename T, size_t N> struct _rank<T[N]>
: integral_constant<int, _rank<remove_cv_t<T>>::value + 1> {};
template<typename T> struct rank : _rank<remove_cv_t<T>> {};

template<typename T, unsigned I> struct _extent
: integral_constant<size_t, 0> {};
template<typename T, unsigned I> struct _extent<T[], I>
: _extent<remove_cv_t<T>, I - 1> {};
template<typename T, size_t N, unsigned I> struct _extent<T[N], I>
: _extent<remove_cv_t<T>, I - 1> {};
template<typename T> struct _extent<T[], 0>
: integral_constant<size_t, 0> {};
template<typename T, size_t N> struct _extent<T[N], 0>
: integral_constant<size_t, N> {};
template<typename T, unsigned I> struct extent
: _extent<remove_cv_t<T>, I> {};


template<typename T, typename U> struct _is_same : false_type {};
template<typename T> struct _is_same<T, T> : true_type {};
template<typename T, typename U> struct is_same
: _is_same<remove_reference_t<T>, remove_reference_t<U>> {};

template<typename Base, typename Derived> struct _is_base_of
: integral_constant<bool, __is_base_of(Base, Derived)> {};
template<typename Base, typename Derived> struct is_base_of
: _is_base_of<remove_cv_t<Base>, remove_cv_t<Derived>> {};

template<typename From, typename To> struct _is_convertible
: integral_constant<bool, __is_convertible_to(From, To)> {};
template<typename From, typename To> struct is_convertible
: _is_convertible<remove_cv_t<From>, remove_cv_t<To>> {};


template<typename T> struct remove_const { using type = T; };
template<typename T> struct remove_const<const T> { using type = T; };
template<typename T> struct remove_volatile { using type = T; };
template<typename T> struct remove_volatile<volatile T> { using type = T; };
template<typename T> struct remove_cv
{ using type = remove_const_t<remove_volatile_t<T>>; };
template<typename T> struct add_const { using type = const T; };
template<typename T> struct add_volatile { using type = volatile T; };
template<typename T> struct add_cv { using type = const volatile T; };


template<typename T> struct remove_reference { using type = T; };
template<typename T> struct remove_reference<T&> { using type = T; };
template<typename T> struct remove_reference<T&&> { using type = T; };

template<typename T> struct add_lvalue_reference { using type = T&; };

template<typename T> struct add_rvalue_reference { using type = T&&; };


template<typename T> struct _make_signed_ll;
template<typename T> struct _make_signed {
  using type =
      conditional_t<is_integral<T>::value,
          conditional_t<is_signed<T>::value, T,
                        typename _make_signed_ll<T>::type>,
          conditional_t<sizeof(T) <= sizeof(signed char),
              signed char,
              conditional_t<sizeof(T) <= sizeof(short),
                  short,
                  conditional_t<sizeof(T) <= sizeof(int),
                      int,
                      conditional_t<sizeof(T) <= sizeof(long),
                          long,
                          enable_if_t<sizeof(T) <= sizeof(long long),
                              long long>>>>>>;
};
#ifdef __CHAR_UNSIGNED__
template<> struct _make_signed_ll<char>
{ using type = signed char; };
#endif
#ifdef __WCHAR_UNSIGNED__
template<> struct _make_signed_ll<wchar_t>
{ using type = int; };
#endif
template<> struct _make_signed_ll<unsigned char>
{ using type = signed char; };
template<> struct _make_signed_ll<unsigned short>
{ using type = short; };
template<> struct _make_signed_ll<unsigned int>
{ using type = int; };
template<> struct _make_signed_ll<unsigned long>
{ using type = long; };
template<> struct _make_signed_ll<unsigned long long>
{ using type = long long; };
template<typename T> struct _make_signed_hl
{ using type = typename _make_signed<T>::type; };
template<typename T> struct _make_signed_hl<const T>
{ using type = add_const_t<typename _make_signed<T>::type>; };
template<typename T> struct _make_signed_hl<volatile T>
{ using type = add_volatile_t<typename _make_signed<T>::type>; };
template<typename T> struct _make_signed_hl<const volatile T>
{ using type = add_cv_t<typename _make_signed<T>::type>; };
template<typename T> struct make_signed
: _make_signed_hl<remove_reference_t<T>> {};

template<typename T> struct _make_unsigned_ll;
template<typename T> struct _make_unsigned {
  using type =
      conditional_t<is_integral<T>::value,
          conditional_t<is_unsigned<T>::value, T,
                        typename _make_unsigned_ll<T>::type>,
          conditional_t<sizeof(T) <= sizeof(unsigned char),
              unsigned char,
              conditional_t<sizeof(T) <= sizeof(unsigned short),
                  unsigned short,
                  conditional_t<sizeof(T) <= sizeof(unsigned int),
                      unsigned int,
                      conditional_t<sizeof(T) <= sizeof(unsigned long),
                          unsigned long,
                          enable_if_t<sizeof(T) <= sizeof(unsigned long long),
                              unsigned long long>>>>>>;
};
#ifndef __CHAR_UNSIGNED__
template<> struct _make_unsigned_ll<char>
{ using type = unsigned char; };
#endif
#ifndef __WCHAR_UNSIGNED__
template<> struct _make_unsigned_ll<wchar_t>
{ using type = unsigned int; };
#endif
template<> struct _make_unsigned_ll<unsigned char>
{ using type = unsigned char; };
template<> struct _make_unsigned_ll<short>
{ using type = unsigned short; };
template<> struct _make_unsigned_ll<int>
{ using type = unsigned int; };
template<> struct _make_unsigned_ll<long>
{ using type = unsigned long; };
template<> struct _make_unsigned_ll<long long>
{ using type = unsigned long long; };
template<typename T> struct _make_unsigned_hl
{ using type = typename _make_unsigned<T>::type; };
template<typename T> struct _make_unsigned_hl<const T>
{ using type = add_const_t<typename _make_unsigned<T>::type>; };
template<typename T> struct _make_unsigned_hl<volatile T>
{ using type = add_volatile_t<typename _make_unsigned<T>::type>; };
template<typename T> struct _make_unsigned_hl<const volatile T>
{ using type = add_cv_t<typename _make_unsigned<T>::type>; };
template<typename T> struct make_unsigned
: _make_unsigned_hl<remove_reference_t<T>> {};


template<typename T> struct remove_extent { using type = T; };
template<typename T> struct remove_extent<T[]> { using type = T; };
template<typename T, size_t N> struct remove_extent<T[N]> { using type = T; };

template<typename T> struct remove_all_extents { using type = T; };
template<typename T> struct remove_all_extents<T[]>
{ using type = remove_all_extents_t<T>; };
template<typename T, size_t N> struct remove_all_extents<T[N]>
{ using type = remove_all_extents_t<T>; };


template<typename T> struct remove_pointer { using type = T; };
template<typename T> struct remove_pointer<T*> { using type = T; };
template<typename T> struct remove_pointer<T* const> { using type = T; };
template<typename T> struct remove_pointer<T* volatile> { using type = T; };
template<typename T> struct remove_pointer<T* const volatile>
{ using type = T; };

template<typename T> struct add_pointer
{ using type = remove_reference_t<T>*; };


template<size_t Len, size_t Align> struct aligned_storage {
  struct alignas(Align) type {
    unsigned char _[Len];
  };

  static_assert(is_pod<type>::value, "aligned_storage: implementation bug");
};

template<size_t Len, typename... Types> struct aligned_union {
  static constexpr size_t alignment_value =
      alignof(typename impl::combine_align<Types...>::type);
  using type = typename aligned_storage<Len, alignment_value>::type;
};

template<typename T> struct _decay {
  using type = conditional_t<is_array<T>::value,
                             remove_extent_t<T>*,
                             conditional_t<is_function<T>::value,
                                           add_pointer_t<T>,
                                           remove_cv_t<T>>>;
};
template<typename T> struct decay : _decay<remove_reference_t<T>> {};

template<bool, typename T> struct enable_if {};
template<typename T> struct enable_if<true, T> { using type = T; };

template<typename T, typename F> struct conditional<true, T, F>
{ using type = T; };
template<typename T, typename F> struct conditional<false, T, F>
{ using type = F; };

template<typename T> struct common_type<T> { using type = decay_t<T>; };
template<typename T, typename U> struct common_type<T, U>
{
 private:
  static add_rvalue_reference_t<T> vt();  // declval<T>()
  static add_rvalue_reference_t<U> vu();  // declval<U>()

 public:
  using type = decay_t<decltype(true ? vt() : vu())>;
};
template<typename T, typename U, typename... V> struct common_type<T, U, V...>
{
 public:
  using type = common_type_t<common_type_t<T, U>, V...>;
};

template<typename T> struct _underlying_type
{ using type = __underlying_type(T); };
template<typename T> struct underlying_type
: _underlying_type<remove_cv_t<T>> {};

template<typename T> class result_of;  // Not defined (specialized below).
template<typename F, typename... ArgTypes> class result_of<F(ArgTypes...)>
{ public: using type = F; };


static_assert(rank<int>::value == 0, "std::rank error");
static_assert(rank<int[2]>::value == 1, "std::rank error");
static_assert(rank<int[][2]>::value == 2, "std::rank error");

static_assert(extent<int>::value == 0, "std::extent error");
static_assert(extent<int[2]>::value == 2, "std::extent error");
static_assert(extent<int[2][4]>::value == 2, "std::extent error");
static_assert(extent<int[][4]>::value == 0, "std::extent error");
static_assert(extent<int, 1>::value == 0, "std::extent error");
static_assert(extent<int[2], 1>::value == 0, "std::extent error");
static_assert(extent<int[2][4], 1>::value == 4, "std::extent error");
static_assert(extent<int[][4], 1>::value == 4, "std::extent error");

static_assert(is_void<void>::value, "std::is_void error");
static_assert(is_void<const void>::value, "std::is_void error");
static_assert(is_void<volatile void>::value, "std::is_void error");
static_assert(is_void<const volatile void>::value, "std::is_void error");


_namespace_end(std)

#endif /* _TYPE_TRAITS_INL_H_ */
