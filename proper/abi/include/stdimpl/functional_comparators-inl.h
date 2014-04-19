_namespace_begin(std)


template<typename T, typename U>
auto equal_to<void>::operator()(T&& x, U&& y) const ->
    decltype(forward<T>(x) == forward<U>(y)) {
  return forward<T>(x) == forward<U>(y);
}

template<typename T, typename U>
auto not_equal_to<void>::operator()(T&& x, U&& y) const ->
    decltype(forward<T>(x) != forward<U>(y)) {
  return forward<T>(x) != forward<U>(y);
}

template<typename T, typename U>
auto greater<void>::operator()(T&& x, U&& y) const ->
    decltype(forward<T>(x) > forward<U>(y)) {
  return forward<T>(x) > forward<U>(y);
}

template<typename T, typename U>
auto less<void>::operator()(T&& x, U&& y) const ->
    decltype(forward<T>(x) < forward<U>(y)) {
  return forward<T>(x) < forward<U>(y);
}

template<typename T, typename U>
auto greater_equal<void>::operator()(T&& x, U&& y) const ->
    decltype(forward<T>(x) >= forward<U>(y)) {
  return forward<T>(x) >= forward<U>(y);
}

template<typename T, typename U>
auto less_equal<void>::operator()(T&& x, U&& y) const ->
    decltype(forward<T>(x) <= forward<U>(y)) {
  return forward<T>(x) <= forward<U>(y);
}


_namespace_end(std)
