_namespace_begin(std)


template<typename T, typename Alloc>
vector<T, Alloc>::vector(const allocator_type& alloc)
: impl::alloc_base<Alloc>(alloc)
{}

template<typename T, typename Alloc>
vector<T, Alloc>::vector(size_type n, const allocator_type& alloc)
: vector(alloc)
{
  resize(n);
}

template<typename T, typename Alloc>
vector<T, Alloc>::vector(size_type n, const_reference v,
                         const allocator_type& alloc)
: vector(alloc)
{
  resize(n, v);
}

template<typename T, typename Alloc>
template<typename InputIter>
vector<T, Alloc>::vector(InputIter b, InputIter e, const allocator_type& alloc)
: vector(alloc)
{
  const auto dist = impl::equal_support__distance(
      b, e, typename iterator_traits<InputIter>::iterator_category());
  if (dist.first) reserve(dist.second);
  copy(b, e, back_inserter(*this));
}

template<typename T, typename Alloc>
vector<T, Alloc>::vector(const vector& o)
: vector(o.get_allocator())
{
  reserve(o.size());
  copy(o.begin(), o.end(), back_inserter(*this));
}

template<typename T, typename Alloc>
vector<T, Alloc>::vector(vector&& o)
    noexcept(is_nothrow_move_constructible<impl::alloc_base<Alloc>>::value)
: impl::alloc_base<Alloc>(move(o))
{
  using _namespace(std)::swap;

  swap(heap_, o.heap_);
  swap(size_, o.size_);
  swap(avail_, o.avail_);
}

template<typename T, typename Alloc>
vector<T, Alloc>::vector(const vector& o, const allocator_type& alloc)
: vector(alloc)
{
  reserve(o.size());
  copy(o.begin(), o.end(), back_inserter(*this));
}

template<typename T, typename Alloc>
vector<T, Alloc>::vector(vector&& o, const allocator_type& alloc)
: vector(alloc)
{
  if (this->get_allocator_() == o.get_allocator_()) {
    using _namespace(std)::swap;

    swap(heap_, o.heap_);
    swap(size_, o.size_);
    swap(avail_, o.avail_);
  } else {
    reserve(o.size());
    copy(o.begin(), o.end(), back_inserter(*this));
    o.clear();
  }
}

template<typename T, typename Alloc>
vector<T, Alloc>::vector(initializer_list<value_type> il,
                         const allocator_type& alloc)
: vector(alloc)
{
  reserve(il.size());
  copy(il.begin(), il.end(), back_inserter(*this));
}

template<typename T, typename Alloc>
vector<T, Alloc>::~vector() {
  clear();
  if (avail_ > 0) this->deallocate_(heap_, avail_);
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::operator=(const vector& o) -> vector& {
  reserve(o.size());
  size_type delta = min(o.size(), size());
  copy(o.begin(), o.begin() + delta, begin());
  if (delta < o.size())
    copy(o.begin() + delta, o.end(), back_inserter(*this));
  else if (delta < size())
    resize(delta);
  return *this;
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::operator=(vector&& o) -> vector& {
  reserve(o.size());
  if (this->get_allocator_() == o.get_allocator_()) {
    swap(o);
    o.clear();
  } else {
    size_type delta = min(o.size(), size());
    move(o.begin(), o.begin() + delta, begin());
    if (delta < o.size())
      move(o.begin() + delta, o.end(), back_inserter(*this));
    else if (delta < size())
      resize(delta);
  }
  return *this;
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::operator=(initializer_list<value_type> il) -> vector& {
  reserve(il.size());
  size_type delta = min(size_type(il.size()), size());
  move(il.begin(), il.begin() + delta, begin());
  if (delta < il.size())
    move(il.begin() + delta, il.end(), back_inserter(*this));
  else if (delta < size())
    resize(delta);
  return *this;
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::begin() noexcept -> iterator {
  return &*heap_;
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::end() noexcept -> iterator {
  return begin() + size();
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::begin() const noexcept -> const_iterator {
  return &*heap_;
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::end() const noexcept -> const_iterator {
  return begin() + size();
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::cbegin() const noexcept -> const_iterator {
  return begin();
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::cend() const noexcept -> const_iterator {
  return begin() + size();
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::rbegin() noexcept -> reverse_iterator {
  return reverse_iterator(end());
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::rend() noexcept -> reverse_iterator {
  return reverse_iterator(begin());
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::rbegin() const noexcept -> const_reverse_iterator {
  return const_reverse_iterator(end());
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::rend() const noexcept -> const_reverse_iterator {
  return const_reverse_iterator(begin());
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::crbegin() const noexcept -> const_reverse_iterator {
  return const_reverse_iterator(cend());
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::crend() const noexcept -> const_reverse_iterator {
  return const_reverse_iterator(cbegin());
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::size() const noexcept -> size_type {
  return size_;
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::max_size() const noexcept -> size_type {
  return allocator_traits<allocator_type>::max_size(this->get_allocator_());
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::resize(size_type desired) -> void {
  if (size_ > desired) {
    if (is_trivially_destructible<value_type>::value) {
      size_ = desired;
    } else {
      while (size_ > desired)
        allocator_traits<allocator_type>::destroy(this->get_allocator_(),
                                                  &data()[--size_]);
    }
  } else if (size_ < desired) {
    reserve(desired);
    if (is_trivially_constructible<value_type>::value) {
      bzero(end(), (desired - size_) * sizeof(value_type));
      size_ = desired;
    } else {
      while (size_ < desired) {
        allocator_traits<allocator_type>::construct(this->get_allocator_(),
                                                    &data()[size_]);
        ++size_;
      }
    }
  }
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::resize(size_type desired, const_reference v) -> void {
  if (size_ > desired) {
    if (is_trivially_destructible<value_type>()) {
      size_ = desired;
    } else {
      while (size_ > desired)
        allocator_traits<allocator_type>::destroy(this->get_allocator_(),
                                                  &data()[--size_]);
    }
  } else if (size_ < desired) {
    reserve(desired);
    while (size_ < desired) {
      allocator_traits<allocator_type>::construct(this->get_allocator_(),
                                                  &data()[size_], v);
      ++size_;
    }
  }
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::capacity() const noexcept -> size_type {
  return avail_;
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::empty() const noexcept -> bool {
  return size() == 0;
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::reserve(size_type rsv) -> void {
  if (avail_ < rsv) {
    size_type new_sz = max(rsv, 2 * size());

    /* First, try to use the resize extension. */
    if (allocator_traits<allocator_type>::resize(this->get_allocator_(),
                                                 heap_, avail_, new_sz)) {
      avail_ = new_sz;
      return;
    } else if (rsv != new_sz &&
               allocator_traits<allocator_type>::resize(this->get_allocator_(),
                                                        heap_, avail_, rsv)) {
      avail_ = rsv;
      return;
    }

    vector temp(this->get_allocator_());
    try {
      temp.heap_ = this->allocate_(new_sz);
      temp.avail_ = new_sz;
    } catch (const bad_alloc&) {
      if (new_sz == rsv) throw;
      new_sz = rsv;
      temp.heap_ = this->allocate_(new_sz);
      temp.avail_ = new_sz;
    }
    if (is_nothrow_move_constructible<value_type>::value)
      move(begin(), end(), back_inserter(temp));
    else
      copy(begin(), end(), back_inserter(temp));
    swap(temp);
  }
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::shrink_to_fit() -> void {
  if (avail_ > size_) {
    if (allocator_traits<allocator_type>::resize(this->get_allocator_(),
                                                 heap_, avail_, size_)) {
      avail_ = size_;
      return;
    }

    vector temp(this->get_allocator_());
    temp.heap_ = this->allocate_(size());
    temp.avail_ = size();
    if (is_nothrow_move_constructible<value_type>::value)
      move(begin(), end(), back_inserter(temp));
    else
      copy(begin(), end(), back_inserter(temp));
    swap(temp);
  }
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::operator[](size_type idx) noexcept -> reference {
  assert(idx < size_);
  return data()[idx];
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::operator[](size_type idx) const noexcept ->
    const_reference {
  assert(idx < size_);
  return data()[idx];
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::at(size_type idx) -> reference {
  if (idx >= size()) throw out_of_range("vector::at");
  return (*this)[idx];
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::at(size_type idx) const -> const_reference {
  if (idx >= size()) throw out_of_range("vector::at");
  return (*this)[idx];
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::front() noexcept -> reference {
  return *begin();
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::front() const noexcept -> const_reference {
  return *begin();
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::back() noexcept -> reference {
  return *rbegin();
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::back() const noexcept -> const_reference {
  return *rbegin();
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::data() noexcept -> value_type* {
  return &*heap_;
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::data() const noexcept -> const value_type* {
  return &*heap_;
}

template<typename T, typename Alloc>
template<typename... Args>
auto vector<T, Alloc>::emplace_back(Args&&... args) -> void {
  reserve(size() + 1U);
  allocator_traits<allocator_type>::construct(this->get_allocator_(),
                                              &heap_[size_],
                                              forward<Args>(args)...);
  ++size_;
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::push_back(const_reference v) -> void {
  reserve(size() + 1U);
  allocator_traits<allocator_type>::construct(this->get_allocator_(),
                                              &data()[size_], v);
  ++size_;
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::push_back(value_type&& v) -> void {
  reserve(size() + 1U);
  allocator_traits<allocator_type>::construct(this->get_allocator_(),
                                              &data()[size_],
                                              move(v));
  ++size_;
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::pop_back() -> void {
  assert(size() > 0);
  if (size() > 0) {
    allocator_traits<allocator_type>::destroy(this->get_allocator_(),
                                              &data()[--size_]);
  }
}

template<typename T, typename Alloc>
template<typename... Args>
auto vector<T, Alloc>::emplace(const_iterator pos_, Args&&... args) ->
    iterator {
  assert(begin() <= pos_ && pos_ <= end());

  const auto dist = pos_ - begin();
  emplace_back(forward<Args>(args)...);
  rotate(begin() + dist, prev(end()), end());
  return begin() + dist;
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::insert(const_iterator pos_, const_reference v) ->
    iterator {
  assert(begin() <= pos_ && pos_ <= end());

  const auto dist = pos_ - begin();
  resize(size() + 1U);
  iterator pos = begin() + dist;
  move_backward(pos, prev(end()), end());
  *pos = v;
  return pos;
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::insert(const_iterator pos_, value_type&& v) ->
    iterator {
  assert(begin() <= pos_ && pos_ <= end());

  const auto dist = pos_ - begin();
  resize(size() + 1U);
  iterator pos = begin() + dist;
  move_backward(pos, prev(end()), end());
  *pos = forward<value_type>(v);
  return pos;
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::insert(const_iterator pos_,
                              size_type n, const_reference v) -> iterator {
  assert(begin() <= pos_ && pos_ <= end());

  const auto dist = pos_ - begin();
  resize(size() + n);
  auto pos = begin() + dist;
  move_backward(pos, end() - n, end());
  fill_n(pos, n, v);
  return pos;
}

template<typename T, typename Alloc>
template<typename InputIter>
auto vector<T, Alloc>::insert(const_iterator pos, InputIter b, InputIter e) ->
    iterator {
  assert(begin() <= pos && pos <= end());

  const size_type off = pos - begin();
  const auto dist = impl::equal_support__distance(
      b, e, typename iterator_traits<InputIter>::iterator_category());
  if (!dist.first) {
    copy(b, e, inserter(*this, begin() + off));
    return begin() + off;
  }

  resize(size() + dist.second);
  move_backward(begin() + off, end() - dist.second, end());
  copy(b, e, begin() + off);
  return begin() + off;
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::insert(const_iterator pos_,
                              initializer_list<value_type> il) -> iterator {
  assert(begin() <= pos_ && pos_ <= end());

  const size_type off = pos_ - begin();
  resize(size() + il.size());
  move_backward(begin() + off, end() - il.size(), end());
  copy(il.begin(), il.end(), begin() + off);
  return begin() + off;
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::erase(const_iterator pos_) -> iterator {
  assert(begin() <= pos_ && pos_ < end());

  iterator pos = begin() + (pos_ - begin());
  move(pos + 1, end(), pos);
  pop_back();
  return pos;
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::erase(const_iterator b_, const_iterator e_) ->
    iterator {
  assert(begin() <= b_ && b_ <= e_ && e_ <= end());

  auto b = begin() + (b_ - begin());
  auto e = begin() + (e_ - begin());

  move(e, end(), b);
  resize(size() - (e - b));
  return b;
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::swap(vector& o)
    noexcept(noexcept(declval<vector>().swap_(o))) -> void {
  using _namespace(std)::swap;

  this->impl::alloc_base<Alloc>::swap_(o);
  swap(heap_, o.heap_);
  swap(size_, o.size_);
  swap(avail_, o.avail_);
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::clear() noexcept -> void {
  resize(0);
}


template<typename T, typename Alloc>
bool operator==(const vector<T, Alloc>& a, const vector<T, Alloc>& b) noexcept(
    noexcept(declval<const typename vector<T, Alloc>::value_type>() ==
             declval<const typename vector<T, Alloc>::value_type>())) {
  return equal(a.begin(), a.end(), b.begin(), b.end());
}

template<typename T, typename Alloc>
bool operator<(const vector<T, Alloc>& a, const vector<T, Alloc>& b) noexcept(
    noexcept(declval<const typename vector<T, Alloc>::value_type>() <
             declval<const typename vector<T, Alloc>::value_type>())) {
  return lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
}

template<typename T, typename Alloc>
bool operator!=(const vector<T, Alloc>& a, const vector<T, Alloc>& b)
    noexcept(noexcept(!(a == b))) {
  return !(a == b);
}

template<typename T, typename Alloc>
bool operator>(const vector<T, Alloc>& a, const vector<T, Alloc>& b)
    noexcept(noexcept(b < a)) {
  return b < a;
}

template<typename T, typename Alloc>
bool operator>=(const vector<T, Alloc>& a, const vector<T, Alloc>& b)
    noexcept(noexcept(!(a < b))) {
  return !(a < b);
}

template<typename T, typename Alloc>
bool operator<=(const vector<T, Alloc>& a, const vector<T, Alloc>& b)
    noexcept(noexcept(!(b < a))) {
  return !(b < a);
}

template<typename T, typename Alloc>
void swap(vector<T, Alloc>& a, vector<T, Alloc>& b)
    noexcept(noexcept(a.swap(b)))
{
  a.swap(b);
}


_namespace_end(std)
