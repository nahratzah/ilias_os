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
  return *prev(i_);
}

template<typename Iterator>
auto reverse_iterator<Iterator>::operator->() const -> pointer {
  return &*prev(i_);
}

template<typename Iterator>
auto reverse_iterator<Iterator>::operator[](difference_type n) const ->
    conditional_t<
        is_base_of<random_access_iterator_tag,
                   typename iterator_traits<Iterator>::iterator_category
                  >::value,
        typename iterator_traits<Iterator>::reference,
        void> {
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


template<typename T, typename Char, typename Traits, typename Distance>
istream_iterator<T, Char, Traits, Distance>::istream_iterator(istream_type& s)
: in_(&s),
  value_()
{
  ++*this;  // Initialize value_ with data from stream.
}

template<typename T, typename Char, typename Traits, typename Distance>
auto istream_iterator<T, Char, Traits, Distance>::operator*() const ->
    const T& {
  assert_msg(in_ != nullptr, "Cannot dereference end-of-stream iterator.");
  return value_;
}

template<typename T, typename Char, typename Traits, typename Distance>
auto istream_iterator<T, Char, Traits, Distance>::operator->() const ->
    const T* {
  assert_msg(in_ != nullptr, "Cannot dereference end-of-stream iterator.");
  return &value_;
}

template<typename T, typename Char, typename Traits, typename Distance>
auto istream_iterator<T, Char, Traits, Distance>::operator++() ->
    istream_iterator& {
  assert_msg(in_ != nullptr, "Cannot step forward past end-of-stream.");
  *in_ >> value_;
  if (in_->fail()) in_ = nullptr;  // Become an end-of-stream iterator.
  return *this;
}

template<typename T, typename Char, typename Traits, typename Distance>
auto istream_iterator<T, Char, Traits, Distance>::operator++(int) ->
    istream_iterator {
  istream_iterator tmp = *this;
  ++*this;
  return tmp;
}

template<typename T, typename Char, typename Traits, typename Distance>
auto operator==(const istream_iterator<T, Char, Traits, Distance>& x,
                const istream_iterator<T, Char, Traits, Distance>& y) -> bool {
  return x.in_ == y.in_;
}

template<typename T, typename Char, typename Traits, typename Distance>
auto operator!=(const istream_iterator<T, Char, Traits, Distance>& x,
                const istream_iterator<T, Char, Traits, Distance>& y) -> bool {
  return !(x == y);
}


template<typename T, typename Char, typename Traits>
ostream_iterator<T, Char, Traits>::ostream_iterator(ostream_type& s)
: out_(s)
{}

template<typename T, typename Char, typename Traits>
ostream_iterator<T, Char, Traits>::ostream_iterator(ostream_type& s,
                                                    const Char* delim)
: out_(s),
  delim_(delim)
{}

/* This is an extension. */
template<typename T, typename Char, typename Traits>
ostream_iterator<T, Char, Traits>::ostream_iterator(
    ostream_type& s, basic_string_ref<Char, Traits> delim)
: out_(s),
  delim_(delim)
{}

template<typename T, typename Char, typename Traits>
auto ostream_iterator<T, Char, Traits>::operator=(const T& v) ->
    ostream_iterator& {
  *out_ << v;
  if (!delim_.empty()) *out_ << delim_;
  return *this;
}

template<typename T, typename Char, typename Traits>
auto ostream_iterator<T, Char, Traits>::operator*() ->
    ostream_iterator& {
  return *this;
}

template<typename T, typename Char, typename Traits>
auto ostream_iterator<T, Char, Traits>::operator++() ->
    ostream_iterator& {
  return *this;
}

template<typename T, typename Char, typename Traits>
auto ostream_iterator<T, Char, Traits>::operator++(int) ->
    ostream_iterator& {
  return *this;
}


template<typename Char, typename Traits>
istreambuf_iterator<Char, Traits>::istreambuf_iterator(
    istream_type& s) noexcept
: istreambuf_iterator(s.rdbuf())
{}

template<typename Char, typename Traits>
istreambuf_iterator<Char, Traits>::istreambuf_iterator(
    streambuf_type* buf) noexcept
: sbuf_(buf)
{}

template<typename Char, typename Traits>
istreambuf_iterator<Char, Traits>::istreambuf_iterator(
    const impl::istreambuf_iterator_proxy<Char, Traits>& p) noexcept
: sbuf_(p.sbuf_)
{}

template<typename Char, typename Traits>
auto istreambuf_iterator<Char, Traits>::operator*() const -> Char {
  assert_msg(sbuf_ != nullptr, "Cannot dereference end-of-stream iterator");
  return sbuf_->sgetc();
}

template<typename Char, typename Traits>
auto istreambuf_iterator<Char, Traits>::operator->() const ->
    impl::istreambuf_iterator_proxy<Char, Traits> {
  assert_msg(sbuf_ != nullptr, "Cannot dereference end-of-stream iterator");
  return proxy(sbuf_->sgetc(), sbuf_);
}

template<typename Char, typename Traits>
auto istreambuf_iterator<Char, Traits>::operator++() -> istreambuf_iterator& {
  assert_msg(sbuf_ != nullptr, "Cannot advance beyond end-of-stream");
  sbuf_->sbumpc();
  return *this;
}

template<typename Char, typename Traits>
auto istreambuf_iterator<Char, Traits>::operator++(int) ->
    impl::istreambuf_iterator_proxy<Char, Traits> {
  assert_msg(sbuf_ != nullptr, "Cannot advance beyond end-of-stream");
  return proxy(sbuf_->sbumpc(), sbuf_);
}

template<typename Char, typename Traits>
auto istreambuf_iterator<Char, Traits>::equal(const istreambuf_iterator& b)
    const -> bool {
  return bool(sbuf_ == nullptr) == bool(b.sbuf_ == nullptr);
}

template<typename Char, typename Traits>
bool operator==(const istreambuf_iterator<Char, Traits>& a,
                const istreambuf_iterator<Char, Traits>& b) {
  return a.equal(b);
}

template<typename Char, typename Traits>
bool operator!=(const istreambuf_iterator<Char, Traits>& a,
                const istreambuf_iterator<Char, Traits>& b) {
  return !a.equal(b);
}

namespace impl {

template<typename Char, typename Traits>
auto istreambuf_iterator_proxy<Char, Traits>::operator*() -> Char {
  return keep_;
}

template<typename Char, typename Traits>
auto istreambuf_iterator_proxy<Char, Traits>::operator->() const ->
    const Char* {
  return &keep_;
}

template<typename Char, typename Traits>
istreambuf_iterator_proxy<Char, Traits>::istreambuf_iterator_proxy(
    Char ch, basic_streambuf<Char, Traits>* buf)
: sbuf_(buf),
  keep_(ch)
{}

} /* namespace std::impl */


template<typename Char, typename Traits>
ostreambuf_iterator<Char, Traits>::ostreambuf_iterator(ostream_type& s)
    noexcept
: ostreambuf_iterator(s.rdbuf())
{}

template<typename Char, typename Traits>
ostreambuf_iterator<Char, Traits>::ostreambuf_iterator(streambuf_type* buf)
    noexcept
: sbuf_(buf)
{
  assert(buf != nullptr);
}

template<typename Char, typename Traits>
auto ostreambuf_iterator<Char, Traits>::operator=(Char ch) ->
    ostreambuf_iterator& {
  if (!failed())
    failed_ = traits_type::eq_int_type(sbuf_->sputc(ch), traits_type::eof());
  return *this;
}

template<typename Char, typename Traits>
auto ostreambuf_iterator<Char, Traits>::operator++() -> ostreambuf_iterator& {
  return *this;
}

template<typename Char, typename Traits>
auto ostreambuf_iterator<Char, Traits>::operator++(int) ->
    ostreambuf_iterator& {
  return *this;
}

template<typename Char, typename Traits>
auto ostreambuf_iterator<Char, Traits>::failed() const noexcept -> bool {
  return failed_;
}


template<typename C> auto begin(C& c) -> decltype(c.begin()) {
  return c.begin();
}
template<typename C> auto begin(const C& c) -> decltype(c.begin()) {
  return c.begin();
}
template<typename C> auto end(C& c) -> decltype(c.end()) {
  return c.end();
}
template<typename C> auto end(const C& c) -> decltype(c.end()) {
  return c.end();
}
template<typename T, size_t N> T* begin(T (&array)[N]) {
  return array;
}
template<typename T, size_t N> T* end(T (&array)[N]) {
  return array + N;
}
template<typename C> auto cbegin(const C& c) ->
    decltype(_namespace(std)::begin(c)) {
  return _namespace(std)::begin(c);
}
template<typename C> auto cend(const C& c) ->
    decltype(_namespace(std)::end(c)) {
  return _namespace(std)::end(c);
}
template<typename C> auto rbegin(C& c) -> decltype(c.rbegin()) {
  return c.rbegin();
}
template<typename C> auto rbegin(const C& c) -> decltype(c.rbegin()) {
  return c.rbegin();
}
template<typename C> auto rend(C& c) -> decltype(c.rend()) {
  return c.rend();
}
template<typename C> auto rend(const C& c) -> decltype(c.rend()) {
  return c.rend();
}
template<typename T, size_t N> reverse_iterator<T*> begin(T (&array)[N]) {
  return reverse_iterator<T*>(array + N);
}
template<typename T, size_t N> reverse_iterator<T*> end(T (&array)[N]) {
  return reverse_iterator<T*>(array);
}
template<typename E> reverse_iterator<const E*> rbegin(
    initializer_list<E> il) {
  return reverse_iterator<const E*>(il.end());
}
template<typename E> reverse_iterator<const E*> rend(
    initializer_list<E> il) {
  return reverse_iterator<const E*>(il.begin());
}
template<typename C> auto crbegin(const C& c) ->
    decltype(_namespace(std)::rbegin(c)) {
  return _namespace(std)::rbegin(c);
}
template<typename C> auto crend(const C& c) ->
    decltype(_namespace(std)::rend(c)) {
  return _namespace(std)::rend(c);
}


_namespace_end(std)
