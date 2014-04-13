#ifndef _STDIMPL_CONS_H_
#define _STDIMPL_CONS_H_

#include <cdecl.h>
#include <type_traits>
#include <utility>

_namespace_begin(std)
namespace impl {


struct ignore_t {};


template<typename T> struct cons_elem_id {};

template<size_t I, typename T, bool = is_empty<T>::value> class cons_elem {
 public:
  using disambiguator = integral_constant<size_t, I>;
  using id = cons_elem_id<remove_reference_t<T>>;

  constexpr cons_elem() : value_() {}
  constexpr cons_elem(const cons_elem&) = default;
  constexpr cons_elem(cons_elem&&) = default;
  constexpr cons_elem(const T& v) : value_(v) {}
  template<typename U>
  constexpr cons_elem(U&& v) : value_(forward<U>(v)) {}

  friend auto get_value_(disambiguator, cons_elem& self) noexcept
  -> add_lvalue_reference_t<T> {
    return self.value_;
  }

  friend auto get_value_(disambiguator, cons_elem&& self) noexcept
  -> add_rvalue_reference_t<T> {
    return static_cast<add_rvalue_reference_t<T>>(self.value_);
  }

  friend auto get_value_(disambiguator, const cons_elem& self) noexcept
  -> add_lvalue_reference_t<add_const_t<T>> {
    return self.value_;
  }

  friend auto get_value_by_type_(id, cons_elem& self) noexcept
  -> add_lvalue_reference_t<T> {
    return self.value_;
  }

  friend auto get_value_by_type_(id, cons_elem&& self) noexcept
  -> add_rvalue_reference_t<T> {
    return static_cast<add_rvalue_reference_t<T>>(self.value_);
  }

  friend auto get_value_by_type_(id, const cons_elem& self) noexcept
  -> add_lvalue_reference_t<add_const_t<T>> {
    return self.value_;
  }

 protected:
  ~cons_elem() = default;

 private:
  T value_;
};

template<size_t I, typename T> class cons_elem<I, T, true>
: private T {
 public:
  using disambiguator = integral_constant<size_t, I>;
  using id = cons_elem_id<remove_reference_t<T>>;

  constexpr cons_elem() : T() {}
  constexpr cons_elem(const T& v) : T(v) {}
  template<typename U>
  constexpr cons_elem(U&& v) : T(forward<U>(v)) {}

  friend auto get_value_(disambiguator, cons_elem& self) noexcept
  -> add_lvalue_reference_t<T> {
    return self;
  }

  friend auto get_value_(disambiguator, cons_elem&& self) noexcept
  -> add_rvalue_reference_t<T> {
    return static_cast<add_rvalue_reference_t<T>>(self);
  }

  friend auto get_value_(disambiguator, const cons_elem& self) noexcept
  -> add_lvalue_reference_t<add_const_t<T>> {
    return self;
  }

  friend auto get_value_by_type_(id, cons_elem& self) noexcept
  -> add_lvalue_reference_t<T> {
    return self;
  }

  friend auto get_value_by_type_(id, cons_elem&& self) noexcept
  -> add_rvalue_reference_t<T> {
    return static_cast<add_rvalue_reference_t<T>>(self);
  }

  friend auto get_value_by_type_(id, const cons_elem& self) noexcept
  -> add_lvalue_reference_t<add_const_t<T>> {
    return self;
  }

 protected:
  ~cons_elem() = default;
};

template<typename...> struct type_list {};

template<typename, typename> struct type_list_concat;
template<typename... L1, typename... L2>
struct type_list_concat<type_list<L1...>, type_list<L2...>> {
  using type = type_list<L1..., L2...>;
};

template<typename L1, typename L2> using type_list_concat_t =
    typename type_list_concat<L1, L2>::type;

/*
 * rm_ignore: remove all ignore_t cons_elem.
 */
template<typename, typename = type_list<>> struct _rm_ignore;

template<size_t I, typename T, typename... Ltodo, typename... Ldone>
struct _rm_ignore<type_list<cons_elem<I, T>, Ltodo...>, type_list<Ldone...>> {
  static constexpr bool skip =
      is_same<remove_cv_t<remove_reference_t<T>>, ignore_t>::value;

  using done = conditional_t<skip,
                             type_list<Ldone...>,
                             type_list_concat_t<type_list<Ldone...>,
                                                type_list<cons_elem<I, T>>>>;

  using type = typename _rm_ignore<type_list<Ltodo...>, done>::type;
};

template<typename Ldone>
struct _rm_ignore<type_list<>, Ldone> {
  using type = Ldone;
};

template<typename L> struct rm_ignore {
  using type = typename _rm_ignore<L>::type;
};

template<typename L> using rm_ignore_t = typename rm_ignore<L>::type;

/*
 * max_align: recursive selection of types with maximum alignment requirement.
 */
template<typename, typename = type_list<>, size_t = 0>
struct _max_align;

template<typename Ltodo1, typename... Ltodo, typename... Lresult, size_t Align>
struct _max_align<type_list<Ltodo1, Ltodo...>, type_list<Lresult...>, Align> {
  static constexpr size_t Align1 = alignment_of<Ltodo1>::value;
  static constexpr bool Less = Align1 > Align;

  using selection =
      conditional_t<Less,
                    type_list<Ltodo1>,
                    conditional_t<Align1 == Align,
                                  type_list_concat_t<type_list<Lresult...>,
                                                     type_list<Ltodo1>>,
                                  type_list<Lresult...>>>;
  static constexpr size_t AlignContinuation = (Less ? Align1 : Align);

  using impl = _max_align<type_list<Ltodo...>, selection, AlignContinuation>;

  using type = typename impl::type;
  static constexpr size_t alignment = impl::alignment;
};

template<typename... Lresult, size_t Align>
struct _max_align<type_list<>, type_list<Lresult...>, Align> {
  using type = type_list<Lresult...>;
  static constexpr size_t alignment = (Align == 0 ? 1U : Align);
};

template<typename> struct max_align;
template<typename... T> struct max_align<type_list<T...>> {
  using type = typename _max_align<type_list<T...>>::type;
  static constexpr size_t alignment = _max_align<type_list<T...>>::alignment;
};

template<typename L> using max_align_t = typename max_align<L>::type;


/*
 * has_type: check if list A contains type B.
 */
template<typename, typename> struct _has_type;

template<typename A0, typename... A, typename T>
struct _has_type<type_list<A0, A...>, T> {
  using type = conditional_t<is_same<A0, T>::value,
                             true_type,
                             typename _has_type<type_list<A...>, T>::type>;
};

template<typename T>
struct _has_type<type_list<>, T> {
  using type = false_type;
};

template<typename, typename> struct has_type;
template<typename... L, typename T> struct has_type<type_list<L...>, T>
: _has_type<type_list<L...>, T>::type
{};


/*
 * Set_difference: remove each element in B from A.
 */
template<typename, typename, typename = type_list<>> struct _set_difference;

template<typename A1, typename... A, typename... Exclude, typename... Out>
struct _set_difference<type_list<A1, A...>, type_list<Exclude...>,
                       type_list<Out...>> {
  using out = conditional_t<has_type<type_list<Exclude...>, A1>::value,
                            type_list<Out...>,
                            type_list_concat_t<type_list<Out...>,
                                               type_list<A1>>>;
  using type = typename _set_difference<type_list<A...>, type_list<Exclude...>,
                                        out>::type;
};

template<typename... Exclude, typename... Out>
struct _set_difference<type_list<>, type_list<Exclude...>, type_list<Out...>> {
  using type = type_list<Out...>;
};

template<typename L, typename Exclude>
struct set_difference {
  using type =
      typename _set_difference<L, Exclude>::type;
};

template<typename L, typename Exclude> using set_difference_t =
    typename set_difference<L, Exclude>::type;


/*
 * align_sort: sort list of types from highest alignment constraint to lowest.
 */
template<typename, typename = type_list<>> struct _align_sort;

template<typename... Head, typename... Out>
struct _align_sort<type_list<Head...>, type_list<Out...>> {
  using append = max_align_t<type_list<Head...>>;
  using remainder = set_difference_t<type_list<Head...>, append>;
  using out = type_list_concat_t<type_list<Out...>, append>;

  using type = typename _align_sort<remainder, out>::type;
};

template<typename... Out>
struct _align_sort<type_list<>, type_list<Out...>> {
  using type = type_list<Out...>;
};

template<typename L> struct align_sort {
  using type = typename _align_sort<L>::type;
};

template<typename L> using align_sort_t = typename align_sort<L>::type;


/*
 * cons_generator: convert a list of types to a list of cons_elem.
 */
template<typename, typename...> struct _cons_generator;

template<size_t... I, typename... Types>
struct _cons_generator<index_sequence<I...>, Types...> {
  using type = type_list<cons_elem<I, Types>...>;
};

template<typename... Types>
struct cons_generator {
  using type =
      rm_ignore_t<typename _cons_generator<index_sequence_for<Types...>,
                                           Types...>::type>;
};

template<typename... Types> using cons_generator_t =
    typename cons_generator<Types...>::type;

/*
 * sorted_cons_generator: generate a list of cons_elements for type,
 * sorted by alignment constraint in descending order.
 */
template<typename... Types> struct sorted_cons_generator {
  using type = align_sort_t<cons_generator_t<Types...>>;
};

template<typename... Types> using sorted_cons_generator_t =
   typename sorted_cons_generator<Types...>::type;


/*
 * get_value: retrieve value from element at position.
 */
template<size_t I> struct _get_value {
  template<typename T0, typename... TT>
  static auto impl(T0&&, TT&&... v)
      noexcept(noexcept(_get_value<I - 1>::impl(forward<TT>(v)...)))
  -> decltype(_get_value<I - 1>::impl(forward<TT>(v)...)) {
    return _get_value<I - 1>::impl(forward<TT>(v)...);
  }
};

template<> struct _get_value<0> {
  template<typename T0, typename... TT>
  static auto impl(T0&& v, TT&&...)
      noexcept(noexcept(forward<T0>(v)))
  -> decltype(forward<T0>(v)) {
    return forward<T0>(v);
  }
};

template<size_t I, typename... Types>
auto get_value(Types&&... v)
    noexcept(noexcept(_get_value<I>::impl(forward<Types>(v)...)))
-> decltype(_get_value<I>::impl(forward<Types>(v)...)) {
  static_assert(sizeof...(Types) > I,
                "Not enough values to extract value at index.");
  return _get_value<I>::impl(forward<Types>(v)...);
}


/*
 * cons_and: calculate a && b && c && ...
 */
constexpr bool cons_and() {
  return true;
}

template<typename B0, typename... B>
constexpr bool cons_and(B0&& b0, B&&... bn) {
  return forward<B0>(b0) && cons_and(forward<B>(bn)...);
}


/*
 * cons: the actual list of types.
 */
template<size_t, typename... Types> class cons;

template<size_t Nitems, size_t... I, typename... Type, bool... B>
class cons<Nitems, cons_elem<I, Type, B>...>
: public cons_elem<I, Type>... {
 public:
  constexpr cons() = default;
  constexpr cons(const cons&) = default;
  constexpr cons(cons&&) = default;
  cons& operator=(const cons&) = default;
  cons& operator=(cons&&) = default;

  template<typename... U> constexpr cons(const cons<Nitems, U...>& v)
      noexcept(cons_and(is_nothrow_constructible<cons_elem<I, Type>,
                        decltype(v.template get_value<I>(v))>::value...));
  template<typename... U> constexpr cons(cons<Nitems, U...>&& v)
      noexcept(cons_and(is_nothrow_constructible<cons_elem<I, Type>,
                        decltype(v.template get_value<I>(move(v)))>::value...));
  template<typename... U> constexpr cons(
      enable_if_t<sizeof...(U) == Nitems, piecewise_construct_t>,
      U&&... v)
      noexcept(cons_and(is_nothrow_constructible<cons_elem<I, Type>,
                        decltype(impl::get_value<I>(forward<U>(v)...))
                        >::value...));
  template<typename... U> cons& operator=(const cons<Nitems, U...>& v)
      noexcept(noexcept(this->assign_(v, index_sequence<I...>())));
  template<typename... U> cons& operator=(cons<Nitems, U...>&& v)
      noexcept(noexcept(this->assign_(forward<cons<Nitems, U...>>(v),
                                      index_sequence<I...>())));

  template<typename... U> bool operator==(const cons<Nitems, U...>& v) const
      noexcept(noexcept(cons_and((this->template get_value<I>(*this) ==
                                  v.template get_value<I>(v))...)));
  template<typename... U> bool operator!=(const cons<Nitems, U...>& v) const
      noexcept(noexcept(!(*this == v)));
  template<typename... U> bool operator<(const cons<Nitems, U...>& v) const
      noexcept(noexcept(this->before_(v)));
  template<typename... U> bool operator>(const cons<Nitems, U...>& v) const
      noexcept(noexcept(v < *this));
  template<typename... U> bool operator<=(const cons<Nitems, U...>& v) const
      noexcept(noexcept(!(v < *this)));
  template<typename... U> bool operator>=(const cons<Nitems, U...>& v) const
      noexcept(noexcept(!(*this < v)));

  void swap_impl(cons& v)
      noexcept(noexcept(this->swap_(v, index_sequence<I...>())));

  template<size_t Idx> static auto get_value(cons& self) noexcept
  -> decltype(get_value_(integral_constant<size_t, Idx>(), self)) {
    return get_value_(integral_constant<size_t, Idx>(), self);
  }

  template<size_t Idx> static auto get_value(cons&& self) noexcept
  -> decltype(get_value_(integral_constant<size_t, Idx>(), move(self))) {
    return get_value_(integral_constant<size_t, Idx>(), move(self));
  }

  template<size_t Idx> static auto get_value(const cons& self) noexcept
  -> decltype(get_value_(integral_constant<size_t, Idx>(), self)) {
    return get_value_(integral_constant<size_t, Idx>(), self);
  }

  template<typename Idx> static auto get_value_by_type(cons& self) noexcept
  -> decltype(get_value_by_type_(cons_elem_id<Idx>(), self)) {
    return get_value_by_type_(cons_elem_id<Idx>(), self);
  }

  template<typename Idx> static auto get_value_by_type(cons&& self) noexcept
  -> decltype(get_value_by_type_(cons_elem_id<Idx>(), move(self))) {
    return get_value_by_type_(cons_elem_id<Idx>(), move(self));
  }

  template<typename Idx> static auto get_value_by_type(const cons& self)
      noexcept
  -> decltype(get_value_by_type_(cons_elem_id<Idx>(), self)) {
    return get_value_by_type_(cons_elem_id<Idx>(), self);
  }

 private:
  template<typename... U, size_t Idx0, size_t... Idx>
  bool before_(const cons<Nitems, U...>& v,
               index_sequence<Idx0, Idx...> = make_index_sequence<Nitems>())
      const
      noexcept(noexcept(
          this->template get_value<Idx0>(*this) == v.template get_value<Idx0>(v) ?
          this->before_(v, index_sequence<Idx...>()) :
          this->template get_value<Idx0>(*this) < v.template get_value<Idx0>(v)));

  template<typename... U>
  bool before_(const cons<Nitems, U...>& v, index_sequence<>) const noexcept;

  template<typename... U, size_t Idx0, size_t... Idx>
  bool equals_(const cons<Nitems, U...>& v,
               index_sequence<Idx0, Idx...> = make_index_sequence<Nitems>())
      const
      noexcept(noexcept(
          this->template get_value<Idx0>(*this) == v.template get_value<Idx0>(v) &&
          this->equals_(v, index_sequence<Idx...>())));

  template<typename... U>
  bool equals_(const cons<Nitems, U...>& v, index_sequence<>) const noexcept;

  template<typename... U, size_t Idx0, size_t... Idx>
  void assign_(const cons<Nitems, U...>& v, index_sequence<Idx0, Idx...>)
      noexcept(noexcept(this->template get_value<Idx0>(*this) =
                        v.template get_value<Idx0>(v)) &&
               noexcept(this->assign_(v, index_sequence<Idx...>())));

  template<typename... U>
  void assign_(const cons<Nitems, U...>& v, index_sequence<>) noexcept;

  template<typename... U, size_t Idx0, size_t... Idx>
  void assign_(cons<Nitems, U...>&& v, index_sequence<Idx0, Idx...>)
      noexcept(noexcept(this->template get_value<Idx0>(*this) =
                        v.template get_value<Idx0>(move(v))) &&
               noexcept(this->assign_(forward<cons<Nitems, U...>>(v),
                                      index_sequence<Idx...>())));

  template<typename... U>
  void assign_(cons<Nitems, U...>&& v, index_sequence<>) noexcept;

  template<size_t Idx0, size_t... Idx>
  void swap_(cons& v, index_sequence<Idx0, Idx...>)
      noexcept(noexcept(swap(this->template get_value<Idx0>(*this),
                             v.template get_value<Idx0>(v))) &&
               noexcept(this->swap_(v, index_sequence<Idx...>())));

  void swap_(cons& v, index_sequence<>) noexcept;
};

template<size_t Nitems, typename L> struct sorted_cons_selector;

template<size_t Nitems, typename... LI>
struct sorted_cons_selector<Nitems, type_list<LI...>> {
  using type = cons<Nitems, LI...>;
};

template<typename... Types> using sorted_cons =
    typename sorted_cons_selector<sizeof...(Types),
                                  sorted_cons_generator_t<Types...>>::type;


} /* namespace std::impl */
_namespace_end(std)

#include <stdimpl/cons-inl.h>

#endif /* _STDIMPL_CONS_H_ */
