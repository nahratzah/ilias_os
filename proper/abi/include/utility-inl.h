_namespace_begin(std)
namespace rel_ops {


template<typename T> bool operator!=(const T& a, const T& b)
    noexcept(noexcept(!(a == b))) {
  return !(a == b);
}

template<typename T> bool operator>(const T& a, const T& b)
    noexcept(noexcept(b < a)) {
  return b < a;
}

template<typename T> bool operator<=(const T& a, const T& b)
    noexcept(noexcept(!(a > b))) {
  return !(b < a);
}

template<typename T> bool operator>=(const T& a, const T& b)
    noexcept(noexcept(!(a < b))) {
  return !(a < b);
}


} /* namespace std::rel_ops */


template<typename T> void swap(T& a, T& b)
    noexcept(is_nothrow_move_constructible<T>::value &&
             is_nothrow_move_assignable<T>::value) {
  T tmp = move(a);
  a = move(b);
  b = move(tmp);
}

template<typename T, size_t N> void swap(T (&a)[N], T (&b)[N])
    noexcept(noexcept(swap(*a, *b))) {
  for (size_t i = 0; i < N; ++i) swap(a[i], b[i]);
}

template<typename T, typename U> T exchange(T& obj, U&& new_val) {
  T old_val = move(obj);
  obj = forward<U>(new_val);
  return old_val;
}

template<typename T> constexpr T&& forward(remove_reference_t<T>& t) noexcept {
  return static_cast<T&&>(t);
}
template<typename T> constexpr T&& forward(remove_reference_t<T>&& t) noexcept {
  return static_cast<T&&>(t);
}

template<typename T> constexpr remove_reference_t<T>&& move(T&& t) noexcept {
  return static_cast<remove_reference_t<T>&&>(t);
}

template<typename T>
constexpr conditional_t<!is_nothrow_move_constructible<T>::value &&
                        is_copy_constructible<T>::value,
                        const T&, T&&>
move_if_noexcept(T& x) noexcept {
  return move(x);
}


template<typename T1, typename T2>
constexpr bool operator==(const pair<T1, T2>& a, const pair<T1, T2>& b) {
  return a.first == b.first && a.second == b.second;
}

template<typename T1, typename T2>
constexpr bool operator<(const pair<T1, T2>& a, const pair<T1, T2>& b) {
  return a.first < b.first || (!(b.first < a.first) && a.second < b.second);
}

template<typename T1, typename T2>
constexpr bool operator!=(const pair<T1, T2>& a, const pair<T1, T2>& b) {
  return !(a == b);
}

template<typename T1, typename T2>
constexpr bool operator>(const pair<T1, T2>& a, const pair<T1, T2>& b) {
  return b < a;
}

template<typename T1, typename T2>
constexpr bool operator<=(const pair<T1, T2>& a, const pair<T1, T2>& b) {
  return !(b < a);
}

template<typename T1, typename T2>
constexpr bool operator>=(const pair<T1, T2>& a, const pair<T1, T2>& b) {
  return !(a < b);
}

template<typename T1, typename T2>
void swap(pair<T1, T2>& x, pair<T1, T2>& y) noexcept(noexcept(x.swap(y))) {
  x.swap(y);
}

template<typename T1, typename T2>
constexpr auto make_pair(T1&& f, T2&& s) ->
    pair<impl::pair_elem_t<T1>, impl::pair_elem_t<T2>> {
  return pair<impl::pair_elem_t<T1>, impl::pair_elem_t<T2>>(forward<T1>(f),
                                                            forward<T2>(s));
}


template<typename T1, typename T2> struct tuple_size<pair<T1, T2>>
: integral_constant<size_t, 2> {};
template<typename T1, typename T2> struct tuple_element<0, pair<T1, T2>>
{ using type = T1; };
template<typename T1, typename T2> struct tuple_element<1, pair<T1, T2>>
{ using type = T2; };

template<size_t I, class T1, class T2>
constexpr auto get(pair<T1, T2>& p) noexcept ->
    typename tuple_element<I, pair<T1, T2>>::type& {
  return impl::pair_getter<I>::get(p);
}

template<size_t I, class T1, class T2>
constexpr auto get(pair<T1, T2>&& p) noexcept ->
    typename tuple_element<I, pair<T1, T2>>::type&& {
  return impl::pair_getter<I>::get(move(p));
}

template<size_t I, class T1, class T2>
constexpr auto get(const pair<T1, T2>& p) noexcept ->
    const typename tuple_element<I, pair<T1, T2>>::type& {
  return impl::pair_getter<I>::get(p);
}

template<class T1, class T2> T1& get(pair<T1, T2>& p) noexcept {
  return get<0>(p);
}

template<class T1, class T2> const T1& get(const pair<T1, T2>& p)
    noexcept {
  return get<0>(p);
}

template<class T1, class T2> T1&& get(pair<T1, T2>&& p) noexcept {
  return get<0>(move(p));
}

template<class T1, class T2> T2& get(pair<T1, T2>& p) noexcept {
  return get<1>(p);
}

template<class T1, class T2> const T2& get(const pair<T1, T2>& p)
    noexcept {
  return get<1>(p);
}

template<class T1, class T2> T2&& get(pair<T1, T2>&& p) noexcept {
  return get<1>(move(p));
}


template<typename T, T... V> struct integer_sequence {
  static_assert(is_integral<T>::value, "requirement on integer_sequence<T>: "
                                       "T shall be an integer type");

  using type = T;
  static constexpr size_t size() noexcept { return sizeof...(V); }
};


template<typename T1, typename T2>
constexpr pair<T1, T2>::pair() {}

template<typename T1, typename T2>
constexpr pair<T1, T2>::pair(const first_type& f, const second_type& s)
: first(f), second(s) {}

template<typename T1, typename T2>
template<typename U, typename V>
constexpr pair<T1, T2>::pair(U&& f, V&& s)
: first(forward<U>(f)), second(forward<V>(s)) {}

template<typename T1, typename T2>
template<typename U, typename V>
constexpr pair<T1, T2>::pair(const pair<U, V>& o)
: pair(o.first, o.second) {}

template<typename T1, typename T2>
template<typename U, typename V>
constexpr pair<T1, T2>::pair(pair<U, V>&& o)
: pair(forward<U>(o.first), forward<V>(o.second)) {}

template<typename T1, typename T2>
template<typename... Args1, typename... Args2>
pair<T1, T2>::pair(piecewise_construct_t,
                   tuple<Args1...> f, tuple<Args2...> s)
: pair(piecewise_construct, move(f), move(s),
       index_sequence_for<Args1...>(), index_sequence_for<Args2...>())
{}

template<typename T1, typename T2>
template<typename F, typename S, size_t... I, size_t... J>
pair<T1, T2>::pair(piecewise_construct_t, F&& f, S&& s,
                   index_sequence<I...>,
                   index_sequence<J...>)
: first(get<I>(forward<F>(f))...),
  second(get<J>(forward<S>(s))...)
{}

template<typename T1, typename T2>
template<typename U, typename V>
auto pair<T1, T2>::operator=(const pair<U, V>& o) -> pair& {
  return *this = pair(o);
}

template<typename T1, typename T2>
auto pair<T1, T2>::operator=(pair&& o)
    noexcept(is_nothrow_move_assignable<first_type>::value &&
             is_nothrow_move_assignable<second_type>::value) -> pair& {
  first = forward<first_type>(o.first);
  second = forward<second_type>(o.second);
  return *this;
}

template<typename T1, typename T2>
template<typename U, typename V>
auto pair<T1, T2>::operator=(pair<U, V>&& o) -> pair& {
  first = forward<U>(o.first);
  second = forward<V>(o.second);
  return *this;
}

template<typename T1, typename T2>
void pair<T1, T2>::swap(pair& o)
    noexcept(noexcept(swap(first, o.first)) &&
             noexcept(swap(second, o.second))) {
  using _namespace(std)::swap;

  swap(first, o.first);
  swap(second, o.second);
}


_namespace_end(std)
