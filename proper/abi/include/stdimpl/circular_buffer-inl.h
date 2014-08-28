#ifndef _STDIMPL_CIRCULAR_BUFFER_INL_H_
#define _STDIMPL_CIRCULAR_BUFFER_INL_H_

#include <stdimpl/circular_buffer.h>
#include <algorithm>
#include <stdexcept>

_namespace_begin(std)
namespace impl {


template<typename T, typename A>
circular_buffer<T, A>::circular_buffer(const allocator_type& a)
: alloc_base(a)
{}

template<typename T, typename A>
circular_buffer<T, A>::circular_buffer(size_type n, const allocator_type& a)
: circular_buffer(a)
{
  resize(n);
}

template<typename T, typename A>
circular_buffer<T, A>::circular_buffer(size_type n, const_reference v,
                                       const allocator_type& a)
: circular_buffer(a)
{
  resize(n, v);
}

template<typename T, typename A>
template<typename InputIter>
circular_buffer<T, A>::circular_buffer(InputIter b, InputIter e,
                                       const allocator_type& a)
: circular_buffer(a)
{
  const auto dist = impl::equal_support__distance(
      b, e, typename iterator_traits<InputIter>::iterator_category());
  if (dist.first) reserve(dist.second);
  copy(b, e, back_inserter(*this));
}

template<typename T, typename A>
circular_buffer<T, A>::circular_buffer(const circular_buffer& o)
: circular_buffer(o.get_allocator())
{
  reserve(o.size());
  copy(o.begin(), o.end(), back_inserter(*this));
}

template<typename T, typename A>
circular_buffer<T, A>::circular_buffer(circular_buffer&& o)
    noexcept(is_nothrow_move_constructible<alloc_base>::value)
: alloc_base(move(o))
{
  using _namespace(std)::swap;

  swap(heap_, o.heap_);
  swap(size_, o.size_);
  swap(avail_, o.avail_);
  swap(off_, o.off_);
}

template<typename T, typename A>
circular_buffer<T, A>::circular_buffer(const circular_buffer& o,
                                       const allocator_type& a)
: circular_buffer(a)
{
  reserve(o.size());
  copy(o.begin(), o.end(), back_inserter(*this));
}

template<typename T, typename A>
circular_buffer<T, A>::circular_buffer(circular_buffer&& o,
                                       const allocator_type& a)
: circular_buffer(a)
{
  if (this->get_allocator_() == o.get_allocator_()) {
    using _namespace(std)::swap;

    swap(heap_, o.heap_);
    swap(size_, o.size_);
    swap(avail_, o.avail_);
    swap(off_, o.off_);
  } else {
    reserve(o.size());
    move(o.begin(), o.end(), back_inserter(*this));
    o.clear();
  }
}

template<typename T, typename A>
circular_buffer<T, A>::circular_buffer(initializer_list<value_type> il,
                                       const allocator_type& a)
: circular_buffer(il.begin(), il.end(), a)
{}

template<typename T, typename A>
circular_buffer<T, A>::~circular_buffer() noexcept {
  clear();
  if (avail_ > 0) this->deallocate_(heap_, avail_);
}

template<typename T, typename A>
auto circular_buffer<T, A>::operator=(const circular_buffer& o) ->
    circular_buffer& {
  reserve(o.size());
  size_type delta = min(o.size(), size());
  copy(o.begin(), o.begin() + delta, begin());
  if (delta < o.size())
    copy(o.begin() + delta, o.end(), back_inserter(*this));
  else if (delta < size())
    resize(delta);
  return *this;
}

template<typename T, typename A>
auto circular_buffer<T, A>::operator=(circular_buffer&& o) ->
    circular_buffer& {
  if (this->get_allocator_() == o.get_allocator_()) {
    swap(o);
  } else {
    reserve(o.size());
    size_type delta = min(o.size(), size());
    move(o.begin(), o.begin() + delta, begin());
    if (delta < o.size())
      move(o.begin() + delta, o.end(), back_inserter(*this));
    else if (delta < size())
      resize(delta);
  }
  return *this;
}

template<typename T, typename A>
auto circular_buffer<T, A>::operator=(initializer_list<value_type> il) ->
    circular_buffer& {
  reserve(il.size());
  size_type delta = min(il.size(), size());
  copy(il.begin(), il.begin() + delta, begin());
  if (delta < il.size())
    copy(il.begin() + delta, il.end(), back_inserter(*this));
  else if (delta < size())
    resize(delta);
  return *this;
}

template<typename T, typename A>
auto circular_buffer<T, A>::begin() noexcept -> iterator {
  return iterator(heap_begin(), heap_end(), off_);
}

template<typename T, typename A>
auto circular_buffer<T, A>::end() noexcept -> iterator {
  return begin() + size_;
}

template<typename T, typename A>
auto circular_buffer<T, A>::begin() const noexcept -> const_iterator {
  return const_iterator(heap_begin(), heap_end(), off_);
}

template<typename T, typename A>
auto circular_buffer<T, A>::end() const noexcept -> const_iterator {
  return begin() + size_;
}

template<typename T, typename A>
auto circular_buffer<T, A>::rbegin() noexcept -> reverse_iterator {
  return reverse_iterator(end());
}

template<typename T, typename A>
auto circular_buffer<T, A>::rend() noexcept -> reverse_iterator {
  return reverse_iterator(begin());
}

template<typename T, typename A>
auto circular_buffer<T, A>::rbegin() const noexcept -> const_reverse_iterator {
  return const_reverse_iterator(end());
}

template<typename T, typename A>
auto circular_buffer<T, A>::rend() const noexcept -> const_reverse_iterator {
  return const_reverse_iterator(begin());
}

template<typename T, typename A>
auto circular_buffer<T, A>::cbegin() const noexcept -> const_iterator {
  return begin();
}

template<typename T, typename A>
auto circular_buffer<T, A>::cend() const noexcept -> const_iterator {
  return end();
}

template<typename T, typename A>
auto circular_buffer<T, A>::crbegin() const noexcept ->
    const_reverse_iterator {
  return rbegin();
}

template<typename T, typename A>
auto circular_buffer<T, A>::crend() const noexcept -> const_reverse_iterator {
  return rend();
}

template<typename T, typename A>
auto circular_buffer<T, A>::size() const noexcept -> size_type {
  return size_;
}

template<typename T, typename A>
auto circular_buffer<T, A>::max_size() const noexcept -> size_type {
  return allocator_traits<typename alloc_base::allocator_type>::max_size(
      this->get_allocator_());
}

template<typename T, typename A>
auto circular_buffer<T, A>::resize(size_type desired) -> void {
  if (size_ > desired) {
    if (is_trivially_destructible<value_type>::value) {
      size_ = desired;
    } else {
      while (size_ > desired)
        pop_back();
    }
  } else if (size_ < desired) {
    reserve(desired);
    if (is_trivially_constructible<value_type>::value) {
      if (_predict_true(size_ < avail_ - off_)) {
        auto delta = min(desired - size_, avail_ - off_ - size_);
        bzero(addressof(*end()),
              delta * sizeof(value_type));
        size_ += delta;
      }
      if (size_ < desired) {
        assert(size_ > avail_ - off_);
        auto delta = desired - size_;
        bzero(heap_begin(), delta * sizeof(value_type));
        size_ += delta;
      }
    } else {
      for (auto p = heap_begin() + off_ + size_;
           p != heap_end() && size_ < desired;
           ++p, ++desired) {
        allocator_traits<allocator_type>::construct(this->get_allocator_(),
                                                    p);
        ++size_;
      }
      for (auto p = heap_begin();
           size_ < desired;
           ++p, ++desired) {
        allocator_traits<allocator_type>::construct(this->get_allocator_(),
                                                    p);
        ++size_;
      }
    }
  }
}

template<typename T, typename A>
auto circular_buffer<T, A>::resize(size_type desired, const_reference v) ->
    void {
  if (size_ > desired) {
    if (is_trivially_destructible<value_type>::value) {
      size_ = desired;
    } else {
      while (size_ > desired)
        pop_back();
    }
  } else if (size_ < desired) {
    reserve(desired);
    for (auto p = heap_begin() + off_ + size_;
         p != heap_end() && size_ < desired;
         ++p, ++desired) {
      allocator_traits<allocator_type>::construct(this->get_allocator_(),
                                                  p, v);
      ++size_;
    }
    for (auto p = heap_begin();
         size_ < desired;
         ++p, ++desired) {
      allocator_traits<allocator_type>::construct(this->get_allocator_(),
                                                  p, v);
      ++size_;
    }
  }
}

template<typename T, typename A>
auto circular_buffer<T, A>::capacity() const noexcept -> size_type {
  return avail_;
}

template<typename T, typename A>
auto circular_buffer<T, A>::empty() const noexcept -> bool {
  return size() == 0;
}

template<typename T, typename A>
auto circular_buffer<T, A>::reserve(size_type rsv) -> void {
  /*
   * We need avail_ to be at least rsv+1,
   * since we need an empty space where the past-the-end iterator
   * can point at.
   */
  rsv += 1;
  if (avail_ >= rsv) return;
  size_type new_sz = max(rsv, 2 * capacity());

  /*
   * Try resize extension.
   *
   * We only use the resize extension if we can safely re-organize the
   * heap.  I.e. constructing and destroying elements won't throw.
   */
  if (size_ < avail_ - off_ ||  // No altering of layout.
      ((is_nothrow_move_constructible<value_type>::value ||
        is_nothrow_copy_constructible<value_type>::value) &&
       is_nothrow_destructible<value_type>::value)) {
    bool realloc = allocator_traits<allocator_type>::resize(
        this->get_allocator_(), heap_, avail_, new_sz);
    if (!realloc && rsv != new_sz) {
      realloc = allocator_traits<allocator_type>::resize(
          this->get_allocator_(), heap_, avail_, new_sz);
    }

    if (realloc) {
      if (size_ > avail_ - off_) {
        auto op = [this](value_type* s, value_type* d) noexcept {
                    allocator_traits<allocator_type>::construct(
                        this->get_allocator_(),
                        d, move_if_noexcept(*s));
                    s->~value_type();
                  };

        auto dst = heap_begin() + avail_;
        const auto src = heap_begin();
        size_t count = size_ - (avail_ - off_);
        for (auto i = src; count > 0; ++i, ++dst, --count)
          op(i, dst);
      }

      avail_ = new_sz;
      return;
    }
  }

  circular_buffer temp(this->get_allocator_());
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

template<typename T, typename A>
auto circular_buffer<T, A>::shrink_to_fit() -> void {
  if (size_ == 0) {
    if (avail_ > 0) this->deallocate_(heap_, avail_);
    avail_ = 0;
    off_ = 0;
    heap_ = nullptr;
    return;
  }

  if (avail_ - off_ - 1U > size_ &&
      allocator_traits<allocator_type>::resize(this->get_allocator_(),
                                               heap_, avail_,
                                               off_ + size_ + 1U)) {
    avail_ = off_ + size_ + 1U;
    return;
  }
}

template<typename T, typename A>
auto circular_buffer<T, A>::operator[](size_type idx) noexcept -> reference {
  assert(idx < size_);
  return begin()[idx];
}

template<typename T, typename A>
auto circular_buffer<T, A>::operator[](size_type idx) const noexcept ->
    const_reference {
  assert(idx < size_);
  return begin()[idx];
}

template<typename T, typename A>
auto circular_buffer<T, A>::at(size_type idx) -> reference {
  if (idx >= size()) throw out_of_range("circular_buffer::at");
  return (*this)[idx];
}

template<typename T, typename A>
auto circular_buffer<T, A>::at(size_type idx) const -> const_reference {
  if (idx >= size()) throw out_of_range("circular_buffer::at");
  return (*this)[idx];
}

template<typename T, typename A>
auto circular_buffer<T, A>::front() noexcept -> reference {
  assert(!empty());
  return *begin();
}

template<typename T, typename A>
auto circular_buffer<T, A>::front() const noexcept -> const_reference {
  assert(!empty());
  return *begin();
}

template<typename T, typename A>
auto circular_buffer<T, A>::back() noexcept -> reference {
  assert(!empty());
  return *rbegin();
}

template<typename T, typename A>
auto circular_buffer<T, A>::back() const noexcept -> const_reference {
  assert(!empty());
  return *rbegin();
}

template<typename T, typename A>
template<typename... Args>
auto circular_buffer<T, A>::emplace_front(Args&&... args) -> void {
  reserve(size() + 1U);
  size_type new_off = (off_ == 0 ? avail_ : off_) - 1U;
  allocator_traits<allocator_type>::construct(this->get_allocator_(),
                                              heap_begin() + new_off,
                                              forward<Args>(args)...);
  ++size_;
  off_ = new_off;
}

template<typename T, typename A>
auto circular_buffer<T, A>::push_front(const_reference v) -> void {
  emplace_front(v);
}

template<typename T, typename A>
auto circular_buffer<T, A>::push_front(value_type&& v) -> void {
  emplace_front(move(v));
}

template<typename T, typename A>
auto circular_buffer<T, A>::pop_front() noexcept -> void {
  assert(!empty());
  if (size_ > 0) {
    allocator_traits<allocator_type>::destroy(this->get_allocator_(),
                                              heap_begin() + off_);
    ++off_;
    --size_;
  }
}

template<typename T, typename A>
template<typename... Args>
auto circular_buffer<T, A>::emplace_back(Args&&... args) -> void {
  reserve(size() + 1U);
  const size_type wrap_idx = avail_ - off_;
  const size_type placement_idx = (size_ >= wrap_idx ?
                                   size_ - wrap_idx :
                                   size_);

  allocator_traits<allocator_type>::construct(this->get_allocator_(),
                                              heap_begin() + placement_idx,
                                              forward<Args>(args)...);
  ++size_;
}

template<typename T, typename A>
auto circular_buffer<T, A>::push_back(const_reference v) -> void {
  emplace_back(v);
}

template<typename T, typename A>
auto circular_buffer<T, A>::push_back(value_type&& v) -> void {
  emplace_back(move(v));
}

template<typename T, typename A>
auto circular_buffer<T, A>::pop_back() noexcept -> void {
  assert(!empty());
  if (size_ > 0) {
    const size_type wrap_idx = avail_ - off_;
    const size_type placement_idx = (size_ - 1U >= wrap_idx ?
                                     size_ - 1U - wrap_idx :
                                     size_ - 1U);
    allocator_traits<allocator_type>::destroy(this->get_allocator_(),
                                              heap_begin() + placement_idx);
    --size_;
  }
}

template<typename T, typename A>
template<typename... Args>
auto circular_buffer<T, A>::emplace(const_iterator pos, Args&&... args) ->
    iterator {
  size_type dist = pos - cbegin();
  assert(dist <= size());

  if (dist < size_ / 2) {
    emplace_front(forward<Args>(args)...);
    rotate(begin(), next(begin()), next(begin(), dist + 1U));
  } else {
    emplace_back(forward<Args>(args)...);
    rotate(begin() + dist, prev(end()), end());
  }
  return begin() + dist;
}

template<typename T, typename A>
auto circular_buffer<T, A>::insert(const_iterator pos, const_reference v) ->
    iterator {
  return emplace(pos, v);
}

template<typename T, typename A>
auto circular_buffer<T, A>::insert(const_iterator pos, value_type&& v) ->
    iterator {
  return emplace(pos, move(v));
}

template<typename T, typename A>
auto circular_buffer<T, A>::insert(const_iterator pos,
                                   size_type n, const_reference v) ->
    iterator {
  size_type dist = pos - cbegin();
  assert(dist <= size());
  reserve(size() + n);

  if (dist < size_ / 2) {
    if (is_nothrow_copy_constructible<value_type>::value) {
      size_type front_len = min(n, off_);
      size_type back_len = n - front_len;

      auto op = [this, &v](value_type& ref) noexcept {
                  allocator_traits<allocator_type>::construct(
                      this->get_allocator_(), addressof(ref), v);
                };

      for_each(heap_begin() + off_ - front_len, heap_begin() + off_, ref(op));
      for_each(heap_end() - back_len, heap_end(), ref(op));
      off_ = (back_len != 0 ? avail_ - back_len : off_ - front_len);
      size_ += n;
    } else {
      for (size_type i = 0; i < n; ++i) push_front(v);
    }
    rotate(begin(), next(begin(), n), next(begin(), n + dist));
  } else {
    for (size_type i = 0; i < n; ++i) push_back(v);
    rotate(begin() + dist, end() - n, end());
  }
  return begin() + dist;
}

template<typename T, typename A>
template<typename InputIter>
auto circular_buffer<T, A>::insert(const_iterator pos,
                                   InputIter b, InputIter e) -> iterator {
  const auto dist = impl::equal_support__distance(
      b, e, typename iterator_traits<InputIter>::iterator_category());
  const size_type off = pos - cbegin();
  if (!dist.first) {
    copy(b, e, inserter(*this, begin() + off));
    return begin() + off;
  }

  reserve(size() + dist.second);
  if (off < size_ / 2) {
    if (is_nothrow_constructible<value_type,
                                 typename iterator_traits<InputIter>::reference
                                >::value) {
      size_type front_len = min(size_type(dist.second), off_);
      size_type back_len = dist.second - front_len;

      auto op = [this, &b](value_type& ref) noexcept {
                  allocator_traits<allocator_type>::construct(
                      this->get_allocator_(), addressof(ref), *b);
                  ++b;
                };

      for_each(heap_begin() + off_ - front_len, heap_begin() + off_, ref(op));
      for_each(heap_end() - back_len, heap_end(), ref(op));
      assert(b == e);
      off_ = (back_len != 0 ? avail_ - back_len : off_ - front_len);
      size_ += dist.second;

      rotate(begin(),
             next(begin(), dist.second),
             next(begin(), dist.second + off));
    } else {
      copy(b, e, inserter(*this, begin() + off));
    }
  } else {
    copy(b, e, back_inserter(*this));
    rotate(begin() + off, end() - dist.second, end());
  }

  return begin() + off;
}

template<typename T, typename A>
auto circular_buffer<T, A>::insert(const_iterator pos,
                                   initializer_list<value_type> il) ->
    iterator {
  return insert(pos, il.begin(), il.end());
}

template<typename T, typename A>
auto circular_buffer<T, A>::erase(const_iterator pos_) -> iterator {
  assert(pos_ != cend());

  size_type dist = pos_ - cbegin();
  iterator pos = begin() + dist;
  if (dist < size_ / 2) {
    move_backward(begin(), pos, next(pos));
    pop_front();
  } else {
    move(next(pos), end(), pos);
    pop_back();
  }
  return pos;
}

template<typename T, typename A>
auto circular_buffer<T, A>::erase(const_iterator b_, const_iterator e_) ->
    iterator {
  const size_type dist_before = b_ - cbegin();
  const size_type dist_after = cend() - e_;

  iterator b = begin() + dist_before;
  iterator e = end() - dist_after;

  if (dist_before < dist_after) {
    iterator new_begin = move_backward(begin(), b, e);
    if (is_trivially_destructible<value_type>::value) {
      auto delta = size_type(new_begin - begin());
      size_ -= delta;
      if (avail_ - off_ < delta)
        off_ = delta - (avail_ - off_);
      else
        off_ += delta;
    } else {
      while (begin() != new_begin)
        pop_front();
    }
  } else {
    iterator new_end = move(e, end(), b);
    if (is_trivially_destructible<value_type>::value) {
      size_ -= end() - new_end;
    } else {
      while (end() != new_end)
        pop_back();
    }
  }
  return b;
}

template<typename T, typename A>
auto circular_buffer<T, A>::swap(circular_buffer& o)
    noexcept(noexcept(declval<circular_buffer>().swap_(o))) -> void {
  using _namespace(std)::swap;

  this->alloc_base::swap_(o);
  swap(heap_, o.heap_);
  swap(size_, o.size_);
  swap(avail_, o.avail_);
  swap(off_, o.off_);
}

template<typename T, typename A>
auto circular_buffer<T, A>::clear() noexcept -> void {
  resize(0);
}

template<typename T, typename A>
auto circular_buffer<T, A>::heap_begin() noexcept -> value_type* {
  return addressof(*heap_);
}

template<typename T, typename A>
auto circular_buffer<T, A>::heap_begin() const noexcept -> const value_type* {
  return addressof(*heap_);
}

template<typename T, typename A>
auto circular_buffer<T, A>::heap_end() noexcept -> value_type* {
  return heap_begin() + avail_;
}

template<typename T, typename A>
auto circular_buffer<T, A>::heap_end() const noexcept -> const value_type* {
  return heap_begin() + avail_;
}


template<typename T>
_circular_buffer_iterator<T>::_circular_buffer_iterator(
    pointer b, pointer e, make_unsigned_t<difference_type> off)
    noexcept
: heap_begin_(b),
  heap_end_(e),
  impl_(b + off)
{
  assert(b <= impl_ && impl_ < e);
}

template<typename T>
template<typename U>
_circular_buffer_iterator<T>::_circular_buffer_iterator(
    const _circular_buffer_iterator<U>& o,
    enable_if_t<is_convertible<typename _circular_buffer_iterator<U>::pointer,
                               pointer>::value,
                int>) noexcept
: heap_begin_(o.heap_begin_),
  heap_end_(o.heap_end_),
  impl_(o.impl_)
{}

template<typename T>
auto _circular_buffer_iterator<T>::operator*() const noexcept -> reference {
  return *impl_;
}

template<typename T>
auto _circular_buffer_iterator<T>::operator->() const noexcept -> pointer {
  return impl_;
}

template<typename T>
auto _circular_buffer_iterator<T>::operator++() noexcept ->
    _circular_buffer_iterator& {
  assert(heap_begin_ != heap_end_);
  ++impl_;
  if (impl_ == heap_end_) impl_ = heap_begin_;
  return *this;
}

template<typename T>
auto _circular_buffer_iterator<T>::operator++(int) noexcept ->
    _circular_buffer_iterator {
  _circular_buffer_iterator tmp = *this;
  ++*this;
  return tmp;
}

template<typename T>
auto _circular_buffer_iterator<T>::operator--() noexcept ->
    _circular_buffer_iterator& {
  assert(heap_begin_ != heap_end_);
  if (impl_ == heap_begin_) impl_ = heap_end_;
  --impl_;
  return *this;
}

template<typename T>
auto _circular_buffer_iterator<T>::operator--(int) noexcept ->
    _circular_buffer_iterator {
  _circular_buffer_iterator tmp = *this;
  --*this;
  return tmp;
}

template<typename T>
auto _circular_buffer_iterator<T>::operator+=(difference_type dist) noexcept ->
    _circular_buffer_iterator& {
  const auto max_dist = heap_end_ - heap_begin_;
  assert(dist < max_dist);

  impl_ += dist;
  if (impl_ < heap_begin_) impl_ += max_dist;
  if (impl_ > heap_end_) impl_ -= max_dist;
  return *this;
}

template<typename T>
auto _circular_buffer_iterator<T>::operator-=(difference_type dist) noexcept ->
    _circular_buffer_iterator& {
  return *this += -dist;
}

template<typename T>
auto _circular_buffer_iterator<T>::operator+(difference_type dist)
    const noexcept -> _circular_buffer_iterator {
  _circular_buffer_iterator clone = *this;
  clone += dist;
  return clone;
}

template<typename T>
auto _circular_buffer_iterator<T>::operator-(difference_type dist)
    const noexcept -> _circular_buffer_iterator {
  _circular_buffer_iterator clone = *this;
  clone -= dist;
  return clone;
}

template<typename T>
auto _circular_buffer_iterator<T>::operator-(
    const _circular_buffer_iterator& rhs) const noexcept -> difference_type {
  const auto max_dist = heap_end_ - heap_begin_;
  difference_type delta = impl_ - rhs.impl_;
  if (delta < 0) delta += max_dist;
  return delta;
}

template<typename T>
auto _circular_buffer_iterator<T>::operator[](difference_type idx)
    const noexcept -> reference {
  return *(*this + idx);
}

template<typename T>
template<typename U>
bool _circular_buffer_iterator<T>::operator==(
    const _circular_buffer_iterator<U>& rhs) const noexcept {
  return impl_ == rhs.impl_;
}

template<typename T>
template<typename U>
bool _circular_buffer_iterator<T>::operator!=(
    const _circular_buffer_iterator<U>& rhs) const noexcept {
  return !(*this == rhs);
}


template<typename T, typename A>
bool operator==(const circular_buffer<T, A>& a, const circular_buffer<T, A>& b)
    noexcept(noexcept(
        declval<const typename circular_buffer<T, A>::value_type>() ==
        declval<const typename circular_buffer<T, A>::value_type>())) {
  return a.size() == b.size() &&
         equal(a.begin(), a.end(), b.begin(), b.end());
}

template<typename T, typename A>
bool operator<(const circular_buffer<T, A>& a, const circular_buffer<T, A>& b)
    noexcept(noexcept(
        declval<const typename circular_buffer<T, A>::value_type>() <
        declval<const typename circular_buffer<T, A>::value_type>())) {
  return lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
}

template<typename T, typename A>
bool operator!=(const circular_buffer<T, A>& a, const circular_buffer<T, A>& b)
    noexcept(noexcept(!(a == b))) {
  return !(a == b);
}

template<typename T, typename A>
bool operator>(const circular_buffer<T, A>& a, const circular_buffer<T, A>& b)
    noexcept(noexcept(b < a)) {
  return b < a;
}

template<typename T, typename A>
bool operator>=(const circular_buffer<T, A>& a, const circular_buffer<T, A>& b)
    noexcept(noexcept(!(a < b))) {
  return !(a < b);
}

template<typename T, typename A>
bool operator<=(const circular_buffer<T, A>& a, const circular_buffer<T, A>& b)
    noexcept(noexcept(!(b < a))) {
  return !(b < a);
}

template<typename T, typename A>
void swap(const circular_buffer<T, A>& a, const circular_buffer<T, A>& b)
    noexcept(noexcept(a.swap(b))) {
  a.swap(b);
}


} /* namespace std::impl */
_namespace_end(std)

#endif /* _STDIMPL_CIRCULAR_BUFFER_INL_H_ */
