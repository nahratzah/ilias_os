#ifndef _DEQUE_INL_H_
#define _DEQUE_INL_H_

#include <deque>
#include <algorithm>
#include <utility>
#include <stdimpl/alloc_deleter.h>
#include <abi/misc_int.h>

_namespace_begin(std)
namespace impl {


template<size_t TSize, size_t TAlign, typename A>
deque_storage_base<TSize, TAlign, A>::deque_storage_base(
    const allocator_type& a)
: alloc_base(a),
  data_(a)
{}

template<size_t TSize, size_t TAlign, typename A>
deque_storage_base<TSize, TAlign, A>::deque_storage_base(
    deque_storage_base&& o)
: alloc_base(move(o)),
  data_(move(o.data_)),
  off_(exchange(o.off_, 0)),
  size_(exchange(o.size_, 0))
{}

template<size_t TSize, size_t TAlign, typename A>
deque_storage_base<TSize, TAlign, A>::~deque_storage_base() noexcept {
  assert(empty());
  for_each(data_ptr_begin(), data_ptr_end(),
           alloc_deleter_visitor(this->get_allocator_()));
}

template<size_t TSize, size_t TAlign, typename A>
auto deque_storage_base<TSize, TAlign, A>::swap(deque_storage_base& o)
    noexcept -> void {
  using _namespace(std)::swap;

  this->alloc_base::swap_(o);
  data_.swap(o.data_);
  swap(size_, o.size_);
  swap(off_, o.off_);
}

template<size_t TSize, size_t TAlign, typename A>
auto deque_storage_base<TSize, TAlign, A>::empty() const noexcept -> bool {
  return size() == 0;
}

template<size_t TSize, size_t TAlign, typename A>
auto deque_storage_base<TSize, TAlign, A>::size() const noexcept -> size_type {
  return size_;
}

template<size_t TSize, size_t TAlign, typename A>
auto deque_storage_base<TSize, TAlign, A>::max_size() const -> size_type {
  using alloc_traits = allocator_traits<typename alloc_base::allocator_type>;
  using abi::umul_overflow;

  /* Calculate max # of elements allocator can provide;
   * clamp to max of size_type. */
  auto alloc_max_size = alloc_traits::max_size(this->get_allocator_());
  if (alloc_max_size > numeric_limits<size_type>::max())
     alloc_max_size = numeric_limits<size_type>::max();

  /* Calculate max # of elements data_type can hold;
   * clamp to max of size_type. */
  auto data_max_size = data_.max_size();
  if (data_max_size > numeric_limits<size_type>::max())
    data_max_size = numeric_limits<size_type>::max();

  /* Calculate max # of elements we can actually hold:
   * the minimum of { alloc_max_size, data_max_size }
   * multiplied by number of items per element. */
  size_type data_max_items;
  if (umul_overflow(min(size_type(alloc_max_size), size_type(data_max_size)),
                    block_items_(),
                    &data_max_items))
    data_max_items = numeric_limits<size_type>::max();

  return data_max_items;
}

template<size_t TSize, size_t TAlign, typename A>
auto deque_storage_base<TSize, TAlign, A>::capacity() const noexcept ->
    size_type {
  size_type avail = data_.size() * block_items_();
  size_type maybe_unavail = max(front_avail_() % block_items_(),
                                back_avail_() % block_items_());

  return avail - maybe_unavail;
}

template<size_t TSize, size_t TAlign, typename A>
auto deque_storage_base<TSize, TAlign, A>::reserve(size_type rsv) -> void {
  void* hint = (data_.empty() ?
                static_cast<void*>(this) :
                static_cast<void*>(&data_.back()));
  for (auto cur_cap = capacity(); cur_cap < rsv; cur_cap += block_items_()) {
    auto ptr = new_alloc_deleter<elem_type>(this->get_allocator_(), hint);
    hint = ptr.get();
    data_.push_back(ptr.get());
    ptr.release();
  }
}

template<size_t TSize, size_t TAlign, typename A>
auto deque_storage_base<TSize, TAlign, A>::shrink_to_fit() -> void {
  shrink_to_fit_before_();
  shrink_to_fit_after_();
  data_.shrink_to_fit();
}

template<size_t TSize, size_t TAlign, typename A>
template<typename T>
auto deque_storage_base<TSize, TAlign, A>::front() noexcept ->
    add_lvalue_reference_t<T> {
  assert(!empty());
  return *begin<T>();
}

template<size_t TSize, size_t TAlign, typename A>
template<typename T>
auto deque_storage_base<TSize, TAlign, A>::front() const noexcept ->
    add_lvalue_reference_t<add_const_t<T>> {
  assert(!empty());
  return *begin<T>();
}

template<size_t TSize, size_t TAlign, typename A>
template<typename T>
auto deque_storage_base<TSize, TAlign, A>::back() noexcept ->
    add_lvalue_reference_t<T> {
  assert(!empty());
  return *prev(end<T>());
}

template<size_t TSize, size_t TAlign, typename A>
template<typename T>
auto deque_storage_base<TSize, TAlign, A>::back() const noexcept ->
    add_lvalue_reference_t<add_const_t<T>> {
  assert(!empty());
  return *prev(end<T>());
}

template<size_t TSize, size_t TAlign, typename A>
template<typename T, typename... Args>
auto deque_storage_base<TSize, TAlign, A>::emplace_front(Args&&... args) ->
    void {
  static_assert(sizeof(T) <= TSize && TAlign % alignof(T) == 0,
                "Storage cannot hold instance of this type.");

  ensure_front_avail_(1);

  iterator<T> e = prev(begin<T>());
  new (static_cast<void*>(addressof(*e))) T(forward<Args>(args)...);
  --off_;
  ++size_;
}

template<size_t TSize, size_t TAlign, typename A>
template<typename T, typename... Args>
auto deque_storage_base<TSize, TAlign, A>::emplace_back(Args&&... args) ->
    void {
  static_assert(sizeof(T) <= TSize && TAlign % alignof(T) == 0,
                "Storage cannot hold instance of this type.");

  ensure_back_avail_(1);

  iterator<T> e = end<T>();
  new (static_cast<void*>(addressof(*e))) T(forward<Args>(args)...);
  ++size_;
}

template<size_t TSize, size_t TAlign, typename A>
template<typename T, typename... Args>
auto deque_storage_base<TSize, TAlign, A>::emplace(const_iterator<T> pos,
                                                   Args&&... args) ->
    iterator<T> {
  static_assert(sizeof(T) <= TSize && TAlign % alignof(T) == 0,
                "Storage cannot hold instance of this type.");

  const size_type dist = pos - cbegin<T>();

  if (block_items_() == 1U) {
    ensure_back_avail_(1);

    /* Make the underlying data type (i.e. the circular buffer)
     * go through the hard work of inserting the element at the right
     * position. */
    auto p = data_.back();
    ::new (p) T(forward<Args>(args)...);
    data_.pop_back();
    data_.insert(next(data_.begin(), off_ + dist), p);
    ++size_;
    return begin<T>() + dist;
  } else if (dist < size() / 2U) {
    emplace_front<T>(forward<Args>(args)...);
    return rotate(begin<T>(), next(begin<T>()), next(begin<T>(), dist + 1U));
  } else {
    emplace_back<T>(forward<Args>(args)...);
    return rotate(begin<T>() + dist, prev(end<T>()), end<T>());
  }
}

/*
 * Insert: bidirectional iterator code.
 */
template<size_t TSize, size_t TAlign, typename A>
template<typename T, typename Iter>
auto deque_storage_base<TSize, TAlign, A>::insert(const_iterator<T> pos,
                                                  Iter b, Iter e) ->
    enable_if_t<is_base_of<bidirectional_iterator_tag,
                           typename iterator_traits<Iter>::iterator_category
                          >::value,
                iterator<T>> {
  static_assert(sizeof(T) <= TSize && TAlign % alignof(T) == 0,
                "Storage cannot hold instance of this type.");

  const size_type dist = pos - cbegin<T>();

  if (dist < size() / 2) {
    size_t count;
    for (count = 0; b != e; --e, ++count)
      emplace_front<T>(*prev(e));
    return rotate(begin<T>(),
                  next(begin<T>(), count),
                  next(begin<T>(), count + dist));
  } else {
    size_t count;
    for (count = 0; b != e; ++b, ++count)
      emplace_back<T>(*b);
    iterator<T> rv = next(begin<T>(), dist);
    rotate(rv,
           prev(end<T>(), count),
           end<T>());
    return rv;
  }
}

/*
 * Insert: generic code.
 */
template<size_t TSize, size_t TAlign, typename A>
template<typename T, typename Iter>
auto deque_storage_base<TSize, TAlign, A>::insert(const_iterator<T> pos,
                                                  Iter b, Iter e) ->
    enable_if_t<!is_base_of<bidirectional_iterator_tag,
                            typename iterator_traits<Iter>::iterator_category
                           >::value,
                iterator<T>> {
  static_assert(sizeof(T) <= TSize && TAlign % alignof(T) == 0,
                "Storage cannot hold instance of this type.");

  const size_type dist = pos - cbegin<T>();

  if (dist < size() / 2) {
    size_t count;
    for (count = 0; b != e; ++b, ++count)
      emplace_front<T>(*b);
    reverse(begin<T>(), next(begin<T>(), count));
    return rotate(begin<T>(),
                  next(begin<T>(), count),
                  next(begin<T>(), count + dist));
  } else {
    size_t count;
    for (count = 0; b != e; ++b, ++count)
      emplace_back<T>(*b);
    iterator<T> rv = next(begin<T>(), dist);
    rotate(rv,
           prev(end<T>(), count),
           end<T>());
    return rv;
  }
}

template<size_t TSize, size_t TAlign, typename A>
template<typename T>
auto deque_storage_base<TSize, TAlign, A>::insert_n(const_iterator<T> pos,
                                                    size_type n, const T& v) ->
    iterator<T> {
  static_assert(sizeof(T) <= TSize && TAlign % alignof(T) == 0,
                "Storage cannot hold instance of this type.");

  const size_type dist = pos - cbegin<T>();

  if (dist < size() / 2) {
    for (size_type i = 0; i < n; ++i)
      emplace_front<T>(v);
    return rotate(begin<T>(),
                  next(begin<T>(), n),
                  next(begin<T>(), n + dist));
  } else {
    for (size_type i = 0; i < n; ++i)
      emplace_back<T>(v);
    iterator<T> rv = next(begin<T>(), dist);
    rotate(rv,
           prev(end<T>(), n),
           end<T>());
    return rv;
  }
}

template<size_t TSize, size_t TAlign, typename A>
template<typename T>
auto deque_storage_base<TSize, TAlign, A>::pop_front() noexcept -> void {
  static_assert(sizeof(T) <= TSize && TAlign % alignof(T) == 0,
                "Storage cannot hold instance of this type.");

  assert(!empty());

  if (!empty()) {
    front<T>().~T();
    ++off_;
    --size_;
  }
}

template<size_t TSize, size_t TAlign, typename A>
template<typename T>
auto deque_storage_base<TSize, TAlign, A>::pop_back() noexcept -> void {
  static_assert(sizeof(T) <= TSize && TAlign % alignof(T) == 0,
                "Storage cannot hold instance of this type.");

  assert(!empty());

  if (!empty()) {
    back<T>().~T();
    --size_;
  }
}

template<size_t TSize, size_t TAlign, typename A>
template<typename T>
auto deque_storage_base<TSize, TAlign, A>::erase(const_iterator<T> i)
    noexcept -> iterator<T> {
  static_assert(sizeof(T) <= TSize && TAlign % alignof(T) == 0,
                "Storage cannot hold instance of this type.");

  const size_type dist = i - cbegin<T>();
  assert(dist < size());

  if (dist == 0) {
    pop_front<T>();
    return begin<T>();
  } else if (dist + 1U == size()) {
    pop_back<T>();
    return end<T>();
  }

  if (block_items_() == 1U) {
    /* Erase from underlying storage instead. */
    next(begin<T>(), dist)->~T();
    auto p_pos = next(data_.begin(), off_ + dist);
    auto p = *p_pos;
    data_.erase(p_pos);
    data_.push_back(p);
    --size_;
  } else if (dist < size() / 2U) {
    rotate(begin<T>(), next(begin<T>(), dist), next(begin<T>(), dist + 1U));
    pop_front<T>();
  } else {
    rotate(next(begin<T>(), dist), next(begin<T>(), dist + 1U), end<T>());
    pop_back<T>();
  }

  return begin<T>() + dist;
}

template<size_t TSize, size_t TAlign, typename A>
template<typename T>
auto deque_storage_base<TSize, TAlign, A>::erase(const_iterator<T> b,
                                                 const_iterator<T> e)
    noexcept -> iterator<T> {
  size_type n = e - b;
  assert(n <= size());

  const size_type dist_b = b - cbegin<T>();
  if (n == 0) return begin<T>() + dist_b;
  const size_type dist_e = cend<T>() - e;
  assert(dist_b <= size() && dist_e <= size());

  if (block_items_() == 1U) {
    /* Rotate underlying data. */
    for_each(b, e, [](const T& v) { v.~T(); });
    if (dist_b <= dist_e) {
      auto d_beg = next(data_.begin(), off_);
      auto d_mid = next(d_beg, dist_b);
      auto d_end = next(d_mid, n);
      rotate(d_beg, d_mid, d_end);
      off_ += n;
      size_ -= n;
    } else {
      auto d_beg = next(data_.begin(), off_ + dist_b);
      auto d_mid = next(d_beg, n);
      auto d_end = next(d_mid, dist_e);
      rotate(d_beg, d_mid, d_end);
      size_ -= n;
    }
    return begin<T>() + dist_b;
  }

  size_type off_b = off_ + dist_b;
  size_type off_e = off_b + n;
  size_type block_b = off_b / block_items_();
  size_type block_b_off = off_b % block_items_();
  size_type block_e = off_e / block_items_();
  size_type block_e_off = off_e % block_items_();

  if (dist_b <= dist_e) {
    /* Align to-be-removed range to the start of a block. */
    if (block_b_off != 0) {
      auto src_end = next(begin<T>(), dist_b);
      auto src_begin = prev(src_end, block_b_off);
      auto dst_end = prev(end<T>(), dist_e);
      move_backward(src_begin, src_end, dst_end);

      /* Update variables. */
      if (block_e_off < block_b_off) {
        --block_e;
        block_e_off += block_items_();
      }
      off_b -= block_b_off;
      off_e -= block_e_off;
      block_e_off -= block_b_off;
      block_b_off = 0;
    }

    /*
     * NOTE:
     * dist_b, dist_e no longer describe the to-be-erased range!
     */

    /* Move entire blocks out of the range. */
    if (block_b < block_e) {
      const size_type block_n = (block_e - block_b) * block_items_();
      for_each(next(begin<T>(), block_b * block_items_() - off_),
               next(begin<T>(), block_e * block_items_() - off_),
               [](T& v) { v.~T(); });
      rotate(next(data_.begin(), off_ / block_items_()),
             next(data_.begin(), block_b),
             next(data_.begin(), block_e));

      off_ += block_n;
      off_b += block_n;
      size_ -= block_n;
      n -= block_n;
    }

    /* Remove non-block-size range to begin. */
    auto r_beg = begin<T>();
    auto r_mid = next(r_beg, off_b - off_);
    auto r_end = next(r_mid, n);
    move_backward(r_beg, r_mid, r_end);

    /* Destroy moved-away elements at start of list. */
    for_each(begin<T>(), next(begin<T>(), n),
             [](T& v) { v.~T(); });
    off_ += n;
    size_ -= n;
  } else {
    /* Align to-be-removed range to the end of a block. */
    if (block_e_off != 0) {
      const size_type shift = block_items_() - block_e_off;
      auto src_begin = prev(end<T>(), dist_e);
      auto src_end = next(src_begin, shift);
      auto dst_begin = next(begin<T>(), dist_b);
      move(src_begin, src_end, dst_begin);

      /* Update variables. */
      off_b += shift;
      off_e += shift;
      block_b_off += shift;
      block_b += block_b_off / block_items_();
      block_b_off %= block_items_();
      ++block_e;
      block_e_off = 0;
    }

    /*
     * NOTE:
     * dist_b, dist_e no longer describe the to-be-erased range!
     */

    /* Move entire blocks out of the range. */
    block_b += (block_b_off == 0 ? 0 : 1);
    if (block_b < block_e) {
      const size_type block_n = (block_e - block_b) * block_items_();
      for_each(next(begin<T>(), block_b * block_items_() - off_),
               next(begin<T>(), block_e * block_items_() - off_),
               [](T& v) { v.~T(); });
      rotate(next(data_.begin(), block_b),
             next(data_.begin(), block_e),
             prev(data_.end(), back_avail_() / block_items_()));

      size_ -= block_n;
      n -= block_n;
    }

    /* Move non-block-size range to end. */
    auto r_beg = next(begin<T>(), off_b - off_);
    auto r_mid = next(r_beg, n);
    auto r_end = end<T>();
    move(r_mid, r_end, r_beg);

    /* Destroy moved-away elements at end of list. */
    for_each(prev(end<T>(), n), end<T>(),
             [](T& v) { v.~T(); });
    size_ -= n;
  }

  return begin<T>() + dist_b;
}

template<size_t TSize, size_t TAlign, typename A>
template<typename T>
auto deque_storage_base<TSize, TAlign, A>::clear() noexcept -> void {
  for_each(begin<T>(), end<T>(),
           [](T& v) { v.~T(); });
  size_ = 0;
  off_ = 0;
}

template<size_t TSize, size_t TAlign, typename A>
auto deque_storage_base<TSize, TAlign, A>::front_avail_() const noexcept ->
    size_type {
  return off_;
}

template<size_t TSize, size_t TAlign, typename A>
auto deque_storage_base<TSize, TAlign, A>::back_avail_() const noexcept ->
    size_type {
  return (data_.size() * block_items_()) - (off_ + size_);
}

template<size_t TSize, size_t TAlign, typename A>
auto deque_storage_base<TSize, TAlign, A>::ensure_front_avail_(size_type n) ->
    void {
  size_type avail = front_avail_();
  if (avail >= n) return;

  for (size_type move_avail = back_avail_();
       avail < n && move_avail >= block_items_();
       move_avail -= block_items_(), avail += block_items_()) {
    auto p = data_.back();
    data_.pop_back();
    data_.push_front(p);
    off_ += block_items_();
  }

  while (avail < n) {
    void* hint = (data_.empty() ?
                  static_cast<void*>(this) :
                  static_cast<void*>(&data_.front()));
    auto ptr = new_alloc_deleter<elem_type>(this->get_allocator_(), hint);
    data_.push_front(ptr.get());
    off_ += block_items_();
    ptr.release();

    avail += block_items_();
  }
}

template<size_t TSize, size_t TAlign, typename A>
auto deque_storage_base<TSize, TAlign, A>::ensure_back_avail_(size_type n) ->
    void {
  size_type avail = back_avail_();
  if (avail >= n) return;

  for (size_type move_avail = front_avail_();
       avail < n && move_avail >= block_items_();
       move_avail -= block_items_(), avail += block_items_()) {
    auto p = data_.front();
    data_.pop_front();
    data_.push_back(p);
    off_ -= block_items_();
  }

  while (avail < n) {
    void* hint = (data_.empty() ?
                  static_cast<void*>(this) :
                  static_cast<void*>(&data_.back()));
    auto ptr = new_alloc_deleter<elem_type>(this->get_allocator_(), hint);
    data_.push_back(ptr.get());
    ptr.release();

    avail += block_items_();
  }
}

template<size_t TSize, size_t TAlign, typename A>
template<typename T>
auto deque_storage_base<TSize, TAlign, A>::begin() noexcept -> iterator<T> {
  using data_iterator = typename defn_::data_iterator;
  using defn_iterator = typename defn_::iterator;
  return iterator<T>(defn_iterator(data_iterator(data_ptr_begin()))) +
         front_avail_();
}

template<size_t TSize, size_t TAlign, typename A>
template<typename T>
auto deque_storage_base<TSize, TAlign, A>::end() noexcept -> iterator<T> {
  using data_iterator = typename defn_::data_iterator;
  using defn_iterator = typename defn_::iterator;
  return iterator<T>(defn_iterator(data_iterator(data_ptr_end()))) -
         back_avail_();
}

template<size_t TSize, size_t TAlign, typename A>
template<typename T>
auto deque_storage_base<TSize, TAlign, A>::begin() const noexcept ->
    const_iterator<T> {
  using data_iterator = typename defn_::const_data_iterator;
  using defn_iterator = typename defn_::const_iterator;
  return const_iterator<T>(defn_iterator(data_iterator(data_ptr_begin()))) +
         front_avail_();
}

template<size_t TSize, size_t TAlign, typename A>
template<typename T>
auto deque_storage_base<TSize, TAlign, A>::end() const noexcept ->
    const_iterator<T> {
  using data_iterator = typename defn_::const_data_iterator;
  using defn_iterator = typename defn_::const_iterator;
  return const_iterator<T>(defn_iterator(data_iterator(data_ptr_end()))) -
         back_avail_();
}

template<size_t TSize, size_t TAlign, typename A>
template<typename T>
auto deque_storage_base<TSize, TAlign, A>::cbegin() const noexcept ->
    const_iterator<T> {
  return begin<T>();
}

template<size_t TSize, size_t TAlign, typename A>
template<typename T>
auto deque_storage_base<TSize, TAlign, A>::cend() const noexcept ->
    const_iterator<T> {
  return end<T>();
}

template<size_t TSize, size_t TAlign, typename A>
auto deque_storage_base<TSize, TAlign, A>::data_ptr_begin() noexcept ->
    typename defn_::data_ptr_iterator {
  using iterator = typename defn_::data_ptr_iterator;
  return iterator(data_.begin());
}

template<size_t TSize, size_t TAlign, typename A>
auto deque_storage_base<TSize, TAlign, A>::data_ptr_end() noexcept ->
    typename defn_::data_ptr_iterator {
  using iterator = typename defn_::data_ptr_iterator;
  return iterator(data_.end());
}

template<size_t TSize, size_t TAlign, typename A>
auto deque_storage_base<TSize, TAlign, A>::data_ptr_begin() const noexcept ->
    typename defn_::const_data_ptr_iterator {
  using iterator = typename defn_::const_data_ptr_iterator;
  return iterator(data_.begin());
}

template<size_t TSize, size_t TAlign, typename A>
auto deque_storage_base<TSize, TAlign, A>::data_ptr_end() const noexcept ->
    typename defn_::const_data_ptr_iterator {
  using iterator = typename defn_::const_data_ptr_iterator;
  return iterator(data_.end());
}

template<size_t TSize, size_t TAlign, typename A>
auto deque_storage_base<TSize, TAlign, A>::shrink_to_fit_before_() noexcept ->
    void {
  using block_iter = typename defn_::data_ptr_iterator;

  size_type blocks_over = front_avail_() / block_items_();
  const auto d_begin = data_.begin();
  const auto d_end = d_begin + blocks_over;

  for_each(block_iter(d_begin), block_iter(d_end),
           alloc_deleter_visitor(this->get_allocator_()));
  data_.erase(d_begin, d_end);
  off_ -= blocks_over * block_items_();
}

template<size_t TSize, size_t TAlign, typename A>
auto deque_storage_base<TSize, TAlign, A>::shrink_to_fit_after_() noexcept ->
    void {
  using block_iter = typename defn_::data_ptr_iterator;

  size_type blocks_over = back_avail_() / block_items_();
  const auto d_end = data_.end();
  const auto d_begin = d_end - blocks_over;

  for_each(block_iter(d_begin), block_iter(d_end),
           alloc_deleter_visitor(this->get_allocator_()));
  data_.erase(d_begin, d_end);
}


} /* namespace std::impl */


template<typename T, typename A>
deque<T, A>::deque(const allocator_type& a)
: data_(a)
{}

template<typename T, typename A>
deque<T, A>::deque(size_type n)
: deque()
{
  resize(n);
}

template<typename T, typename A>
deque<T, A>::deque(size_type n, const_reference v, const allocator_type& a)
: deque(a)
{
  resize(n, v);
}

template<typename T, typename A>
template<typename InputIter>
deque<T, A>::deque(InputIter b, InputIter e, const allocator_type& a)
: deque(a)
{
  assign(b, e);
}

template<typename T, typename A>
deque<T, A>::deque(const deque& o)
: deque(o.begin(), o.end(), o.data_.get_allocator_for_copy_())
{}

template<typename T, typename A>
deque<T, A>::deque(deque&& o)
    noexcept(is_nothrow_move_constructible<impl::alloc_base<A>>::value)
: data_(move(o.data_))
{}

template<typename T, typename A>
deque<T, A>::deque(const deque& o, const allocator_type& a)
: deque(a)
{
  *this = o;
}

template<typename T, typename A>
deque<T, A>::deque(deque&& o, const allocator_type& a)
: deque(a)
{
  if (this->data_.get_allocator_() == o.data_.get_allocator_())
    data_.swap(o.data_);
  else
    *this = move(o);
}

template<typename T, typename A>
deque<T, A>::deque(initializer_list<value_type> il, const allocator_type& a)
: deque(il.begin(), il.end(), a)
{}

template<typename T, typename A>
deque<T, A>::~deque() noexcept {
  clear();
}

template<typename T, typename A>
auto deque<T, A>::operator=(const deque& o) -> deque& {
  assign(o.begin(), o.end());
  return *this;
}

template<typename T, typename A>
auto deque<T, A>::operator=(deque&& o) -> deque& {
  assign(move_iterator<iterator>(o.begin()), move_iterator<iterator>(o.end()));
  o.clear();
  return *this;
}

template<typename T, typename A>
auto deque<T, A>::operator=(initializer_list<value_type> il) -> deque& {
  assign(il);
  return *this;
}

template<typename T, typename A>
template<typename Iter>
auto deque<T, A>::assign(Iter o_begin, Iter o_end) -> void {
  clear();
  data_.template insert<value_type>(begin(), o_begin, o_end);
}

template<typename T, typename A>
auto deque<T, A>::assign(size_type dist, const_reference v) -> void {
  clear();
  insert(begin(), dist, v);
}

template<typename T, typename A>
auto deque<T, A>::assign(initializer_list<value_type> il) -> void {
  assign(il.begin(), il.end());
}

template<typename T, typename A>
auto deque<T, A>::get_allocator() const -> allocator_type {
  return data_.get_allocator();
}

template<typename T, typename A>
auto deque<T, A>::begin() noexcept -> iterator {
  return data_.template begin<value_type>();
}

template<typename T, typename A>
auto deque<T, A>::begin() const noexcept -> const_iterator {
  return data_.template begin<value_type>();
}

template<typename T, typename A>
auto deque<T, A>::end() noexcept -> iterator {
  return data_.template end<value_type>();
}

template<typename T, typename A>
auto deque<T, A>::end() const noexcept -> const_iterator {
  return data_.template end<value_type>();
}

template<typename T, typename A>
auto deque<T, A>::rbegin() noexcept -> reverse_iterator {
  return reverse_iterator(end());
}

template<typename T, typename A>
auto deque<T, A>::rbegin() const noexcept -> const_reverse_iterator {
  return const_reverse_iterator(end());
}

template<typename T, typename A>
auto deque<T, A>::rend() noexcept -> reverse_iterator {
  return reverse_iterator(begin());
}

template<typename T, typename A>
auto deque<T, A>::rend() const noexcept -> const_reverse_iterator {
  return const_reverse_iterator(begin());
}

template<typename T, typename A>
auto deque<T, A>::cbegin() const noexcept -> const_iterator {
  return begin();
}

template<typename T, typename A>
auto deque<T, A>::cend() const noexcept -> const_iterator {
  return end();
}

template<typename T, typename A>
auto deque<T, A>::crbegin() const noexcept -> const_reverse_iterator {
  return rbegin();
}

template<typename T, typename A>
auto deque<T, A>::crend() const noexcept -> const_reverse_iterator {
  return rend();
}

template<typename T, typename A>
auto deque<T, A>::size() const noexcept -> size_type {
  return data_.size();
}

template<typename T, typename A>
auto deque<T, A>::max_size() const -> size_type {
  return data_.max_size();
}

template<typename T, typename A>
auto deque<T, A>::resize(size_type n) -> void {
  if (size() > n) {
    erase(begin() + n, end());
  } else {
    while (size() < n)
      emplace_back();
  }
}

template<typename T, typename A>
auto deque<T, A>::resize(size_type n, const_reference v) -> void {
  if (size() > n) {
    erase(next(begin(), n), end());
  } else {
    while (size() < n)
      push_back(v);
  }
}

template<typename T, typename A>
auto deque<T, A>::shrink_to_fit() -> void {
  data_.shrink_to_fit();
}

template<typename T, typename A>
auto deque<T, A>::capacity() const noexcept -> size_type {
  return data_.capacity();
}

template<typename T, typename A>
auto deque<T, A>::reserve(size_type rsv) -> void {
  return data_.reserve(rsv);
}

template<typename T, typename A>
auto deque<T, A>::empty() const noexcept -> bool {
  return data_.empty();
}

template<typename T, typename A>
auto deque<T, A>::operator[](size_type idx) noexcept -> reference {
  assert(idx < size());
  return begin()[idx];
}

template<typename T, typename A>
auto deque<T, A>::operator[](size_type idx) const noexcept -> const_reference {
  assert(idx < size());
  return begin()[idx];
}

template<typename T, typename A>
auto deque<T, A>::at(size_type idx) -> reference {
  if (idx >= size()) throw out_of_range("deque::at");
  return (*this)[idx];
}

template<typename T, typename A>
auto deque<T, A>::at(size_type idx) const -> const_reference {
  if (idx >= size()) throw out_of_range("deque::at");
  return (*this)[idx];
}

template<typename T, typename A>
auto deque<T, A>::front() noexcept -> reference {
  return data_.template front<value_type>();
}

template<typename T, typename A>
auto deque<T, A>::front() const noexcept -> const_reference {
  return data_.template front<value_type>();
}

template<typename T, typename A>
auto deque<T, A>::back() noexcept -> reference {
  return data_.template back<value_type>();
}

template<typename T, typename A>
auto deque<T, A>::back() const noexcept -> const_reference {
  return data_.template back<value_type>();
}

template<typename T, typename A>
template<typename... Args>
auto deque<T, A>::emplace_front(Args&&... args) -> void {
  data_.template emplace_front<value_type>(forward<Args>(args)...);
}

template<typename T, typename A>
template<typename... Args>
auto deque<T, A>::emplace_back(Args&&... args) -> void {
  data_.template emplace_back<value_type>(forward<Args>(args)...);
}

template<typename T, typename A>
template<typename... Args>
auto deque<T, A>::emplace(const_iterator pos, Args&&... args) -> iterator {
  return data_.emplace(pos, forward<Args>(args)...);
}

template<typename T, typename A>
auto deque<T, A>::push_front(const value_type& v) -> void {
  return emplace_front(v);
}

template<typename T, typename A>
auto deque<T, A>::push_front(value_type&& v) -> void {
  return emplace_front(move(v));
}

template<typename T, typename A>
auto deque<T, A>::push_back(const value_type& v) -> void {
  return emplace_back(v);
}

template<typename T, typename A>
auto deque<T, A>::push_back(value_type&& v) -> void {
  return emplace_back(move(v));
}

template<typename T, typename A>
auto deque<T, A>::insert(const_iterator pos, const value_type& v) -> iterator {
  return data_.template emplace<value_type>(pos, v);
}

template<typename T, typename A>
auto deque<T, A>::insert(const_iterator pos, value_type&& v) -> iterator {
  return data_.template emplace<value_type>(pos, move(v));
}

template<typename T, typename A>
auto deque<T, A>::insert(const_iterator pos, size_type n, const_reference v) ->
    iterator {
  return data_.template insert_n<value_type>(pos, n, v);
}

template<typename T, typename A>
template<typename InputIter>
auto deque<T, A>::insert(const_iterator pos, InputIter b, InputIter e) ->
    iterator {
  return data_.template insert<value_type>(pos, b, e);
}

template<typename T, typename A>
auto deque<T, A>::insert(const_iterator pos,
                         initializer_list<value_type> il) -> iterator {
  return insert(pos, il.begin(), il.end());
}

template<typename T, typename A>
auto deque<T, A>::pop_front() noexcept -> void {
  data_.template pop_front<value_type>();
}

template<typename T, typename A>
auto deque<T, A>::pop_back() noexcept -> void {
  data_.template pop_back<value_type>();
}

template<typename T, typename A>
auto deque<T, A>::erase(const_iterator i) noexcept -> iterator {
  return data_.template erase<value_type>(i);
}

template<typename T, typename A>
auto deque<T, A>::erase(const_iterator b, const_iterator e) noexcept ->
    iterator {
  return data_.template erase<value_type>(b, e);
}

template<typename T, typename A>
auto deque<T, A>::swap(deque& o) noexcept -> void {
  data_.swap(o.data_);
}

template<typename T, typename A>
auto deque<T, A>::clear() noexcept -> void {
  data_.template clear<value_type>();
}


template<typename T, typename A>
bool operator==(const deque<T, A>& a, const deque<T, A>& b) {
  return equal(a.begin(), a.end(), b.begin(), b.end());
}

template<typename T, typename A>
bool operator!=(const deque<T, A>& a, const deque<T, A>& b) {
  return !(a == b);
}

template<typename T, typename A>
bool operator<(const deque<T, A>& a, const deque<T, A>& b) {
  return lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
}

template<typename T, typename A>
bool operator>(const deque<T, A>& a, const deque<T, A>& b) {
  return b < a;
}

template<typename T, typename A>
bool operator<=(const deque<T, A>& a, const deque<T, A>& b) {
  return !(b < a);
}

template<typename T, typename A>
bool operator>=(const deque<T, A>& a, const deque<T, A>& b) {
  return !(a < b);
}

template<typename T, typename A>
void swap(deque<T, A>& a, deque<T, A>& b) {
  a.swap(b);
}


_namespace_end(std)

#endif /* _DEQUE_INL_H_ */
