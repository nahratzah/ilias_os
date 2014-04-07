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


_namespace_end(std)
