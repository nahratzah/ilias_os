#ifndef _TUPLE_INL_H_
#define _TUPLE_INL_H_

#include <tuple>

_namespace_begin(std)


template<typename... Types>
constexpr tuple<Types...>::tuple()
: cons()
{}

template<typename... Types>
constexpr tuple<Types...>::tuple(const Types&... v)
: cons(piecewise_construct_t(), v...)
{}

template<typename... Types>
template<typename... UTypes, typename>
constexpr tuple<Types...>::tuple(UTypes&&... v)
: cons(piecewise_construct_t(), forward<UTypes>(v)...)
{}

template<typename... Types>
template<typename... UTypes, typename>
constexpr tuple<Types...>::tuple(const tuple<UTypes...>& v)
: cons(v)
{}

template<typename... Types>
template<typename... UTypes, typename>
constexpr tuple<Types...>::tuple(tuple<UTypes...>&& v)
: cons(forward<tuple<UTypes...>>(v))
{}

template<typename... Types>
template<typename U1, typename U2>
constexpr tuple<Types...>::tuple(const pair<U1, U2>& v,
    enable_if_t<implicit_convertible<U1, U2>::value, enable_>)
: cons(v.first, v.second)
{}

template<typename... Types>
template<typename U1, typename U2>
constexpr tuple<Types...>::tuple(pair<U1, U2>&& v,
    enable_if_t<implicit_convertible<U1, U2>::value, enable_>)
: cons(forward<U1>(v.first), forward<U2>(v.second))
{}

template<typename... Types>
template<typename... UTypes>
auto tuple<Types...>::operator=(const tuple<UTypes...>& u) -> tuple& {
  this->cons::operator=(u);
  return *this;
}

template<typename... Types>
template<typename... UTypes>
auto tuple<Types...>::operator=(tuple<UTypes...>&& u) -> tuple& {
  this->cons::operator=(forward<tuple<UTypes...>>(u));
  return *this;
}

template<typename... Types>
template<typename U1, typename U2>
auto tuple<Types...>::operator=(const pair<U1, U2>& u) -> tuple& {
  static_assert(sizeof...(Types) == 2,
                "Assigning std::pair to std::tuple requires "
                "tuple to have exactly two elements.");
  return *this = tuple<const U1&, const U2&>(u.first, u.second);
}

template<typename... Types>
template<typename U1, typename U2>
auto tuple<Types...>::operator=(pair<U1, U2>&& u) -> tuple& {
  static_assert(sizeof...(Types) == 2,
                "Assigning std::pair to std::tuple requires "
                "tuple to have exactly two elements.");
  return *this = tuple<U1, U2>(forward<U1>(u.first), forward<U2>(u.second));
}

template<typename... Types>
auto tuple<Types...>::swap(tuple& rhs)
    noexcept(noexcept(this->cons::swap_impl(rhs)))
-> void {
  this->cons::swap_impl(rhs);
}


template<typename... Types>
constexpr auto make_tuple(Types&&... v)
-> tuple<typename impl::refwrap<Types>::type...> {
  return tuple<typename impl::refwrap<Types>::type...>(forward<Types>(v)...);
}

template<typename... Types>
constexpr auto forward_as_tuple(Types&&... v) noexcept -> tuple<Types&&...> {
  return tuple<Types&&...>(forward<Types>(v)...);
}

template<typename... Types>
constexpr auto tie(Types&... v) noexcept -> tuple<Types&...> {
  return tuple<Types&...>(v...);
}


namespace impl {

template<typename... T> struct tuple_cat_list;

template<typename... T>
struct tuple_cat_list_combine;

template<> struct tuple_cat_list_combine<tuple_cat_list<>> {
  using type = tuple_cat_list<>;
};

template<typename... T> struct tuple_cat_list_combine<tuple_cat_list<T...>> {
  using type = tuple_cat_list<T...>;
};

template<typename... T, typename... U, typename... Tail>
struct tuple_cat_list_combine<tuple_cat_list<T...>, tuple_cat_list<U...>,
                              Tail...> {
  using type = typename tuple_cat_list_combine<tuple_cat_list<T..., U...>,
                                               Tail...>::type;
};

template<typename T, typename = make_index_sequence<tuple_size<T>::value>>
struct tuple_cat_t0;

template<typename T, size_t... I>
struct tuple_cat_t0<T, index_sequence<I...>> {
  using type = tuple_cat_list<typename tuple_element<I, T>::type...>;
};

template<typename... T>
struct tuple_cat_t {
  using type =
      typename tuple_cat_list_combine<typename tuple_cat_t0<T>::type...>::type;
};

template<typename T> struct tuple_cat_result;
template<typename... T> struct tuple_cat_result<tuple_cat_list<T...>> {
  using type = tuple<remove_reference_t<T>...>;
};

template<typename... Tuples> using tuple_cat_result_t =
    typename tuple_cat_result<typename tuple_cat_t<Tuples...>::type>::type;


template<typename T0, size_t... I0>
auto expand0(T0&& t0,
             const index_sequence<I0...>&) noexcept
-> decltype(forward_as_tuple(get<I0>(t0)...)) {
  return forward_as_tuple(get<I0>(t0)...);
}

template<typename T0, size_t... I0, typename T1, size_t... I1>
auto expand0(T0&& t0, T1&& t1,
             const index_sequence<I0...>&,
             const index_sequence<I1...>&) noexcept
-> decltype(forward_as_tuple(get<I0>(t0)..., get<I1>(t1)...)) {
  return forward_as_tuple(get<I0>(t0)..., get<I1>(t1)...);
}

template<typename T0>
auto expand(T0&& t0) noexcept
-> decltype(expand0(forward<T0>(t0),
                    make_index_sequence<tuple_size<T0>::value>())) {
  return expand0(forward<T0>(t0),
                 make_index_sequence<tuple_size<T0>::value>());
}

template<typename T0, typename T1>
auto expand(T0&& t0, T1&& t1) noexcept
-> decltype(expand0(forward<T0>(t0), forward<T1>(t1),
                    make_index_sequence<tuple_size<T0>::value>(),
                    make_index_sequence<tuple_size<T1>::value>())) {
  return expand0(forward<T0>(t0), forward<T1>(t1),
                 make_index_sequence<tuple_size<T0>::value>(),
                 make_index_sequence<tuple_size<T1>::value>());
}

inline auto merge() noexcept -> tuple<> { return tuple<>(); }

template<typename T0>
auto merge(T0&& t0) noexcept
-> decltype(expand(forward<T0>(t0))) {
  return expand(forward<T0>(t0));
}

template<typename T0, typename T1, typename... TT>
auto merge(T0&& t0, T1&& t1, TT&&... tt) noexcept
-> decltype(merge(expand(forward<T0>(t0), forward<T1>(t1)),
                  forward<TT>(tt)...))
{
  return merge(expand(forward<T0>(t0), forward<T1>(t1)),
               forward<TT>(tt)...);
}


} /* namespace std::impl */


template<typename... Tuples>
constexpr auto tuple_cat(Tuples&&... v)
#if 0
    noexcept(noexcept(impl::tuple_cat_result_t<Tuples...>(
        impl::merge(forward<Tuples>(v)...))))
#endif
-> impl::tuple_cat_result_t<Tuples...> {
  return impl::tuple_cat_result_t<Tuples...>(
      impl::merge(forward<Tuples>(v)...));
}


template<typename T>
class tuple_size<T&>
: public tuple_size<T> {};

template<typename T>
class tuple_size<T&&>
: public tuple_size<T> {};

template<typename... Types>
class tuple_size<tuple<Types...>>
: public integral_constant<size_t, sizeof...(Types)> {};


template<size_t I, class T> class tuple_element<I, const T>
{ public: using type = add_const_t<typename tuple_element<I, T>::type>; };

template<size_t I, class T> class tuple_element<I, volatile T>
{ public: using type = add_volatile_t<typename tuple_element<I, T>::type>; };

template<size_t I, class T> class tuple_element<I, const volatile T>
{ public: using type = add_cv_t<typename tuple_element<I, T>::type>; };

template<size_t I, class T> class tuple_element<I, T&>
: public tuple_element<I, T> {};

template<size_t I, class T> class tuple_element<I, T&&>
: public tuple_element<I, T> {};

namespace impl {
template<size_t I, typename T0, typename... Types> struct select_tuple_element
: select_tuple_element<I - 1, Types...> {};
template<typename T0, typename... Types>
struct select_tuple_element<0, T0, Types...> { using type = T0; };
} /* namespace std::impl */

template<size_t I, typename... Types> class tuple_element<I, tuple<Types...>> {
  static_assert(I < sizeof...(Types), "Tuple element index out of bounds.");

 public:
  using type = typename impl::select_tuple_element<I, Types...>::type;
};


template<size_t I, typename... Types>
constexpr auto get(tuple<Types...>& t) noexcept
-> add_lvalue_reference_t<typename tuple_element<I, tuple<Types...>>::type> {
  return t.template get_value<I>(t);
}

template<size_t I, typename... Types>
constexpr auto get(tuple<Types...>&& t) noexcept
-> add_rvalue_reference_t<typename tuple_element<I, tuple<Types...>>::type> {
  return t.template get_value<I>(move(t));
}

template<size_t I, typename... Types>
constexpr auto get(const tuple<Types...>& t) noexcept
-> add_lvalue_reference_t<
       typename tuple_element<I, const tuple<Types...>>::type> {
  return t.template get_value<I>(t);
}


template<typename I, typename... Types>
constexpr auto get(tuple<Types...>& t) noexcept
-> add_lvalue_reference_t<I> {
  return t.template get_value_by_type<I>(t);
}

template<typename I, typename... Types>
constexpr auto get(tuple<Types...>&& t) noexcept
-> add_rvalue_reference_t<I> {
  return t.template get_value_by_type<I>(move(t));
}

template<typename I, typename... Types>
constexpr auto get(const tuple<Types...>& t) noexcept
-> add_lvalue_reference_t<add_const_t<I>> {
  return t.template get_value_by_type<I>();
}


template<typename... Types>
void swap(tuple<Types...>& lhs, tuple<Types...>& rhs)
    noexcept(noexcept(lhs.swap(rhs))) {
  lhs.swap(rhs);
}


_namespace_end(std)

#endif /* _TUPLE_INL_H_ */
