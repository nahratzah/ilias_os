_namespace_begin(std)


template<typename Iterator>
reverse_iterator<Iterator>::reverse_iterator(iterator_type i)
  noexcept(is_nothrow_copy_constructible<iterator_type>::value)
: i_(i)
{}

template<typename Iterator>
template<typename U>
reverse_iterator<Iterator>::reverse_iterator(const reverse_iterator<U>& o)
: i_(o.base())
{}

template<typename Iterator>
template<typename U>
auto reverse_iterator<Iterator>::operator=(const reverse_iterator<U>& o) ->
    reverse_iterator& {
  i_ = o.base();
  return *this;
}

template<typename Iterator>
auto reverse_iterator<Iterator>::base() const
    noexcept(is_nothrow_copy_constructible<iterator_type>::value) ->
    iterator_type
{
  return i_;
}

template<typename Iterator>
auto reverse_iterator<Iterator>::operator*() const -> reference {
  auto i = i_;
  return *--i;
}

template<typename Iterator>
auto reverse_iterator<Iterator>::operator->() const -> pointer {
  auto i = i_;
  return &*i_;
}

template<typename Iterator>
auto reverse_iterator<Iterator>::operator[](difference_type n) const
    noexcept(declval<iterator_type>()[0]) ->
    decltype(declval<iterator_type>()[0]) {
  return i_[-n - 1];
}

template<typename Iterator>
auto reverse_iterator<Iterator>::operator++() ->
    reverse_iterator& {
  --i_;
  return *this;
}

template<typename Iterator>
auto reverse_iterator<Iterator>::operator--() ->
    reverse_iterator& {
  ++i_;
  return *this;
}

template<typename Iterator>
auto reverse_iterator<Iterator>::operator++(int) ->
    reverse_iterator {
  return reverse_iterator(i_--);
}

template<typename Iterator>
auto reverse_iterator<Iterator>::operator--(int) ->
    reverse_iterator {
  return reverse_iterator(i_++);
}

template<typename Iterator>
auto reverse_iterator<Iterator>::operator+(difference_type n) const ->
    reverse_iterator {
  return reverse_iterator(i_ - n);
}

template<typename Iterator>
auto reverse_iterator<Iterator>::operator-(difference_type n) const ->
    reverse_iterator {
  return reverse_iterator(i_ + n);
}

template<typename Iterator>
auto reverse_iterator<Iterator>::operator+=(difference_type n) ->
    reverse_iterator& {
  i_ -= n;
  return *this;
}

template<typename Iterator>
auto reverse_iterator<Iterator>::operator-=(difference_type n) ->
    reverse_iterator& {
  i_ += n;
  return *this;
}


template<typename I1, typename I2>
bool operator==(const reverse_iterator<I1>& a, const reverse_iterator<I2>& b)
    noexcept(noexcept(a.base() == b.base())) {
  return a.base() == b.base();
}

template<typename I1, typename I2>
bool operator!=(const reverse_iterator<I1>& a, const reverse_iterator<I2>& b)
    noexcept(noexcept(a.base() != b.base())) {
  return a.base() != b.base();
}

template<typename I1, typename I2>
bool operator<(const reverse_iterator<I1>& a, const reverse_iterator<I2>& b)
    noexcept(noexcept(a.base() > b.base())) {
  return a.base() > b.base();
}

template<typename I1, typename I2>
bool operator>(const reverse_iterator<I1>& a, const reverse_iterator<I2>& b)
    noexcept(noexcept(a.base() < b.base())) {
  return a.base() < b.base();
}

template<typename I1, typename I2>
bool operator<=(const reverse_iterator<I1>& a, const reverse_iterator<I2>& b)
    noexcept(noexcept(a.base() >= b.base())) {
  return a.base() >= b.base();
}

template<typename I1, typename I2>
bool operator>=(const reverse_iterator<I1>& a, const reverse_iterator<I2>& b)
    noexcept(noexcept(a.base() <= b.base())) {
  return a.base() <= b.base();
}


template<typename Iterator>
auto operator+(typename reverse_iterator<Iterator>::difference_type n,
               const reverse_iterator<Iterator>& i)
    noexcept(noexcept(i + n)) -> reverse_iterator<Iterator> {
  return i + n;
}

template<typename Iterator>
auto operator-(const reverse_iterator<Iterator>& a, const reverse_iterator<Iterator>& b)
    noexcept(noexcept(b.base() - a.base())) ->
    typename reverse_iterator<Iterator>::difference_type {
  return b.base() - a.base();
}

template<typename I1, typename I2>
auto operator-(const reverse_iterator<I1>& a, const reverse_iterator<I2>& b)
    noexcept(noexcept(b.base() - a.base())) ->
    decltype(b.base() - a.base()) {
  return b.base() - a.base();
}


/* Provide helper implementations for advance, distance. */
namespace impl {


template<typename Iter, typename Distance>
void advance(Iter& x, Distance n, const random_access_iterator_tag&) {
  x += n;
}

template<typename Iter, typename Distance>
void advance(Iter& x, Distance n, const bidirectional_iterator_tag&) {
  while (n > Distance(0)) {
    ++x;
    --n;
  }
  while (n < Distance(0)) {
    --x;
    ++n;
  }
}

template<typename Iter, typename Distance>
void advance(Iter& x, Distance n, const input_iterator_tag&) {
  assert(n >= Distance(0));
  while (n > Distance(0)) {
    ++x;
    --n;
  }
}


template<typename Iter, typename Distance>
auto distance(Iter& x, Iter& y, const random_access_iterator_tag&) ->
    typename iterator_traits<Iter>::difference_type {
  return y - x;
}

template<typename Iter, typename Distance>
auto distance(Iter& x, Iter& y, const input_iterator_tag&) ->
    typename iterator_traits<Iter>::difference_type {
  typename iterator_traits<Iter>::difference_type count = 0;

  while (x != y) {
    ++x;
    ++count;
  }
  return count;
}


} /* namespace std::impl */

template<typename InputIterator, typename Distance>
void advance(InputIterator& x, Distance n) {
  impl::advance(
      x, n, typename iterator_traits<InputIterator>::iterator_category());
}

template<typename InputIterator>
typename iterator_traits<InputIterator>::difference_type distance(
    InputIterator x, InputIterator y) {
  return impl::distance(
      x, y, typename iterator_traits<InputIterator>::iterator_category());
}

template<typename ForwardIterator>
ForwardIterator next(
    ForwardIterator x,
    typename iterator_traits<ForwardIterator>::difference_type n) {
  advance(x, n);
  return x;
}

template<typename BidirectionalIterator>
BidirectionalIterator prev(
    BidirectionalIterator x,
    typename iterator_traits<BidirectionalIterator>::difference_type n) {
  advance(x, -n);
  return x;
}


_namespace_end(std)
