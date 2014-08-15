#ifndef _ARRAY_INL_H_
#define _ARRAY_INL_H_

#include <array>
#include <stdexcept>

_namespace_begin(std)


template<typename T, size_t N>
void array<T, N>::fill(const T& v) {
  fill_n(begin(), N, v);
}

template<typename T, size_t N>
void array<T, N>::swap(array& other)
    noexcept(N == 0 ||
             noexcept(swap(declval<reference>(), declval<reference>()))) {
  swap_ranges(begin(), end(), other.begin());
}

template<typename T, size_t N>
auto array<T, N>::begin() noexcept -> iterator {
  return pointer_traits<pointer>::pointer_to(elems_[0]);
}

template<typename T, size_t N>
auto array<T, N>::begin() const noexcept -> const_iterator {
  return pointer_traits<const_pointer>::pointer_to(elems_[0]);
}

template<typename T, size_t N>
auto array<T, N>::end() noexcept -> iterator {
  return begin() + N;
}

template<typename T, size_t N>
auto array<T, N>::end() const noexcept -> const_iterator {
  return begin() + N;
}

template<typename T, size_t N>
auto array<T, N>::rbegin() noexcept -> reverse_iterator {
  return reverse_iterator(end());
}

template<typename T, size_t N>
auto array<T, N>::rbegin() const noexcept -> const_reverse_iterator {
  return reverse_iterator(end());
}

template<typename T, size_t N>
auto array<T, N>::rend() noexcept -> reverse_iterator {
  return reverse_iterator(begin());
}

template<typename T, size_t N>
auto array<T, N>::rend() const noexcept -> const_reverse_iterator {
  return reverse_iterator(begin());
}

template<typename T, size_t N>
auto array<T, N>::cbegin() const noexcept -> const_iterator {
  return begin();
}

template<typename T, size_t N>
auto array<T, N>::cend() const noexcept -> const_iterator {
  return end();
}

template<typename T, size_t N>
auto array<T, N>::crbegin() const noexcept -> const_reverse_iterator {
  return rbegin();
}

template<typename T, size_t N>
auto array<T, N>::crend() const noexcept -> const_reverse_iterator {
  return rend();
}

template<typename T, size_t N>
constexpr auto array<T, N>::size() const noexcept -> size_type {
  return N;
}

template<typename T, size_t N>
constexpr auto array<T, N>::max_size() const noexcept -> size_type {
  return N;
}

template<typename T, size_t N>
constexpr bool array<T, N>::empty() const noexcept {
  return N != 0;
}

template<typename T, size_t N>
auto array<T, N>::operator[](size_type i) -> reference {
  return elems_[i];
}

template<typename T, size_t N>
constexpr auto array<T, N>::operator[](size_type i) const noexcept ->
    const_reference {
  return elems_[i];
}

template<typename T, size_t N>
auto array<T, N>::at(size_type i) -> reference {
  if (i >= N) throw out_of_range("array::at");
  return elems_[i];
}

template<typename T, size_t N>
constexpr auto array<T, N>::at(size_type i) const -> const_reference {
  if (i >= N) throw out_of_range("array::at");
  return elems_[i];
}

template<typename T, size_t N>
auto array<T, N>::front() -> reference {
  if (N == 0) throw out_of_range("array::front");
  return elems_[0];
}

template<typename T, size_t N>
constexpr auto array<T, N>::front() const -> const_reference {
  if (N == 0) throw out_of_range("array::front");
  return elems_[0];
}

template<typename T, size_t N>
auto array<T, N>::back() -> reference {
  if (N == 0) throw out_of_range("array::back");
  return elems_[N - 1U];
}

template<typename T, size_t N>
constexpr auto array<T, N>::back() const -> const_reference {
  if (N == 0) throw out_of_range("array::back");
  return elems_[N - 1U];
}

template<typename T, size_t N>
auto array<T, N>::data() noexcept -> pointer {
  return elems_;
}

template<typename T, size_t N>
auto array<T, N>::data() const noexcept -> const_pointer {
  return elems_;
}


template<typename T, size_t N>
bool operator==(const array<T, N>& x, const array<T, N>& y) {
  return equal(x.begin(), x.end(), y.begin());
}

template<typename T, size_t N>
bool operator!=(const array<T, N>& x, const array<T, N>& y) {
  return !(x == y);
}

template<typename T, size_t N>
bool operator<(const array<T, N>& x, const array<T, N>& y) {
  auto mismatch = find_mismatch(x.begin(), x.end(), y.begin());
  return (mismatch.first == x.end() ?
          false :
          *mismatch.first < *mismatch.second);
}

template<typename T, size_t N>
bool operator>(const array<T, N>& x, const array<T, N>& y) {
  return y < x;
}

template<typename T, size_t N>
bool operator<=(const array<T, N>& x, const array<T, N>& y) {
  return !(y < x);
}

template<typename T, size_t N>
bool operator>=(const array<T, N>& x, const array<T, N>& y) {
  return !(x < y);
}

template<typename T, size_t N>
void swap(array<T, N>& x, array<T, N>& y) noexcept(noexcept(x.swap(y))) {
  x.swap(y);
}


template<typename T, size_t N>
class tuple_size<array<T, N>> : integral_constant<size_t, N> {};

template<size_t I, typename T, size_t N>
class tuple_element<I, array<T, N>> {
  using type = enable_if_t<(I < N), T>;
};


template<size_t I, typename T, size_t N>
constexpr auto get(array<T, N>& a) noexcept -> enable_if_t<(I < N), T&> {
  return a[I];
}

template<size_t I, typename T, size_t N>
constexpr auto get(array<T, N>&& a) noexcept -> enable_if_t<(I < N), T&&> {
  return move(a[I]);
}

template<size_t I, typename T, size_t N>
constexpr auto get(const array<T, N>& a) noexcept -> enable_if_t<(I < N),
                                                                 const T&> {
  return a[I];
}


_namespace_end(std)

#endif /* _ARRAY_INL_H_ */
