#ifndef _FORWARD_LIST_INL_H_
#define _FORWARD_LIST_INL_H_

#include <forward_list>
#include <functional>
#include <stdimpl/alloc_deleter.h>

_namespace_begin(std)
namespace impl {

template<typename T, class Tag>
template<typename... Args>
forward_list_elem<T, Tag>::forward_list_elem(Args&&... args)
: value(forward<Args>(args)...)
{}

} /* namespace std::impl */


template<typename T, typename A>
forward_list<T, A>::forward_list(const allocator_type& a)
: alloc_base(a)
{}

template<typename T, typename A>
forward_list<T, A>::forward_list(size_type n)
: forward_list()
{
  while (n-- > 0) emplace_front();
}

template<typename T, typename A>
forward_list<T, A>::forward_list(size_type n, const_reference v,
                                 const allocator_type& a)
: forward_list(a)
{
  assign(n, v);
}

template<typename T, typename A>
template<typename InputIter>
forward_list<T, A>::forward_list(InputIter b, InputIter e,
                                 const allocator_type& a)
: forward_list(a)
{
  assign(b, e);
}

template<typename T, typename A>
forward_list<T, A>::forward_list(const forward_list& other)
: forward_list(other.begin(), other.end(), other.get_allocator_for_copy_())
{}

template<typename T, typename A>
forward_list<T, A>::forward_list(forward_list&& other)
: alloc_base(other)
{
  data_.swap(other.data_);
}

template<typename T, typename A>
forward_list<T, A>::forward_list(const forward_list& other,
                                 const allocator_type& a)
: forward_list(a)
{
  assign(other.begin(), other.end());
}

template<typename T, typename A>
forward_list<T, A>::forward_list(forward_list&& other,
                                 const allocator_type& a)
: forward_list(a)
{
  if (this->get_allocator_() == other.get_allocator_()) {
    data_.swap(other.data_);
  } else {
    assign(move_iterator<iterator>(other.begin()),
           move_iterator<iterator>(other.end()));
  }
}

template<typename T, typename A>
forward_list<T, A>::forward_list(initializer_list<value_type> il,
                                 const allocator_type& a)
: forward_list(a)
{
  assign(il);
}

template<typename T, typename A>
forward_list<T, A>::~forward_list() noexcept {
  clear();
}

template<typename T, typename A>
auto forward_list<T, A>::operator=(const forward_list& other) ->
    forward_list& {
  if (this != &other) assign(other.begin(), other.end());
  return *this;
}

template<typename T, typename A>
auto forward_list<T, A>::operator=(forward_list&& other) ->
    forward_list& {
  assert(this != &other);

  if (this->get_allocator_() == other.get_allocator_()) {
    clear();
    data_.swap(other.data_);
  } else {
    assign(move_iterator<iterator>(other.begin()),
           move_iterator<iterator>(other.end()));
    other.clear();
  }
  return *this;
}

template<typename T, typename A>
auto forward_list<T, A>::operator=(initializer_list<value_type> il) ->
    forward_list& {
  assign(il);
  return *this;
}

template<typename T, typename A>
template<typename InputIter>
auto forward_list<T, A>::assign(InputIter b, InputIter e) -> void {
  clear();
  for (iterator i = before_begin(); b != e; ++b)
    i = insert_after(i, *b);
}

template<typename T, typename A>
auto forward_list<T, A>::assign(size_type n, const_reference v) -> void {
  clear();
  while (n-- > 0) push_front(v);
}

template<typename T, typename A>
auto forward_list<T, A>::assign(initializer_list<value_type> il) -> void {
  assign(il.begin(), il.end());
}

template<typename T, typename A>
auto forward_list<T, A>::before_begin() noexcept -> iterator {
  return iterator(data_.before_begin());
}

template<typename T, typename A>
auto forward_list<T, A>::before_begin() const noexcept -> const_iterator {
  return const_iterator(data_.before_begin());
}

template<typename T, typename A>
auto forward_list<T, A>::begin() noexcept -> iterator {
  return iterator(data_.begin());
}

template<typename T, typename A>
auto forward_list<T, A>::begin() const noexcept -> const_iterator {
  return const_iterator(data_.begin());
}

template<typename T, typename A>
auto forward_list<T, A>::end() noexcept -> iterator {
  return iterator(data_.end());
}

template<typename T, typename A>
auto forward_list<T, A>::end() const noexcept -> const_iterator {
  return const_iterator(data_.end());
}

template<typename T, typename A>
auto forward_list<T, A>::cbefore_begin() const noexcept -> const_iterator {
  return before_begin();
}

template<typename T, typename A>
auto forward_list<T, A>::cbegin() const noexcept -> const_iterator {
  return begin();
}

template<typename T, typename A>
auto forward_list<T, A>::cend() const noexcept -> const_iterator {
  return end();
}

template<typename T, typename A>
auto forward_list<T, A>::empty() const noexcept -> bool {
  return data_.empty();
}

template<typename T, typename A>
auto forward_list<T, A>::max_size() const -> size_type {
  using alloc_traits = allocator_traits<typename alloc_base::allocator_type>;
  return alloc_traits::max_size(this->get_allocator_());
}

template<typename T, typename A>
auto forward_list<T, A>::front() noexcept -> reference {
  return *begin();
}

template<typename T, typename A>
auto forward_list<T, A>::front() const noexcept -> const_reference {
  return *begin();
}

template<typename T, typename A>
template<typename... Args>
auto forward_list<T, A>::emplace_front(Args&&... args) -> void {
  void* hint = (empty() ?
                static_cast<void*>(this) :
                static_cast<void*>(&front()));
  auto ptr = impl::new_alloc_deleter<elem>(this->get_allocator_(),
                                           hint,
                                           forward<Args>(args)...);
  data_.link_front(ptr.release());
}

template<typename T, typename A>
auto forward_list<T, A>::push_front(const_reference v) -> void {
  emplace_front(v);
}

template<typename T, typename A>
auto forward_list<T, A>::push_front(value_type&& v) -> void {
  emplace_front(move(v));
}

template<typename T, typename A>
auto forward_list<T, A>::pop_front() noexcept -> void {
  impl::existing_alloc_deleter(this->get_allocator_(),
                               data_.unlink_front());
}

template<typename T, typename A>
template<typename... Args>
auto forward_list<T, A>::emplace_after(const_iterator i, Args&&... args) ->
    iterator {
  assert(i != end());
  void* hint = const_cast<void*>(static_cast<const void*>(&*i));
  auto ptr = impl::new_alloc_deleter<elem>(this->get_allocator_(), hint,
                                           forward<Args>(args)...);
  return iterator(data_.link_after(i.impl_, ptr.release()));
}

template<typename T, typename A>
auto forward_list<T, A>::insert_after(const_iterator i, const_reference v) ->
    iterator {
  return emplace_after(i, v);
}

template<typename T, typename A>
auto forward_list<T, A>::insert_after(const_iterator i, value_type&& v) ->
    iterator {
  return emplace_after(i, move(v));
}

template<typename T, typename A>
auto forward_list<T, A>::insert_after(const_iterator i_,
                                      size_type n, const_reference v) ->
    iterator {
  auto i = iterator(data_.nonconst_iterator(i_.impl_));
  while (n-- > 0)
    i = insert_after(i, v);
  return i;
}

template<typename T, typename A>
template<typename InputIter>
auto forward_list<T, A>::insert_after(const_iterator i_,
                                      InputIter b, InputIter e) ->
    iterator {
  iterator i = iterator(data_.nonconst_iterator(i_.impl_));
  while (b != e) {
    i = insert_after(i, *b);
    ++b;
  }
  return i;
}

template<typename T, typename A>
auto forward_list<T, A>::insert_after(const_iterator i,
                                      initializer_list<value_type> il) ->
    iterator {
  return insert_after(i, il.begin(), il.end());
}

template<typename T, typename A>
auto forward_list<T, A>::erase_after(const_iterator pred) -> iterator {
  impl::existing_alloc_deleter(this->get_allocator_(),
                               data_.unlink_after(pred.impl_));
  return next(iterator(data_.nonconst_iterator(pred.impl_)));
}

template<typename T, typename A>
auto forward_list<T, A>::erase_after(const_iterator pred, iterator e) ->
    iterator {
  while (next(pred) != e)
    erase_after(pred);
  return e;
}

template<typename T, typename A>
auto forward_list<T, A>::swap(forward_list& other) -> void {
  this->alloc_base::swap_(other);
  data_.swap(other.data_);
}

template<typename T, typename A>
auto forward_list<T, A>::resize(size_type n) -> void {
  iterator pred = before_begin();
  while (next(pred) != end() && n > 0) {
    ++pred;
    --n;
  }

  if (n > 0) {
    while (n-- > 0)
      pred = emplace_after(pred);
  } else {
    erase_after(pred, end());
  }
}

template<typename T, typename A>
auto forward_list<T, A>::resize(size_type n, const_reference v) -> void {
  iterator pred = before_begin();
  while (next(pred) != end() && n > 0) {
    ++pred;
    --n;
  }

  if (n > 0)
    insert_after(pred, n, v);
  else
    erase_after(pred, end());
}

template<typename T, typename A>
auto forward_list<T, A>::clear() noexcept -> void {
  while (!empty()) pop_front();
}

template<typename T, typename A>
auto forward_list<T, A>::splice_after(const_iterator i, forward_list&& other)
    noexcept -> void {
  data_.splice_after(i.impl_, move(other.data_));
}

template<typename T, typename A>
auto forward_list<T, A>::splice_after(const_iterator i, forward_list&& other,
                                      const_iterator o_i)
    noexcept -> void {
  assert(o_i != other.end() && next(o_i) != other.end());
  splice_after(i, move(other), o_i, next(o_i, 2));
}

template<typename T, typename A>
auto forward_list<T, A>::splice_after(
    const_iterator i, forward_list&&,
    const_iterator o_b, const_iterator o_e) -> void {
  assert(o_b != o_e);
  data_.splice_after(i.impl_, o_b.impl_, o_e.impl_);
}

template<typename T, typename A>
auto forward_list<T, A>::remove(const_reference v) -> void {
  using placeholders::_1;

  remove_if(bind(equal_to<value_type>(), _1, cref(v)));
}

template<typename T, typename A>
template<typename Predicate>
auto forward_list<T, A>::remove_if(Predicate predicate) -> void {
  iterator pred = before_begin();
  while (next(pred) != end()) {
    const auto i = next(pred);
    if (predicate(*i))
      erase_after(pred);
    else
      pred = i;
  }
}

template<typename T, typename A>
auto forward_list<T, A>::unique() -> void {
  unique(equal_to<value_type>());
}

template<typename T, typename A>
template<typename BinaryPredicate>
auto forward_list<T, A>::unique(BinaryPredicate compare) -> void {
  using placeholders::_1;

  for (iterator b = begin(); b != end(); ++b) {
    iterator e = find_if_not(next(b), end(),
                             bind(ref(compare), ref(*b), _1));
    erase_after(b, e);
  }
}

template<typename T, typename A>
auto forward_list<T, A>::merge(forward_list&& other) -> void {
  using placeholders::_1;

  merge(move(other), less<value_type>());
}

template<typename T, typename A>
template<typename BinaryPredicate>
auto forward_list<T, A>::merge(forward_list&& other,
                               BinaryPredicate predicate) -> void {
  using placeholders::_1;
  using placeholders::_2;

  data_.merge(other.data_, bind(ref(predicate),
                                bind(&elem::value, _1),
                                bind(&elem::value, _2)));
}

template<typename T, typename A>
auto forward_list<T, A>::sort() -> void {
  sort(less<value_type>());
}

template<typename T, typename A>
template<typename Compare>
auto forward_list<T, A>::sort(Compare predicate) -> void {
  using placeholders::_1;

  data_.sort(bind(ref(predicate),
                  bind(&elem::value, _1),
                  bind(&elem::value, _1)));
}

template<typename T, typename A>
auto forward_list<T, A>::reverse() noexcept -> void {
  data_.reverse();
}


template<typename T, typename A>
forward_list<T, A>::iterator::iterator(typename data_type::iterator impl)
    noexcept
: impl_(impl)
{}

template<typename T, typename A>
auto forward_list<T, A>::iterator::operator*() const noexcept -> T& {
  return impl_->value;
}

template<typename T, typename A>
auto forward_list<T, A>::iterator::operator->() const noexcept -> T* {
  return &impl_->value;
}

template<typename T, typename A>
auto forward_list<T, A>::iterator::operator++() noexcept -> iterator& {
  ++impl_;
  return *this;
}

template<typename T, typename A>
auto forward_list<T, A>::iterator::operator++(int) noexcept -> iterator {
  iterator tmp = *this;
  ++impl_;
  return tmp;
}

template<typename T, typename A>
auto forward_list<T, A>::iterator::operator==(const iterator& o)
    const noexcept -> bool {
  return impl_ == o.impl_;
}

template<typename T, typename A>
auto forward_list<T, A>::iterator::operator!=(const iterator& o)
    const noexcept -> bool {
  return !(*this == o);
}

template<typename T, typename A>
auto forward_list<T, A>::iterator::operator==(const const_iterator& o)
    const noexcept -> bool {
  return impl_ == o.impl_;
}

template<typename T, typename A>
auto forward_list<T, A>::iterator::operator!=(const const_iterator& o)
    const noexcept -> bool {
  return !(*this == o);
}


template<typename T, typename A>
forward_list<T, A>::const_iterator::const_iterator(
    typename data_type::const_iterator impl) noexcept
: impl_(impl)
{}

template<typename T, typename A>
forward_list<T, A>::const_iterator::const_iterator(const iterator& i) noexcept
: impl_(i.impl_)
{}

template<typename T, typename A>
auto forward_list<T, A>::const_iterator::operator*() const noexcept ->
    const T& {
  return impl_->value;
}

template<typename T, typename A>
auto forward_list<T, A>::const_iterator::operator->() const noexcept ->
    const T* {
  return &impl_->value;
}

template<typename T, typename A>
auto forward_list<T, A>::const_iterator::operator++() noexcept ->
    const_iterator& {
  ++impl_;
  return *this;
}

template<typename T, typename A>
auto forward_list<T, A>::const_iterator::operator++(int) noexcept ->
    const_iterator {
  const_iterator tmp = *this;
  ++impl_;
  return tmp;
}

template<typename T, typename A>
auto forward_list<T, A>::const_iterator::operator==(const iterator& o)
    const noexcept -> bool {
  return impl_ == o.impl_;
}

template<typename T, typename A>
auto forward_list<T, A>::const_iterator::operator!=(const iterator& o)
    const noexcept -> bool {
  return !(*this == o);
}

template<typename T, typename A>
auto forward_list<T, A>::const_iterator::operator==(const const_iterator& o)
    const noexcept -> bool {
  return impl_ == o.impl_;
}

template<typename T, typename A>
auto forward_list<T, A>::const_iterator::operator!=(const const_iterator& o)
    const noexcept -> bool {
  return !(*this == o);
}


template<typename T, typename A>
auto operator==(const forward_list<T, A>& a,
                const forward_list<T, A>& b) -> bool {
  return equal(a.begin(), a.end(), b.begin(), b.end());
}

template<typename T, typename A>
auto operator!=(const forward_list<T, A>& a,
                const forward_list<T, A>& b) -> bool {
  return !(a == b);
}

template<typename T, typename A>
auto operator<(const forward_list<T, A>& a,
               const forward_list<T, A>& b) -> bool {
  return lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
}

template<typename T, typename A>
auto operator>(const forward_list<T, A>& a,
               const forward_list<T, A>& b) -> bool {
  return b < a;
}

template<typename T, typename A>
auto operator<=(const forward_list<T, A>& a,
                const forward_list<T, A>& b) -> bool {
  return !(b < a);
}

template<typename T, typename A>
auto operator>=(const forward_list<T, A>& a,
                const forward_list<T, A>& b) -> bool {
  return !(a < b);
}

template<typename T, typename A>
auto swap(forward_list<T, A>& a, forward_list<T, A>& b) -> void {
  a.swap(b);
}


_namespace_end(std)

#endif /* _FORWARD_LIST_INL_H_ */
