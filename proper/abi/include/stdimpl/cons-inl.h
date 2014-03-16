_namespace_begin(std)
namespace impl {


template<size_t Nitems, size_t... I, typename... Type, bool... B>
template<typename... U>
constexpr cons<Nitems, cons_elem<I, Type, B>...>::cons(
    const cons<Nitems, U...>& v)
    noexcept(cons_and(is_nothrow_constructible<cons_elem<I, Type, B>,
                        decltype(v.template get_value<I>())>::value...))
: cons_elem<I, Type, B>(v.template get_value<I>())...
{}

template<size_t Nitems, size_t... I, typename... Type, bool... B>
template<typename... U>
constexpr cons<Nitems, cons_elem<I, Type, B>...>::cons(cons<Nitems, U...>&& v)
    noexcept(cons_and(is_nothrow_constructible<cons_elem<I, Type, B>,
                        decltype(move(v.template get_value<I>()))>::value...))
: cons_elem<I, Type, B>(move(v.template get_value<I>()))...
{}

template<size_t Nitems, size_t... I, typename... Type, bool... B>
template<typename... U>
constexpr cons<Nitems, cons_elem<I, Type, B>...>::cons(
    enable_if_t<sizeof...(U) == Nitems, piecewise_construct_t>,
    U&&... v)
    noexcept(cons_and(is_nothrow_constructible<cons_elem<I, Type, B>,
                        decltype(impl::get_value<I>(std::forward<U>(v)...))
                        >::value...))
: cons_elem<I, Type, B>(impl::get_value<I>(std::forward<U>(v)...))...
{}

template<size_t Nitems, size_t... I, typename... Type, bool... B>
template<typename... U>
auto cons<Nitems, cons_elem<I, Type, B>...>::operator=(
    const cons<Nitems, U...>& v)
    noexcept(noexcept(this->assign_(v)))
-> cons& {
  this->assign_(v);
  return *this;
}

template<size_t Nitems, size_t... I, typename... Type, bool... B>
template<typename... U>
auto cons<Nitems, cons_elem<I, Type, B>...>::operator=(cons<Nitems, U...>&& v)
    noexcept(noexcept(this->asign_(forward<cons<Nitems, U...>>(v))))
-> cons& {
  this->assign_(forward<cons<Nitems, U...>>(v));
  return *this;
}

template<size_t Nitems, size_t... I, typename... Type, bool... B>
template<typename... U>
auto cons<Nitems, cons_elem<I, Type, B>...>::operator==(
    const cons<Nitems, U...>& v) const
    noexcept(noexcept(this->equals_(v)))
-> bool {
  return this->equals_(v);
}

template<size_t Nitems, size_t... I, typename... Type, bool... B>
template<typename... U>
auto cons<Nitems, cons_elem<I, Type, B>...>::operator!=(
    const cons<Nitems, U...>& v) const
    noexcept(noexcept(!(*this == v)))
-> bool {
  return !(*this == v);
}

template<size_t Nitems, size_t... I, typename... Type, bool... B>
template<typename... U>
auto cons<Nitems, cons_elem<I, Type, B>...>::operator<(
    const cons<Nitems, U...>& v) const
    noexcept(noexcept(this->before_(v)))
-> bool {
  return this->before_(v);
}

template<size_t Nitems, size_t... I, typename... Type, bool... B>
template<typename... U>
auto cons<Nitems, cons_elem<I, Type, B>...>::operator>(
    const cons<Nitems, U...>& v) const
    noexcept(noexcept(v < *this))
-> bool {
  return v < *this;
}

template<size_t Nitems, size_t... I, typename... Type, bool... B>
template<typename... U>
auto cons<Nitems, cons_elem<I, Type, B>...>::operator<=(
    const cons<Nitems, U...>& v) const
    noexcept(noexcept(!(v < *this)))
-> bool {
  return !(v < *this);
}

template<size_t Nitems, size_t... I, typename... Type, bool... B>
template<typename... U>
auto cons<Nitems, cons_elem<I, Type, B>...>::operator>=(
    const cons<Nitems, U...>& v) const
    noexcept(noexcept(!(v < *this)))
-> bool {
  return !(*this < v);
}

template<size_t Nitems, size_t... I, typename... Type, bool... B>
auto cons<Nitems, cons_elem<I, Type, B>...>::swap_impl(cons& v)
  noexcept(noexcept(this->swap_(v)))
-> void {
  this->swap_(v);
}

template<size_t Nitems, size_t... I, typename... Type, bool... B>
template<typename... U, size_t Idx0, size_t... Idx>
auto cons<Nitems, cons_elem<I, Type, B>...>::before_(
    const cons<Nitems, U...>& v, index_sequence<Idx0, Idx...>) const
    noexcept(noexcept(
        this->template get_value<Idx0>() == v.template get_value<Idx0>() ?
        this->before_(v, index_sequence<Idx...>()) :
        this->template get_value<Idx0>() < v.template get_value<Idx0>()))
-> bool {
  return (this->template get_value<Idx0>() == v.template get_value<Idx0>() ?
          this->before_(v, index_sequence<Idx...>()) :
          this->template get_value<Idx0>() < v.template get_value<Idx0>());
}

template<size_t Nitems, size_t... I, typename... Type, bool... B>
template<typename... U>
auto cons<Nitems, cons_elem<I, Type, B>...>::before_(
    const cons<Nitems, U...>& v, index_sequence<>) const noexcept
-> bool {
  return false;
}


template<size_t Nitems, size_t... I, typename... Type, bool... B>
template<typename... U, size_t Idx0, size_t... Idx>
auto cons<Nitems, cons_elem<I, Type, B>...>::equals_(
    const cons<Nitems, U...>& v, index_sequence<Idx0, Idx...>) const
    noexcept(noexcept(
        this->template get_value<Idx0>() == v.template get_value<Idx0>() &&
        this->equals_(v, index_sequence<Idx...>())))
-> bool {
  return (this->template get_value<Idx0>() == v.template get_value<Idx0>() &&
          this->equals_(v, index_sequence<Idx...>()));
}

template<size_t Nitems, size_t... I, typename... Type, bool... B>
template<typename... U>
auto cons<Nitems, cons_elem<I, Type, B>...>::equals_(
    const cons<Nitems, U...>& v, index_sequence<>) const noexcept
-> bool {
  return true;
}


template<size_t Nitems, size_t... I, typename... Type, bool... B>
template<typename... U, size_t Idx0, size_t... Idx>
auto cons<Nitems, cons_elem<I, Type, B>...>::assign_(
    const cons<Nitems, U...>& v, index_sequence<Idx0, Idx...>)
    noexcept(noexcept(this->template get_value<Idx0>() =
                      v.template get_value<Idx0>()) &&
             noexcept(this->assign_(v, index_sequence<Idx...>())))
-> void {
  this->template get_value<Idx0>() = v.template get_value<Idx0>();
  this->assign_(v, index_sequence<Idx...>());
}

template<size_t Nitems, size_t... I, typename... Type, bool... B>
template<typename... U>
auto cons<Nitems, cons_elem<I, Type, B>...>::assign_(
    const cons<Nitems, U...>& v, index_sequence<>) noexcept
-> void
{}


template<size_t Nitems, size_t... I, typename... Type, bool... B>
template<typename... U, size_t Idx0, size_t... Idx>
auto cons<Nitems, cons_elem<I, Type, B>...>::assign_(
    cons<Nitems, U...>&& v, index_sequence<Idx0, Idx...>)
    noexcept(noexcept(this->template get_value<Idx0>() =
                      move(v.template get_value<Idx0>())) &&
             noexcept(this->assign_(forward<cons<Nitems, U...>>(v),
                                    index_sequence<Idx...>())))
-> void {
  this->template get_value<Idx0>() = move(v.template get_value<Idx0>());
  this->assign_(forward<cons<Nitems, U...>>(v), index_sequence<Idx...>());
}

template<size_t Nitems, size_t... I, typename... Type, bool... B>
template<typename... U>
auto cons<Nitems, cons_elem<I, Type, B>...>::assign_(
    cons<Nitems, U...>&& v, index_sequence<>) noexcept
-> void
{}


template<size_t Nitems, size_t... I, typename... Type, bool... B>
template<size_t Idx0, size_t... Idx>
auto cons<Nitems, cons_elem<I, Type, B>...>::swap_(
    cons& v, index_sequence<Idx0, Idx...>)
    noexcept(noexcept(swap(this->template get_value<Idx0>(),
                           v.template get_value<Idx0>())) &&
             noexcept(this->swap_(v, index_sequence<Idx...>())))
-> void {
  swap(this->template get_value<Idx0>(), v.template get_value<Idx0>());
  this->swap_(v, index_sequence<Idx...>());
}

template<size_t Nitems, size_t... I, typename... Type, bool... B>
auto cons<Nitems, cons_elem<I, Type, B>...>::swap_(
    cons& v, index_sequence<>) noexcept
-> void
{}


} /* namespace std::impl */
_namespace_end(std)