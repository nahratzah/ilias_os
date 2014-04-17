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
auto operator-(const reverse_iterator<Iterator>& a,
               const reverse_iterator<Iterator>& b)
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


template<typename Iter>
auto distance(Iter& x, Iter& y, const random_access_iterator_tag&) ->
    typename iterator_traits<Iter>::difference_type {
  return y - x;
}

template<typename Iter>
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


template<typename Container>
back_insert_iterator<Container>::back_insert_iterator(Container& c)
: container(&c)
{}

template<typename Container>
auto back_insert_iterator<Container>::operator=(
    const typename Container::value_type& v) -> back_insert_iterator& {
  container->push_back(v);
  return *this;
}

template<typename Container>
auto back_insert_iterator<Container>::operator=(
    typename Container::value_type&& v) -> back_insert_iterator& {
  container->push_back(move(v));
  return *this;
}

template<typename Container>
auto back_insert_iterator<Container>::operator*() -> back_insert_iterator& {
  return *this;
}

template<typename Container>
auto back_insert_iterator<Container>::operator++() -> back_insert_iterator& {
  return *this;
}

template<typename Container>
auto back_insert_iterator<Container>::operator++(int) -> back_insert_iterator {
  return *this;
}

template<typename Container>
auto back_inserter(Container& c) -> back_insert_iterator<Container> {
  return back_insert_iterator<Container>(c);
}


template<typename Container>
front_insert_iterator<Container>::front_insert_iterator(Container& c)
: container(&c)
{}

template<typename Container>
auto front_insert_iterator<Container>::operator=(
    const typename Container::value_type& v) -> front_insert_iterator& {
  container->push_front(v);
  return *this;
}

template<typename Container>
auto front_insert_iterator<Container>::operator=(
    typename Container::value_type&& v) -> front_insert_iterator& {
  container->push_front(move(v));
  return *this;
}

template<typename Container>
auto front_insert_iterator<Container>::operator*() -> front_insert_iterator& {
  return *this;
}

template<typename Container>
auto front_insert_iterator<Container>::operator++() -> front_insert_iterator& {
  return *this;
}

template<typename Container>
auto front_insert_iterator<Container>::operator++(int) ->
    front_insert_iterator {
  return *this;
}

template<typename Container>
auto front_inserter(Container& c) -> front_insert_iterator<Container> {
  return front_insert_iterator<Container>(c);
}


template<typename Container>
insert_iterator<Container>::insert_iterator(Container& c,
                                            typename Container::iterator i)
: container(&c),
  iter(i)
{}

template<typename Container>
auto insert_iterator<Container>::operator=(
    const typename Container::value_type& v) -> insert_iterator& {
  iter = container->insert(iter, v);
  ++iter;
  return *this;
}

template<typename Container>
auto insert_iterator<Container>::operator=(
    typename Container::value_type&& v) -> insert_iterator& {
  iter = container->insert(iter, move(v));
  ++iter;
  return *this;
}

template<typename Container>
auto insert_iterator<Container>::operator*() -> insert_iterator& {
  return *this;
}

template<typename Container>
auto insert_iterator<Container>::operator++() -> insert_iterator& {
  return *this;
}

template<typename Container>
auto insert_iterator<Container>::operator++(int) -> insert_iterator {
  return *this;
}

template<typename Container>
auto inserter(Container& c, typename Container::iterator i) ->
    insert_iterator<Container> {
  return insert_iterator<Container>(c, i);
}


template<typename Iterator>
move_iterator<Iterator>::move_iterator(Iterator iter)
: iter_(iter)
{}

template<typename Iterator>
template<typename U>
move_iterator<Iterator>::move_iterator(const move_iterator<U>& other)
: iter_(other.base())
{}

template<typename Iterator>
template<typename U>
auto move_iterator<Iterator>::operator=(const move_iterator<U>& other) ->
    move_iterator& {
  iter_ = other.base();
  return *this;
}

template<typename Iterator>
auto move_iterator<Iterator>::base() const -> iterator_type {
  return iter_;
}

template<typename Iterator>
auto move_iterator<Iterator>::operator*() const -> reference {
  return move(*iter_);
}

template<typename Iterator>
auto move_iterator<Iterator>::operator->() const -> pointer {
  return iter_;
}

template<typename Iterator>
auto move_iterator<Iterator>::operator++() -> move_iterator& {
  ++iter_;
  return *this;
}

template<typename Iterator>
auto move_iterator<Iterator>::operator++(int) -> move_iterator {
  move_iterator tmp = *this;
  ++iter_;
  return tmp;
}

template<typename Iterator>
auto move_iterator<Iterator>::operator--() -> move_iterator& {
  --iter_;
  return *this;
}

template<typename Iterator>
auto move_iterator<Iterator>::operator--(int) -> move_iterator {
  move_iterator tmp = *this;
  --iter_;
  return tmp;
}

template<typename Iterator>
auto move_iterator<Iterator>::operator+(difference_type n) const ->
    move_iterator {
  return move_iterator(iter_ + n);
}

template<typename Iterator>
auto move_iterator<Iterator>::operator+=(difference_type n) -> move_iterator& {
  iter_ += n;
  return *this;
}

template<typename Iterator>
auto move_iterator<Iterator>::operator-(difference_type n) const ->
    move_iterator {
  return move_iterator(iter_ - n);
}

template<typename Iterator>
auto move_iterator<Iterator>::operator-=(difference_type n) -> move_iterator& {
  iter_ -= n;
  return *this;
}

template<typename Iterator>
auto move_iterator<Iterator>::operator[](difference_type n) const ->
    reference {
  return move(iter_[n]);
}

template<typename Iterator1, typename Iterator2>
auto operator==(const move_iterator<Iterator1>& x,
                const move_iterator<Iterator2>& y) -> bool {
  return x.base() == y.base();
}

template<typename Iterator1, typename Iterator2>
auto operator!=(const move_iterator<Iterator1>& x,
                const move_iterator<Iterator2>& y) -> bool {
  return !(x == y);
}

template<typename Iterator1, typename Iterator2>
auto operator<(const move_iterator<Iterator1>& x,
               const move_iterator<Iterator2>& y) -> bool {
  return x.base() < y.base();
}

template<typename Iterator1, typename Iterator2>
auto operator<=(const move_iterator<Iterator1>& x,
                const move_iterator<Iterator2>& y) -> bool {
  return !(y < x);
}

template<typename Iterator1, typename Iterator2>
auto operator>(const move_iterator<Iterator1>& x,
               const move_iterator<Iterator2>& y) -> bool {
  return y < x;
}

template<typename Iterator1, typename Iterator2>
auto operator>=(const move_iterator<Iterator1>& x,
                const move_iterator<Iterator2>& y) -> bool {
  return !(x < y);
}

template<typename Iterator1, typename Iterator2>
auto operator-(const move_iterator<Iterator1>& x,
               const move_iterator<Iterator2>& y) ->
    decltype(x.base() - y.base()) {
  return x.base() - y.base();
}

template<typename Iterator>
auto operator+(typename move_iterator<Iterator>::difference_type n,
               const move_iterator<Iterator>& x) -> move_iterator<Iterator> {
  return x + n;
}

template<typename Iterator>
auto make_move_iterator(Iterator i) -> move_iterator<Iterator> {
  return move_iterator<Iterator>(i);
}


_namespace_end(std)
