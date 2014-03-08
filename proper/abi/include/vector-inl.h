_namespace_begin(std)
namespace impl {


template<typename Vector>
struct vector_copy_impl_basics<Vector, true> {
  using vector = Vector;
  using value_type = typename vector::value_type;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using size_type = typename vector::size_type;
  using allocator_type = typename vector::allocator_type;

  static void copy(vector& a, const_pointer p, size_type n) noexcept {
    assert(a.capacity() >= n);
    memcpy(a.data(), p, (a.size_ = n) * sizeof(value_type));
  }

  static void copy_at(pointer dst, const_pointer src, size_type n) noexcept {
    memcpy(dst, src, n);
  }

  static void move(vector& a, const_pointer p, size_type n) noexcept {
    copy(a, p, n);
  }

  static void assign(vector& a, const_pointer p, size_type n) noexcept {
    copy(a, p, n);
  }

  static void move_assign(vector& a, const_pointer p, size_type n) noexcept {
    copy(a, p, n);
  }

  static void shift_down(pointer dst, pointer src, size_type n) noexcept {
    assert(&*dst < &*src);
    memmove(&*dst, &*src, n * sizeof(value_type));
  }

  static void shift_up(vector&, size_type n_up, pointer src, size_type n)
      noexcept {
    assert(n_up > 0);
    memmove(src + n_up, src, n * sizeof(value_type));
  }
};

template<typename Vector>
struct vector_copy_impl_basics<Vector, false> {
  using vector = Vector;
  using value_type = typename vector::value_type;
  using reference = typename vector::reference;
  using const_reference = typename vector::const_reference;
  using pointer = typename vector::pointer;
  using const_pointer = typename vector::const_pointer;
  using size_type = typename vector::size_type;
  using allocator_type = typename vector::allocator_type;

  static void copy(vector& a, const_pointer p, size_type n) noexcept(noexcept(
      allocator_traits<allocator_type>::construct(
          a.get_allocator_(), declval<pointer>(),
          declval<const value_type>()))) {
    assert(a.capacity() >= n);
    for (pointer i = a.heap_, end = a.heap_ + n; i != end; ++i, ++p)
      allocator_traits<allocator_type>::construct(a.get_allocator_(), i, *p);
    a.size_ = n;
  }

  static void copy_at(pointer dst, const_pointer src, size_type n) noexcept(
      noexcept(declval<reference>() = declval<const_reference>())) {
    while (n-- > 0) *dst++ = *src++;
  }

  static void move(vector& a, const_pointer p, size_type n) noexcept(noexcept(
      allocator_traits<allocator_type>::construct(a.get_allocator_(),
                                                  declval<pointer>(),
                                                  move_if_noexcept(
                                                    declval<value_type>())))) {
    assert(a.capacity() >= n);
    for (pointer i = a.heap_, end = a.heap_ + n; i != end; ++i, ++p) {
      allocator_traits<allocator_type>::construct(a.get_allocator_(),
                                                  i, move_if_noexcept(*p));
      ++a.size_;
    }
  }

  static void assign(vector& a, const_pointer p, size_type n) noexcept(
      noexcept(declval<reference>() = declval<const_reference>()) &&
      noexcept(
        allocator_traits<allocator_type>::construct(a.get_allocator_(),
                                                    declval<pointer>(),
                                                    declval<value_type>())) &&
      noexcept(
        allocator_traits<allocator_type>::destroy(a.get_allocator_(),
                                                  declval<pointer>()))) {
    assert(a.capacity() >= n);
    pointer i = a.heap_;
    pointer i_end = a.heap_ + a.size();
    pointer p_end = p + n;
    while (i != i_end && p != p_end) *i++ = *p++;
    while (p != p_end) {
      allocator_traits<allocator_type>::construct(a.get_allocator_(),
                                                  i++, *p++);
      ++a.size_;
    }
    while (a.size_ > n) {
      allocator_traits<allocator_type>::destroy(a.get_allocator_(),
                                                &a.heap_[--a.size_]);
    }
  }

  static void move_assign(vector& a, const_pointer p, size_type n) noexcept(
      noexcept(declval<reference>() =
                   move_if_noexcept(declval<const_reference>())) &&
      noexcept(allocator_traits<allocator_type>::construct(
          a.get_allocator_(), declval<pointer>(),
          move_if_noexcept(declval<value_type>()))) &&
      noexcept(
        allocator_traits<allocator_type>::destroy(a.get_allocator_(),
                                                  declval<pointer>()))) {
    assert(a.capacity() >= n);
    pointer i = a.heap_;
    pointer i_end = a.heap_ + a.size();
    pointer p_end = p + n;
    while (i != i_end && p != p_end) *i++ = move_if_noexcept(*p++);
    while (p != p_end) {
      allocator_traits<allocator_type>::construct(a.get_allocator_(),
                                                  i++, move_if_noexcept(*p++));
      ++a.size_;
    }
    while (a.size_ > n) {
      allocator_traits<allocator_type>::destroy(a.get_allocator_(),
                                                &a.heap_[--a.size_]);
    }
  }

  static void shift_down(pointer dst, pointer src, size_type n) noexcept(
      noexcept(declval<reference>() =
                   move_if_noexcept(declval<const_reference>()))) {
    assert(&*dst < &*src);
    while (n-- > 0) *dst++ = move_if_noexcept(*src++);
  }

  static void shift_up(vector& a, size_type n_up, pointer src, size_type n)
      noexcept(
          noexcept(allocator_traits<allocator_type>::construct(
              a.get_allocator_(), declval<pointer>(),
              move_if_noexcept(declval<value_type>()))) &&
          noexcept(declval<reference>() =
                       move_if_noexcept(declval<const_reference>()))) {
    assert(n_up > 0);

    pointer copy_src = src + n;
    pointer copy_dst = src + n_up + n;

    while (n > n_up) {
      --n;
      --copy_dst;
      --copy_src;
      allocator_traits<allocator_type>::construct(
          a.get_allocator_(), copy_dst, move_if_noexcept(*copy_src));
    }
    while (n > 0) {
      --n;
      --copy_dst;
      --copy_src;
      *copy_dst = move_if_noexcept(*copy_src);
    }
  }
};


} /* namespace std::impl */


template<typename T, typename Alloc>
struct vector<T, Alloc>::copy_impl {
  static void copy(vector& a, initializer_list<value_type> b)
      noexcept(noexcept(copy_impl_basics::copy(a, b.begin(), b.size()))) {
    copy_impl_basics::copy(a, b.begin(), b.size());
  }

  static void copy_at(pointer dst, const_pointer src, size_type n)
      noexcept(noexcept(copy_impl_basics::copy_at(dst, src, n))) {
    copy_impl_basics::copy_at(dst, src, n);
  }

  static void copy(vector& a, const vector& b)
      noexcept(noexcept(copy_impl_basics::copy(a, b.data(), b.size()))) {
    copy_impl_basics::copy(a, b.data(), b.size());
  }

  static void move(vector& a, vector&& b)
      noexcept(noexcept(copy_impl_basics::move(a, b.data(), b.size()))) {
    copy_impl_basics::move(a, b.data(), b.size());
  }

  static void assign(vector& a, const vector& b)
      noexcept(noexcept(copy_impl_basics::assign(a, b.data(), b.size()))) {
    copy_impl_basics::assign(a, b.data(), b.size());
  }

  static void assign(vector& a, vector&& b)
      noexcept(noexcept(copy_impl_basics::assign(a, b.data(), b.size()))) {
    copy_impl_basics::move_assign(a, b.data(), b.size());
  }

  static void assign(vector& a, initializer_list<value_type> il)
      noexcept(noexcept(copy_impl_basics::assign(a, il.begin(), il.size()))) {
    copy_impl_basics::assign(a, il.begin(), il.size());
  }

  static void shift_down(pointer dst, pointer src, size_type n) noexcept(
      noexcept(noexcept(copy_impl_basics::shift_down(dst, src, n)))) {
    copy_impl_basics::shift_down(dst, src, n);
  }

  static void shift_up(vector& a, size_type n_up, pointer src, size_type n)
      noexcept(noexcept(copy_impl_basics::shift_up(a, n_up, src, n))) {
    copy_impl_basics::shift_up(a, n_up, src, n);
  }
};


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
  while (b != e) emplace_back(*b++);
}

template<typename T, typename Alloc>
vector<T, Alloc>::vector(const vector& o)
: vector(o.get_allocator())
{
  reserve(o.size());
  copy_impl::copy(*this, o);
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
  copy_impl::copy(*this, o);
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
  } else
    reserve(o.size());
    copy_impl::move(*this, move(o));
}

template<typename T, typename Alloc>
vector<T, Alloc>::vector(initializer_list<value_type> il,
                         const allocator_type& alloc)
: vector(alloc)
{
  reserve(il.size());
  copy_impl::copy(*this, il);
}

template<typename T, typename Alloc>
vector<T, Alloc>::~vector() {
  clear();
  if (avail_ > 0) this->deallocate_(heap_, avail_);
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::operator=(const vector& o) -> vector& {
  reserve(o.size());
  copy_impl::assign(*this, o);
  return *this;
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::operator=(vector&& o) -> vector& {
  reserve(o.size());
  copy_impl::assign(*this, move(o));
  return *this;
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::operator=(initializer_list<value_type> il) -> vector& {
  reserve(il.size());
  copy_impl::assign(*this, il);
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
  return &*heap_;
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
    if (is_trivially_destructible<value_type>()) {
      size_ = desired;
    } else {
      while (size_ > desired)
        allocator_traits<allocator_type>::destroy(this->get_allocator_(),
                                                  &heap_[--size_]);
    }
  } else if (size_ > desired) {
    reserve(desired);

    while (size_ < desired) {
      allocator_traits<allocator_type>::construct(this->get_allocator_(),
                                                  &heap_[size_]);
      ++size_;
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
                                                  &heap_[--size_]);
    }
  } else if (size_ > desired) {
    reserve(desired);

    while (size_ < desired) {
      allocator_traits<allocator_type>::construct(this->get_allocator_(),
                                                  &heap_[size_], v);
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
    copy_impl::move(temp, move(*this));
    swap(temp);
  }
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::shrink_to_fit() -> void {
  if (avail_ > size_) {
    vector temp(this->get_allocator_());
    temp.heap_ = this->allocate_(size());
    temp.avail_ = size();
    copy_impl::move(temp, move(*this));
    swap(temp);
  }
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::operator[](size_type idx) noexcept -> reference {
  return data()[idx];
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::operator[](size_type idx) const noexcept ->
    const_reference {
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
  resize(size() + 1U);
  iterator pos = begin() + (pos_ - begin());
  copy_impl::shift_up(*this, 1U, pos, end() - pos);
  *pos = value_type(forward<Args>(args)...);
  return pos;
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::insert(const_iterator pos_, const_reference v) ->
    iterator {
  resize(size() + 1U);
  iterator pos = begin() + (pos_ - begin());
  copy_impl::shift_up(*this, 1U, pos, end() - pos);
  *pos = v;
  return pos;
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::insert(const_iterator pos_, value_type&& v) ->
    iterator {
  resize(size() + 1U);
  iterator pos = begin() + (pos_ - begin());
  copy_impl::shift_up(*this, 1U, pos, end() - pos);
  *pos = forward<value_type>(v);
  return pos;
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::insert(const_iterator pos_,
                              size_type n, const_reference v) -> iterator {
  iterator pos = begin() + (pos_ - begin());
  if (n != 0) {
    reserve(size() + n);
    copy_impl::shift_up(*this, n, pos, end() - pos);
    for (iterator i = pos; n > 0; ++i, --n) *i = v;
  }
  return pos;
}

template<typename T, typename Alloc>
template<typename InputIter>
auto vector<T, Alloc>::insert(const_iterator pos, InputIter b, InputIter e) ->
    iterator {
  size_type off = pos - begin();
  while (b != e) insert(pos++, *b++);
  return begin() + off;
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::insert(const_iterator pos_,
                              initializer_list<value_type> il) -> iterator {
  assert(begin() <= pos_ && pos_ < end());

  iterator pos = begin() + (pos_ - begin());
  if (!il.empty()) {
    reserve(size() + il.size());
    copy_impl::shift_up(*this, il.size(), pos, end() - pos);
    copy_impl::copy_at(pos, il.begin(), il.size());
  }
  return pos;
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::erase(const_iterator pos_) -> iterator {
  assert(begin() <= pos_ && pos_ < end());

  iterator pos = begin() + (pos_ - begin());
  copy_impl::shift_down(pos, pos + 1U, end() - (pos + 1U));
  resize(size() - 1U);
  return begin() + (pos - begin());
}

template<typename T, typename Alloc>
auto vector<T, Alloc>::erase(const_iterator b, const_iterator e) -> iterator {
  assert(begin() <= b && b <= e && e <= end());

  if (b != e) {
    copy_impl::shift_down(begin() + (b - begin()),
                          begin() + (e - begin()), end() - e);
    resize(size() - (e - b));
  }
  return begin() + (b - begin());
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
  if (is_trivially_destructible<value_type>()) {
    size_ = 0;
  } else {
    while (size_ > 0) {
      allocator_traits<allocator_type>::destroy(this->get_allocator_(),
                                                &data()[--size_]);
    }
  }
}


template<typename T, typename Alloc>
bool operator==(const vector<T, Alloc>& a, const vector<T, Alloc>& b) noexcept(
    noexcept(declval<const typename vector<T, Alloc>::value_type>() ==
             declval<const typename vector<T, Alloc>::value_type>())) {
  auto n = a.size();
  if (n != b.size()) return false;
  auto i = a.cbegin();
  auto j = b.cbegin();
  while (n-- > 0) if (*i++ != *j++) return false;
  return true;
}

template<typename T, typename Alloc>
bool operator<(const vector<T, Alloc>& a, const vector<T, Alloc>& b) noexcept(
    noexcept(declval<const typename vector<T, Alloc>::value_type>() <
             declval<const typename vector<T, Alloc>::value_type>())) {
  auto n = min(a.size(), b.size());
  auto i = a.cbegin();
  auto j = b.cbegin();
  while (n-- > 0) {
    if (*i != *j) return *i < *j;
    ++i;
    ++j;
  }
  return a.size() < b.size();
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
