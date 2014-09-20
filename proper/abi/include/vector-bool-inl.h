#ifndef _VECTOR_BOOL_INL_H_
#define _VECTOR_BOOL_INL_H_

#include <vector>
#include <algorithm>
#include <limits>

_namespace_begin(std)
namespace impl {


template<typename InputIter>
auto bitwise_copy(InputIter b, InputIter e,
                  _vector_bool_iterator<false> out) ->
    _vector_bool_iterator<false> {
  using elem = _vector_bool_iterator<false>::elem;
  constexpr unsigned int digits = numeric_limits<elem>::digits;

  /* Empty range. */
  if (b == e) return out;

  /*
   * First, handle case where out.shift_ may not be zero.
   */
  if (out.shift_ != 0) {
    elem mask = (elem(1) << out.shift_);
    elem keep_mask = ~elem(0);
    elem assign = 0;
    for (unsigned char i = out.shift_; i != digits; ++i, ++b, mask <<= 1) {
      /* GUARD: End of sequence. */
      if (_predict_false(b == e)) {
        *out.p_ = (*out.p_ & keep_mask) | assign;
        out.shift_ = i;
        return out;
      }

      bool v = *b;
      keep_mask &= ~mask;
      assign |= (v ? mask : elem(0));
    }
    *out.p_ = (*out.p_ & keep_mask) | assign;
    ++out.p_;
    out.shift_ = 0;
  }

  /*
   * Handle copying for out.shift_ == 0.
   */
  for (;;) {
    elem mask = 0x1;
    elem assign = 0;
    for (unsigned char i = 0; i != digits; ++i, ++b, mask <<= 1) {
      /* GUARD: End of sequence. */
      if (_predict_false(b == e)) {
        if (i != 0) {
          elem keep_mask = ~(mask - 1U);
          *out.p_ = (*out.p_ & keep_mask) | assign;
          out.shift_ = i;
        }
        return out;
      }

      bool v = *b;
      assign |= (v ? mask : elem(0));
    }
    *out.p_ = assign;
    ++out.p_;
  }

  /* UNREACHABLE */
}

template<typename InputIter>
auto bitwise_move(InputIter b, InputIter e,
                  _vector_bool_iterator<false> out) ->
    _vector_bool_iterator<false> {
  using elem = _vector_bool_iterator<false>::elem;
  constexpr unsigned int digits = numeric_limits<elem>::digits;

  /* Empty range. */
  if (b == e) return out;

  /*
   * First, handle case where out.shift_ may not be zero.
   */
  if (out.shift_ != 0) {
    elem mask = (elem(1) << out.shift_);
    elem keep_mask = ~elem(0);
    elem assign = 0;
    for (unsigned char i = out.shift_; i != digits; ++i, ++b, mask <<= 1) {
      /* GUARD: End of sequence. */
      if (_predict_false(b == e)) {
        *out.p_ = (*out.p_ & keep_mask) | assign;
        out.shift_ = i;
        return out;
      }

      bool v = move(*b);
      keep_mask &= ~mask;
      assign |= (v ? mask : elem(0));
    }
    *out.p_ = (*out.p_ & keep_mask) | assign;
    ++out.p_;
    out.shift_ = 0;
  }

  /*
   * Handle copying for out.shift_ == 0.
   */
  for (;;) {
    elem mask = 0x1;
    elem assign = 0;
    for (unsigned char i = 0; i != digits; ++i, ++b, mask <<= 1) {
      /* GUARD: End of sequence. */
      if (_predict_false(b == e)) {
        if (i != 0) {
          elem keep_mask = ~(mask - 1U);
          *out.p_ = (*out.p_ & keep_mask) | assign;
          out.shift_ = i;
        }
        return out;
      }

      bool v = move(*b);
      assign |= (v ? mask : elem(0));
    }
    *out.p_ = assign;
    ++out.p_;
  }

  /* UNREACHABLE */
}


template<bool IsConst>
constexpr _vector_bool_iterator<IsConst>::_vector_bool_iterator(elem_pointer p)
    noexcept
: p_(p)
{}

template<bool IsConst>
template<typename Rhs>
_vector_bool_iterator<IsConst>::_vector_bool_iterator(
    const Rhs& rhs,
    enable_if_t<(is_base_of<_vector_bool_iterator<false>, Rhs>::value &&
                 IsConst),
                int>) noexcept {
  _vector_bool_iterator<false> rhs_impl = rhs;
  p_ = rhs_impl.p_;
  shift_ = rhs_impl.shift_;
}

template<bool IsConst>
constexpr _vector_bool_iterator<IsConst>::_vector_bool_iterator(
    elem_pointer p, unsigned char shift) noexcept
: p_(p),
  shift_(shift)
{}

template<bool IsConst>
auto _vector_bool_iterator<IsConst>::operator*() const noexcept -> reference {
  return _vector_bool_iterator_deref<reference>(p_, elem(1) << shift_).result;
}

template<bool IsConst>
auto _vector_bool_iterator<IsConst>::operator++() noexcept ->
    _vector_bool_iterator& {
  if (shift_ == numeric_limits<elem>::digits - 1) {
    shift_ = 0;
    ++p_;
  } else {
    ++shift_;
  }
  return *this;
}

template<bool IsConst>
auto _vector_bool_iterator<IsConst>::operator++(int) noexcept ->
    _vector_bool_iterator {
  _vector_bool_iterator copy = *this;
  ++*this;
  return copy;
}

template<bool IsConst>
auto _vector_bool_iterator<IsConst>::operator--() noexcept ->
    _vector_bool_iterator& {
  if (shift_ == 0) {
    shift_ = numeric_limits<elem>::digits - 1;
    --p_;
  } else {
    --shift_;
  }
  return *this;
}

template<bool IsConst>
auto _vector_bool_iterator<IsConst>::operator--(int) noexcept ->
    _vector_bool_iterator {
  _vector_bool_iterator copy = *this;
  --*this;
  return copy;
}

template<bool IsConst>
auto _vector_bool_iterator<IsConst>::operator+=(difference_type delta)
    noexcept -> _vector_bool_iterator& {
  return *this = *this + delta;
}

template<bool IsConst>
auto _vector_bool_iterator<IsConst>::operator-=(difference_type delta)
    noexcept -> _vector_bool_iterator& {
  return *this = *this - delta;
}

template<bool IsConst>
constexpr auto _vector_bool_iterator<IsConst>::operator+(difference_type delta)
    const noexcept -> _vector_bool_iterator {
  if (_predict_true(delta >= 0)) {
    /*
     * We need the unsigned type,
     * because overflow is undefined on signed types.
     */
    const make_unsigned_t<difference_type> d = delta;
    return _vector_bool_iterator(p_ + d / numeric_limits<elem>::digits,
                                 (shift_ + d) % numeric_limits<elem>::digits);
  } else {
    /*
     * Convert to unsigned type;
     * be careful not to trigger -INT_MIN = INT_MIN.
     *
     * We need the unsigned type,
     * because overflow is undefined on signed types.
     */
    make_unsigned_t<difference_type> d = -(delta - 1);
    d += 1;
    return _vector_bool_iterator(p_ - d / numeric_limits<elem>::digits,
                                 (shift_ - d) % numeric_limits<elem>::digits);
  }
}

template<bool IsConst>
constexpr auto _vector_bool_iterator<IsConst>::operator-(difference_type delta)
    const noexcept -> _vector_bool_iterator {
  if (_predict_true(delta >= 0)) {
    /*
     * We need the unsigned type,
     * because overflow is undefined on signed types.
     */
    const make_unsigned_t<difference_type> d = delta;
    return _vector_bool_iterator(p_ - d / numeric_limits<elem>::digits,
                                 (shift_ - d) % numeric_limits<elem>::digits);
  } else {
    /*
     * Convert to unsigned type;
     * be careful not to trigger -INT_MIN = INT_MIN.
     *
     * We need the unsigned type,
     * because overflow is undefined on signed types.
     */
    make_unsigned_t<difference_type> d = -(delta - 1);
    d += 1;
    return _vector_bool_iterator(p_ + d / numeric_limits<elem>::digits,
                                 (shift_ + d) % numeric_limits<elem>::digits);
  }
}

template<bool IsConst>
constexpr auto _vector_bool_iterator<IsConst>::operator-(
    const _vector_bool_iterator& rhs) const noexcept -> difference_type {
  const auto p_delta = p_ - rhs.p_;
  const difference_type lhs_shift = shift_;
  const difference_type rhs_shift = rhs.shift_;
  const auto s_delta = lhs_shift - rhs_shift;

  return p_delta * numeric_limits<elem>::digits + s_delta;
}

template<bool IsConst>
auto _vector_bool_iterator<IsConst>::operator[](difference_type n)
    const noexcept -> reference {
  return *(*this + n);
}

inline auto operator==(const _vector_bool_iterator<true>& x,
                       const _vector_bool_iterator<true>& y)
    noexcept -> bool {
  return x.p_ == y.p_ && x.shift_ == y.shift_;
}

inline auto operator!=(const _vector_bool_iterator<true>& x,
                       const _vector_bool_iterator<true>& y)
    noexcept -> bool {
  return !(x == y);
}

inline auto operator<(const _vector_bool_iterator<true>& x,
                      const _vector_bool_iterator<true>& y)
    noexcept -> bool {
  return (x.p_ != y.p_ ?
          x.p_ < y.p_ :
          x.shift_ < y.shift_);
}

inline auto operator>(const _vector_bool_iterator<true>& x,
                      const _vector_bool_iterator<true>& y)
    noexcept -> bool {
  return y < x;
}

inline auto operator<=(const _vector_bool_iterator<true>& x,
                       const _vector_bool_iterator<true>& y)
    noexcept -> bool {
  return !(y < x);
}

inline auto operator>=(const _vector_bool_iterator<true>& x,
                       const _vector_bool_iterator<true>& y)
    noexcept -> bool {
  return !(x < y);
}


constexpr _vector_bool_reference::_vector_bool_reference(elem* p, elem msk)
    noexcept
: p_(p),
  msk_(msk)
{}

inline _vector_bool_reference::operator bool() const noexcept {
  return (*p_ & msk_);
}

inline auto _vector_bool_reference::operator=(const bool& v)
    const noexcept -> const _vector_bool_reference& {
  if (v)
    *p_ |= msk_;
  else
    *p_ &= ~msk_;
  return *this;
}

inline auto _vector_bool_reference::operator=(const _vector_bool_reference& v)
    const noexcept -> const _vector_bool_reference& {
  if (v)
    *p_ |= msk_;
  else
    *p_ &= ~msk_;
  return *this;
}

inline auto _vector_bool_reference::flip() const noexcept -> void {
  *p_ ^= msk_;
}

inline auto exchange(const _vector_bool_reference& r, const bool& v)
    noexcept -> bool {
  bool rv = r;
  r = v;
  return rv;
}

inline auto swap(const _vector_bool_reference& a,
                 const _vector_bool_reference& b) noexcept -> void {
  bool tmp = a;
  a = b;
  b = tmp;
}

inline auto swap(const _vector_bool_reference& a,
                 bool& b) noexcept -> void {
  bool tmp = a;
  a = b;
  b = tmp;
}

inline auto swap(bool& a,
                 const _vector_bool_reference& b) noexcept -> void {
  bool tmp = a;
  a = b;
  b = tmp;
}


} /* namespace std::impl */


template<typename InputIter>
auto copy(InputIter b, InputIter e,
          impl::_vector_bool_iterator<false> out) ->
    impl::_vector_bool_iterator<false> {
  return impl::bitwise_copy(b, e, out);
}

template<typename InputIter>
auto move(InputIter b, InputIter e,
          impl::_vector_bool_iterator<false> out) ->
    impl::_vector_bool_iterator<false> {
  return impl::bitwise_move(b, e, out);
}

template<bool IsConst>
auto count(impl::_vector_bool_iterator<IsConst> b,
           impl::_vector_bool_iterator<IsConst> e,
           const bool& v)
    noexcept -> impl::_vector_bool_iterator<true>::difference_type {
  return impl::_vector_bool_iterator_algorithms::bitwise_count(b, e, v);
}

inline auto copy(impl::_vector_bool_iterator<false> b,
                 impl::_vector_bool_iterator<false> e,
                 impl::_vector_bool_iterator<false> out) noexcept ->
    impl::_vector_bool_iterator<false> {
  using iter = impl::_vector_bool_iterator<true>;
  return copy(iter(b), iter(e), out);
}

inline auto move(impl::_vector_bool_iterator<false> b,
                 impl::_vector_bool_iterator<false> e,
                 impl::_vector_bool_iterator<false> out) noexcept ->
    impl::_vector_bool_iterator<false> {
  using iter = impl::_vector_bool_iterator<true>;
  return move(iter(b), iter(e), out);
}

inline auto move(impl::_vector_bool_iterator<true> b,
                 impl::_vector_bool_iterator<true> e,
                 impl::_vector_bool_iterator<false> out) noexcept ->
    impl::_vector_bool_iterator<false> {
  return copy(b, e, out);
}


template<typename Alloc>
vector<bool, Alloc>::vector(const allocator_type& a)
: alloc_base(a)
{}

template<typename Alloc>
vector<bool, Alloc>::vector(size_type n, const bool& v,
                            const allocator_type& a)
: vector(a)
{
  assign(n, v);
}

template<typename Alloc>
template<typename InputIter>
vector<bool, Alloc>::vector(InputIter b, InputIter e,
                            const allocator_type& a)
: vector(a)
{
  assign(b, e);
}

template<typename Alloc>
vector<bool, Alloc>::vector(const vector& v)
: vector(v.get_allocator())
{
  reserve(v.size());
  copy(v.heap_, v.heap_ + v.heap_size_(), heap_);  // Copy underlying data.
  size_ = v.size_;
}

template<typename Alloc>
vector<bool, Alloc>::vector(vector&& v)
: alloc_base(move(v))
{
  using _namespace(std)::swap;

  swap(heap_, v.heap_);
  swap(size_, v.size_);
  swap(avail_, v.avail_);
}

template<typename Alloc>
vector<bool, Alloc>::vector(const vector& v, const allocator_type& a)
: vector(a)
{
  reserve(v.size());
  copy(v.heap_, v.heap_ + v.heap_size_(), heap_);  // Copy underlying data.
  size_ = v.size_;
}

template<typename Alloc>
vector<bool, Alloc>::vector(vector&& v, const allocator_type& a)
: vector(a)
{
  if (this->get_allocator_() == v.get_allocator_()) {
    using _namespace(std)::swap;

    swap(heap_, v.heap_);
    swap(size_, v.size_);
    swap(avail_, v.avail_);
  } else {
    reserve(v.size());
    copy(v.heap_, v.heap_ + v.heap_size_(), heap_);  // Copy underlying data.
    size_ = v.size_;
    v.clear();
  }
}

template<typename Alloc>
vector<bool, Alloc>::vector(initializer_list<value_type> il,
                            const allocator_type& a)
: vector(a)
{
  assign(il);
}

template<typename Alloc>
vector<bool, Alloc>::~vector() noexcept {
  clear();
  if (heap_avail_() > 0) this->deallocate_(heap_, heap_avail_());
}

template<typename Alloc>
auto vector<bool, Alloc>::operator=(const vector& v) -> vector& {
  reserve(v.size());
  copy(v.heap_, v.heap_ + v.heap_size_(), heap_);  // Copy underlying data.
  size_ = v.size_;
  return *this;
}

template<typename Alloc>
auto vector<bool, Alloc>::operator=(vector&& v) -> vector& {
  if (this->get_allocator_() == v.get_allocator_()) {
    swap(v);
    v.clear();
  } else {
    reserve(v.size());
    copy(v.heap_, v.heap_ + v.heap_size_(), heap_);  // Copy underlying data.
    size_ = v.size_;
  }
  return *this;
}

template<typename Alloc>
auto vector<bool, Alloc>::operator=(initializer_list<value_type> il) ->
    vector& {
  assign(il);
  return *this;
}

template<typename Alloc>
template<typename InputIter>
auto vector<bool, Alloc>::assign(InputIter b, InputIter e) -> void {
  const auto dist = impl::equal_support__distance(
      b, e, typename iterator_traits<InputIter>::iterator_category());
  if (dist.first) reserve(dist.second);
  copy(b, e, back_inserter(*this));
}

template<typename Alloc>
auto vector<bool, Alloc>::assign(size_type n, const bool& v) -> void {
  reserve(n);
  const elem underlying_v = (v ? ~elem(0) : elem(0));
  constexpr unsigned int digits = numeric_limits<elem>::digits;
  fill_n(heap_, (n + digits - 1) / digits, underlying_v);
  size_ = n;
}

template<typename Alloc>
auto vector<bool, Alloc>::assign(initializer_list<value_type> il) -> void {
  reserve(il.size());
  size_ = il.size();

  /* Initialize by construction underlying data instead. */
  auto il_i = il.begin();
  elem* p = heap_;
  while (il_i != il.end()) {
    elem v = 0;
    elem msk = 1;
    for (int i = 0;
         i < numeric_limits<elem>::digits && il_i != il.end();
         ++i, ++il_i, msk <<= 1U) {
      if (*il_i)
        v |= msk;
    }
    *p++ = v;
  }
}

template<typename Alloc>
auto vector<bool, Alloc>::get_allocator() const -> allocator_type {
  return this->alloc_base::get_allocator();
}

template<typename Alloc>
auto vector<bool, Alloc>::begin() noexcept -> iterator {
  return iterator(heap_);
}

template<typename Alloc>
auto vector<bool, Alloc>::end() noexcept -> iterator {
  return begin() + size();
}

template<typename Alloc>
auto vector<bool, Alloc>::begin() const noexcept -> const_iterator {
  return const_iterator(heap_);
}

template<typename Alloc>
auto vector<bool, Alloc>::end() const noexcept -> const_iterator {
  return begin() + size();
}

template<typename Alloc>
auto vector<bool, Alloc>::cbegin() const noexcept -> const_iterator {
  return begin();
}

template<typename Alloc>
auto vector<bool, Alloc>::cend() const noexcept -> const_iterator {
  return end();
}

template<typename Alloc>
auto vector<bool, Alloc>::rbegin() noexcept -> reverse_iterator {
  return reverse_iterator(end());
}

template<typename Alloc>
auto vector<bool, Alloc>::rend() noexcept -> reverse_iterator {
  return reverse_iterator(begin());
}

template<typename Alloc>
auto vector<bool, Alloc>::rbegin() const noexcept -> const_reverse_iterator {
  return const_reverse_iterator(end());
}

template<typename Alloc>
auto vector<bool, Alloc>::rend() const noexcept -> const_reverse_iterator {
  return const_reverse_iterator(begin());
}

template<typename Alloc>
auto vector<bool, Alloc>::crbegin() const noexcept -> const_reverse_iterator {
  return rbegin();
}

template<typename Alloc>
auto vector<bool, Alloc>::crend() const noexcept -> const_reverse_iterator {
  return rend();
}

template<typename Alloc>
auto vector<bool, Alloc>::size() const noexcept -> size_type {
  return size_;
}

template<typename Alloc>
auto vector<bool, Alloc>::max_size() const noexcept -> size_type {
  using alloc_traits = allocator_traits<typename alloc_base::allocator_type>;

  const auto alloc_max_sz = alloc_traits::max_size(this->get_allocator_());
  constexpr size_type type_max_sz = numeric_limits<size_type>::max();
  return (alloc_max_sz < type_max_sz ? alloc_max_sz : type_max_sz);
}

template<typename Alloc>
auto vector<bool, Alloc>::resize(size_type desired, value_type v) -> void {
  if (size_ > desired) {
    size_ = desired;
  } else if (size_ < desired) {
    reserve(desired);
    const elem set = (v ? ~elem(0) : elem(0));
    constexpr unsigned int digits = numeric_limits<elem>::digits;

    const auto s_mod = size_ % digits;
    auto p = heap_ + size_ / digits;
    if (s_mod != 0) {
      const elem s_msk = elem(1) << s_mod - 1;
      *p = (*p & s_msk) | (set & ~s_msk);  // Set formerly out of range bits.
      ++p;
    }

    const auto heap_end = heap_ + heap_size_();
    fill(p, heap_end, set);
    size_ = desired;
  }
}

template<typename Alloc>
auto vector<bool, Alloc>::capacity() const noexcept -> size_type {
  return avail_;
}

template<typename Alloc>
auto vector<bool, Alloc>::empty() const noexcept -> bool {
  return size() == 0;
}

template<typename Alloc>
auto vector<bool, Alloc>::reserve(size_type rsv) -> void {
  using alloc_traits = allocator_traits<typename alloc_base::allocator_type>;
  constexpr unsigned int digits = numeric_limits<elem>::digits;

  if (avail_ < rsv) {
    rsv = ((rsv + digits - 1) / digits) * digits;  // Round up.
    size_type new_sz = max(rsv, 2 * capacity());

    if (alloc_traits::resize(this->get_allocator_(), heap_,
                             avail_ / digits, new_sz / digits)) {
      avail_ = new_sz;
      return;
    } else if (alloc_traits::resize(this->get_allocator_(), heap_,
                                    avail_ / digits, rsv / digits)) {
      avail_ = rsv;
      return;
    }

    vector temp(this->get_allocator());
    try {
      temp.heap_ = this->allocate_(new_sz / digits);
      temp.avail_ = new_sz;
    } catch (const bad_alloc&) {
      if (new_sz == rsv) throw;
      new_sz = rsv;
      temp.heap_ = this->allocate_(new_sz / digits);
      temp.avail_ = new_sz;
    }
    copy(heap_, heap_ + heap_size_(), temp.heap_);
    temp.size_ = size();
    swap(temp);
  }
}

template<typename Alloc>
auto vector<bool, Alloc>::shrink_to_fit() -> void {
  using alloc_traits = allocator_traits<typename alloc_base::allocator_type>;
  constexpr unsigned int digits = numeric_limits<elem>::digits;
  const size_type rounded_sz = heap_size_() * digits;

  if (avail_ > rounded_sz) {
    if (alloc_traits::resize(this->get_allocator_(),
                             heap_, avail_ / digits, rounded_sz / digits)) {
      avail_ = rounded_sz;
      return;
    }

    vector temp(this->get_allocator_());
    temp.heap_ = this->allocate_(rounded_sz);
    temp.avail_ = rounded_sz;
    copy(heap_, heap_ + heap_size_(), temp.heap_);
    temp.size_ = size();
    swap(temp);
  }
}

template<typename Alloc>
auto vector<bool, Alloc>::operator[](size_type idx) noexcept -> reference {
  assert(idx < size());
  return begin()[idx];
}

template<typename Alloc>
auto vector<bool, Alloc>::operator[](size_type idx) const noexcept ->
    const_reference {
  assert(idx < size());
  return begin()[idx];
}

template<typename Alloc>
auto vector<bool, Alloc>::at(size_type idx) -> reference {
  if (idx >= size()) throw out_of_range("vector::at");
  return (*this)[idx];
}

template<typename Alloc>
auto vector<bool, Alloc>::at(size_type idx) const -> const_reference {
  if (idx >= size()) throw out_of_range("vector::at");
  return (*this)[idx];
}

template<typename Alloc>
auto vector<bool, Alloc>::front() noexcept -> reference {
  return *begin();
}

template<typename Alloc>
auto vector<bool, Alloc>::front() const noexcept -> const_reference {
  return *begin();
}
template<typename Alloc>
auto vector<bool, Alloc>::back() noexcept -> reference {
  return *rbegin();
}

template<typename Alloc>
auto vector<bool, Alloc>::back() const noexcept -> const_reference {
  return *rbegin();
}

template<typename Alloc>
auto vector<bool, Alloc>::push_back(const value_type& v) -> void {
  reserve(size() + 1U);
  *end() = v;
  ++size_;
}

template<typename Alloc>
auto vector<bool, Alloc>::pop_back() noexcept -> void {
  assert(size_ > 0);
  if (size_ > 0) --size_;
}

template<typename Alloc>
auto vector<bool, Alloc>::insert(const_iterator pos_, const value_type& v) ->
    iterator {
  assert(begin() <= pos_ && pos_ <= end());

  const auto dist = pos_ - begin();
  reserve(size() + 1U);
  resize(size() + 1U);
  iterator pos = begin() + dist;
  move_backward(pos, prev(end()), end());
  *pos = v;
  return pos;
}

template<typename Alloc>
auto vector<bool, Alloc>::insert(const_iterator pos_,
                                 size_type n, const value_type& v) ->
    iterator {
  assert(begin() <= pos_ && pos_ <= end());

  const auto dist = pos_ - begin();
  reserve(size() + 1U);
  resize(size() + 1U);
  iterator pos = begin() + dist;
  move_backward(pos, end() - n, end());
  fill_n(pos, n, v);
  return pos;
}

template<typename Alloc>
template<typename InputIter>
auto vector<bool, Alloc>::insert(const_iterator pos,
                                 InputIter b, InputIter e) -> iterator {
  assert(begin() <= pos && pos <= end());

  const size_type off = pos - begin();
  const auto dist = impl::equal_support__distance(
      b, e, typename iterator_traits<InputIter>::iterator_category());
  if (!dist.first) {
    copy(b, e, inserter(*this, begin() + off));
    return begin() + off;
  }

  resize(size() + dist.second);
  move_backward(begin() + off, end() - dist.second(), end());
  copy(b, e, begin() + off);
  return begin() + off;
}

template<typename Alloc>
auto vector<bool, Alloc>::insert(const_iterator pos_,
                                 initializer_list<value_type> il) -> iterator {
  assert(begin() <= pos_ && pos_ <= end());

  const size_type off = pos_ - begin();
  resize(size() + il.size());
  move_backward(begin() + off, end() - il.size(), end());
  copy(il.begin(), il.end(), begin() + off);
  return begin() + off;
}

template<typename Alloc>
auto vector<bool, Alloc>::erase(const_iterator pos_) -> iterator {
  assert(begin() <= pos_ && pos_ < end());

  iterator pos = begin() + (pos_ - begin());
  move(pos + 1, end(), pos);
  pop_back();
  return pos;
}

template<typename Alloc>
auto vector<bool, Alloc>::erase(const_iterator b_, const_iterator e_) ->
    iterator {
  assert(begin() <= b_ && b_ <= e_ && e_ <= end());

  auto b = begin() + (b_ - begin());
  auto e = begin() + (e_ - begin());

  move(e, end(), b);
  resize(size() - (e - b));
  return b;
}

template<typename Alloc>
auto vector<bool, Alloc>::swap(vector& o)
    noexcept(noexcept(declval<vector>().swap_(o))) -> void {
  using _namespace(std)::swap;

  this->alloc_base::swap_(o);
  swap(heap_, o.heap_);
  swap(size_, o.size_);
  swap(avail_, o.avail_);
}

template<typename Alloc>
auto vector<bool, Alloc>::clear() noexcept -> void {
  resize(0);
}

template<typename Alloc>
auto vector<bool, Alloc>::heap_size_() const noexcept -> size_type {
  constexpr unsigned int digits = numeric_limits<elem>::digits;

  return (size() + digits - 1) / digits;
}

template<typename Alloc>
auto vector<bool, Alloc>::heap_avail_() const noexcept -> size_type {
  constexpr unsigned int digits = numeric_limits<elem>::digits;

  assert(capacity() % digits == 0);
  return capacity() / digits;
}

template<typename Alloc>
auto operator==(const vector<bool, Alloc>& a, const vector<bool, Alloc>& b)
    noexcept -> bool {
  using elem = typename vector<bool, Alloc>::elem;
  constexpr unsigned int digits = numeric_limits<elem>::digits;

  if (a.size() != b.size()) return false;
  const auto sz = a.size();

  /* Perform comparison on underlying data type. */
  const elem* p_a = a.heap_;
  const elem* p_b = b.heap_;
  const elem* p_a_end = a.heap_ + sz / digits;
  const elem* p_b_end = b.heap_ + sz / digits;
  if (!equal(p_a, p_a_end, p_b, p_b_end)) return false;

  /* Compare partial integer at end of array, using proper mask. */
  if (sz % digits) {
    const elem msk = (elem(1) << (sz % digits)) - 1U;
    if ((*p_a_end & msk) != (*p_b_end & msk)) return false;
  }

  return true;
}

template<typename Alloc>
auto operator<(const vector<bool, Alloc>& a, const vector<bool, Alloc>& b)
    noexcept -> bool {
  using elem = typename vector<bool, Alloc>::elem;
  constexpr unsigned int digits = numeric_limits<elem>::digits;

  const auto sz = min(a.size(), b.size());

  /* Find first position where underlying integers don't match up. */
  const elem* p_a = a.heap_;
  const elem* p_b = b.heap_;
  const elem* p_a_end = a.heap_ + sz / digits;
  const elem* p_b_end = b.heap_ + sz / digits;
  const elem* mismatch_a;
  const elem* mismatch_b;
  tie(mismatch_a, mismatch_b) = mismatch(p_a, p_a_end, p_b, p_b_end);

  /* Compare partial integer at end of array, using iterators. */
  return lexicographical_compare(impl::_vector_bool_iterator<true>(mismatch_a),
                                 a.end(),
                                 impl::_vector_bool_iterator<true>(mismatch_b),
                                 b.end());
}


_namespace_end(std)

#endif /* _VECTOR_BOOL_INL_H_ */
