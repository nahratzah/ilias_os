#ifndef _SET_INL_H_
#define _SET_INL_H_

#include <cdecl.h>
#include <set>
#include <algorithm>
#include <iterator>
#include <stdimpl/alloc_deleter.h>

_namespace_begin(std)


template<typename Key, typename Cmp, typename A>
set<Key, Cmp, A>::set(const Cmp& cmp, const A& a)
: alloc_base(a),
  data_(cmp)
{}

template<typename Key, typename Cmp, typename A>
template<typename InputIterator>
set<Key, Cmp, A>::set(InputIterator b, InputIterator e,
                      const Cmp& cmp, const A& a)
: set(cmp, a)
{
  copy(b, e, inserter(*this, end()));
}

template<typename Key, typename Cmp, typename A>
set<Key, Cmp, A>::set(const set& s)
: set(s.begin(), s.end(), s.key_comp(), s.get_allocator_for_copy_())
{}

template<typename Key, typename Cmp, typename A>
set<Key, Cmp, A>::set(set&& s)
: alloc_base(move(s)),
  data_(move(s.data_))
{}

template<typename Key, typename Cmp, typename A>
set<Key, Cmp, A>::set(const set& s, const A& a)
: set(s.begin(), s.end(), s.key_comp(), a)
{}

template<typename Key, typename Cmp, typename A>
set<Key, Cmp, A>::set(set&& s, const A& a)
: set(s.key_comp(), a)
{
  if (this->get_allocator_() == s.get_allocator_()) {
    data_ = move(s.data_);
    size_ = exchange(s.size_, 0);
  } else {
    move(s.begin(), s.end(), inserter(*this, end()));
  }
}

template<typename Key, typename Cmp, typename A>
set<Key, Cmp, A>::set(initializer_list<value_type> il,
                      const Cmp& cmp, const A& a)
: set(il.begin(), il.end(), cmp, a)
{}

template<typename Key, typename Cmp, typename A>
template<typename InputIterator>
set<Key, Cmp, A>::set(InputIterator b, InputIterator e, const A& a)
: set(b, e, Cmp(), a)
{}

template<typename Key, typename Cmp, typename A>
set<Key, Cmp, A>::set(initializer_list<value_type> il, const A& a)
: set(il, Cmp(), a)
{}

template<typename Key, typename Cmp, typename A>
set<Key, Cmp, A>::~set() noexcept {
  clear();
}

template<typename Key, typename Cmp, typename A>
auto set<Key, Cmp, A>::operator=(const set& s) -> set& {
  if (&s != this) {
    clear();
    insert(s.begin(), s.end());
  }
  return *this;
}

template<typename Key, typename Cmp, typename A>
auto set<Key, Cmp, A>::operator=(set&& s) -> set& {
  assert(&s != this);
  clear();
  if (this->get_allocator_() == s.get_allocator_()) {
    data_type(move(s.data_)).swap(data_);
    size_ = exchange(s.size_, 0);
  } else {
    move(s.begin(), s.end(), inserter(*this, end()));
  }
  return *this;
}

template<typename Key, typename Cmp, typename A>
auto set<Key, Cmp, A>::operator=(initializer_list<value_type> il) -> set& {
  clear();
  insert(il.begin(), il.end());
  return *this;
}

template<typename Key, typename Cmp, typename A>
auto set<Key, Cmp, A>::get_allocator() const noexcept -> allocator_type {
  return alloc_base::get_allocator();
}

template<typename Key, typename Cmp, typename A>
auto set<Key, Cmp, A>::begin() noexcept -> iterator {
  return data_.begin();
}

template<typename Key, typename Cmp, typename A>
auto set<Key, Cmp, A>::begin() const noexcept -> const_iterator {
  return data_.begin();
}

template<typename Key, typename Cmp, typename A>
auto set<Key, Cmp, A>::end() noexcept -> iterator {
  return data_.end();
}

template<typename Key, typename Cmp, typename A>
auto set<Key, Cmp, A>::end() const noexcept -> const_iterator {
  return data_.end();
}

template<typename Key, typename Cmp, typename A>
auto set<Key, Cmp, A>::rbegin() noexcept -> reverse_iterator {
  return data_.rbegin();
}

template<typename Key, typename Cmp, typename A>
auto set<Key, Cmp, A>::rbegin() const noexcept -> const_reverse_iterator {
  return data_.rbegin();
}

template<typename Key, typename Cmp, typename A>
auto set<Key, Cmp, A>::rend() noexcept -> reverse_iterator {
  return data_.rend();
}

template<typename Key, typename Cmp, typename A>
auto set<Key, Cmp, A>::rend() const noexcept -> const_reverse_iterator {
  return data_.rend();
}

template<typename Key, typename Cmp, typename A>
auto set<Key, Cmp, A>::cbegin() const noexcept -> const_iterator {
  return data_.cbegin();
}

template<typename Key, typename Cmp, typename A>
auto set<Key, Cmp, A>::cend() const noexcept -> const_iterator {
  return data_.cend();
}

template<typename Key, typename Cmp, typename A>
auto set<Key, Cmp, A>::crbegin() const noexcept -> const_reverse_iterator {
  return data_.crbegin();
}

template<typename Key, typename Cmp, typename A>
auto set<Key, Cmp, A>::crend() const noexcept -> const_reverse_iterator {
  return data_.crend();
}

template<typename Key, typename Cmp, typename A>
auto set<Key, Cmp, A>::empty() const noexcept -> bool {
  return data_.empty();
}

template<typename Key, typename Cmp, typename A>
auto set<Key, Cmp, A>::size() const noexcept -> size_type {
  return size_;
}

template<typename Key, typename Cmp, typename A>
auto set<Key, Cmp, A>::max_size() const noexcept -> size_type {
  return allocator_traits<typename alloc_base::allocator_type>::max_size(
      this->get_allocator_());
}

template<typename Key, typename Cmp, typename A>
template<typename... Args>
auto set<Key, Cmp, A>::emplace(Args&&... args) -> pair<iterator, bool> {
  void* hint = (empty() ?
                static_cast<void*>(this) :
                static_cast<void*>(&*data_.root()));
  auto ptr = impl::new_alloc_deleter<elem>(this->get_allocator_(),
                                           hint,
                                           forward<Args>(args)...);
  auto rv = data_.link(ptr.get(), false);
  if (rv.second) {
    ptr.release();
    ++size_;
  }
  return rv;
}

template<typename Key, typename Cmp, typename A>
template<typename... Args>
auto set<Key, Cmp, A>::emplace_hint(const_iterator pos, Args&&... args) ->
    iterator {
  void* hint = (empty() ?
                static_cast<void*>(this) :
                static_cast<void*>(&*data_.root()));
  auto ptr = impl::new_alloc_deleter<elem>(this->get_allocator_(),
                                           hint,
                                           forward<Args>(args)...);
  auto rv = data_.link(pos.impl(), ptr.get(), false);
  if (rv.second) {
    ptr.release();
    ++size_;
  }
  return rv.first;
}

template<typename Key, typename Cmp, typename A>
auto set<Key, Cmp, A>::insert(const value_type& v) -> pair<iterator, bool> {
  void* hint = (empty() ?
                static_cast<void*>(this) :
                static_cast<void*>(&*data_.root()));
  auto ptr = impl::new_alloc_deleter<elem>(this->get_allocator_(),
                                           hint,
                                           v);
  auto rv = data_.link(ptr.get(), false);
  if (rv.second) {
    ptr.release();
    ++size_;
  }
  return rv;
}

template<typename Key, typename Cmp, typename A>
auto set<Key, Cmp, A>::insert(value_type&& v) -> pair<iterator, bool> {
  void* hint = (empty() ?
                static_cast<void*>(this) :
                static_cast<void*>(&*data_.root()));
  auto ptr = impl::new_alloc_deleter<elem>(this->get_allocator_(),
                                           hint,
                                           move(v));
  auto rv = data_.link(ptr.get(), false);
  if (rv.second) {
    ptr.release();
    ++size_;
  }
  return rv;
}

template<typename Key, typename Cmp, typename A>
auto set<Key, Cmp, A>::insert(const_iterator pos, const value_type& v) ->
    iterator {
  void* hint = (empty() ?
                static_cast<void*>(this) :
                static_cast<void*>(&*data_.root()));
  auto ptr = impl::new_alloc_deleter<elem>(this->get_allocator_(),
                                           hint,
                                           v);
  auto rv = data_.link(pos.impl(), ptr.get(), false);
  if (rv.second) {
    ptr.release();
    ++size_;
  }
  return rv.first;
}

template<typename Key, typename Cmp, typename A>
auto set<Key, Cmp, A>::insert(const_iterator pos, value_type&& v) ->
    iterator {
  void* hint = (empty() ?
                static_cast<void*>(this) :
                static_cast<void*>(&*data_.root()));
  auto ptr = impl::new_alloc_deleter<elem>(this->get_allocator_(),
                                           hint,
                                           move(v));
  auto rv = data_.link(pos.impl(), ptr.get(), false);
  if (rv.second) {
    ptr.release();
    ++size_;
  }
  return rv.first;
}

template<typename Key, typename Cmp, typename A>
template<typename InputIterator>
auto set<Key, Cmp, A>::insert(InputIterator b, InputIterator e) -> void {
  copy(b, e, inserter(*this, end()));
}

template<typename Key, typename Cmp, typename A>
auto set<Key, Cmp, A>::insert(initializer_list<value_type> il) -> void {
  insert(il.begin(), il.end());
}

template<typename Key, typename Cmp, typename A>
auto set<Key, Cmp, A>::erase(const_iterator i) noexcept -> iterator {
  assert(i != end());
  iterator rv = data_type::nonconst_iterator(next(i.impl()));
  auto ptr = impl::existing_alloc_deleter(this->get_allocator_(),
                                          data_.unlink(i.impl()));
  if (ptr) --size_;
  return rv;
}

template<typename Key, typename Cmp, typename A>
auto set<Key, Cmp, A>::erase(const key_type& k) noexcept -> size_type {
  const_iterator i = find(k);
  if (i == end()) return 0;
  erase(i);
  return 1;
}

template<typename Key, typename Cmp, typename A>
auto set<Key, Cmp, A>::erase(const_iterator b, const_iterator e) noexcept ->
    iterator {
  while (b != e) b = erase(b);
  return data_type::nonconst_iterator(e.impl());
}

template<typename Key, typename Cmp, typename A>
auto set<Key, Cmp, A>::swap(set& s) noexcept -> void {
  using _namespace(std)::swap;
  data_.swap(s.data_);
  swap(size_, s.size_);
}

template<typename Key, typename Cmp, typename A>
auto set<Key, Cmp, A>::clear() noexcept -> void {
  data_.unlink_all(impl::alloc_deleter_visitor(this->get_allocator_()));
  size_ = 0;
}

template<typename Key, typename Cmp, typename A>
auto set<Key, Cmp, A>::key_comp() const -> key_compare {
  return data_.key_comp().get_compare();
}

template<typename Key, typename Cmp, typename A>
auto set<Key, Cmp, A>::value_comp() const -> key_compare {
  return data_.value_comp().get_compare();
}

template<typename Key, typename Cmp, typename A>
auto set<Key, Cmp, A>::find(const key_type& k) -> iterator {
  return data_.find(k);
}

template<typename Key, typename Cmp, typename A>
auto set<Key, Cmp, A>::find(const key_type& k) const -> const_iterator {
  return data_.find(k);
}

template<typename Key, typename Cmp, typename A>
template<typename K>
auto set<Key, Cmp, A>::find(const K& k) -> iterator {
  return data_.find(k);
}

template<typename Key, typename Cmp, typename A>
template<typename K>
auto set<Key, Cmp, A>::find(const K& k) const -> const_iterator {
  return data_.find(k);
}

template<typename Key, typename Cmp, typename A>
auto set<Key, Cmp, A>::count(const key_type& k) const -> size_type {
  auto r = data_.equal_range(k);
  return distance(r.first, r.second);
}

template<typename Key, typename Cmp, typename A>
template<typename K>
auto set<Key, Cmp, A>::count(const K& k) const -> size_type {
  auto r = data_.equal_range(k);
  return distance(k.first, k.second);
}

template<typename Key, typename Cmp, typename A>
auto set<Key, Cmp, A>::lower_bound(const key_type& k) -> iterator {
  return data_.lower_bound(k);
}

template<typename Key, typename Cmp, typename A>
auto set<Key, Cmp, A>::lower_bound(const key_type& k) const -> const_iterator {
  return data_.lower_bound(k);
}

template<typename Key, typename Cmp, typename A>
template<typename K>
auto set<Key, Cmp, A>::lower_bound(const K& k) -> iterator {
  return data_.lower_bound(k);
}

template<typename Key, typename Cmp, typename A>
template<typename K>
auto set<Key, Cmp, A>::lower_bound(const K& k) const -> const_iterator {
  return data_.lower_bound(k);
}

template<typename Key, typename Cmp, typename A>
auto set<Key, Cmp, A>::upper_bound(const key_type& k) -> iterator {
  return data_.upper_bound(k);
}

template<typename Key, typename Cmp, typename A>
auto set<Key, Cmp, A>::upper_bound(const key_type& k) const -> const_iterator {
  return data_.upper_bound(k);
}

template<typename Key, typename Cmp, typename A>
template<typename K>
auto set<Key, Cmp, A>::upper_bound(const K& k) -> iterator {
  return data_.upper_bound(k);
}

template<typename Key, typename Cmp, typename A>
template<typename K>
auto set<Key, Cmp, A>::upper_bound(const K& k) const -> const_iterator {
  return data_.upper_bound(k);
}

template<typename Key, typename Cmp, typename A>
auto set<Key, Cmp, A>::equal_range(const key_type& k) ->
    pair<iterator, iterator> {
  return data_.equal_range(k);
}

template<typename Key, typename Cmp, typename A>
auto set<Key, Cmp, A>::equal_range(const key_type& k) const ->
    pair<const_iterator, const_iterator> {
  return data_.equal_range(k);
}

template<typename Key, typename Cmp, typename A>
template<typename K>
auto set<Key, Cmp, A>::equal_range(const K& k) ->
    pair<iterator, iterator> {
  return data_.equal_range(k);
}

template<typename Key, typename Cmp, typename A>
template<typename K>
auto set<Key, Cmp, A>::equal_range(const K& k) const ->
    pair<const_iterator, const_iterator> {
  return data_.equal_range(k);
}

template<typename Key, typename Cmp, typename A>
bool operator==(const set<Key, Cmp, A>& x,
                const set<Key, Cmp, A>& y) {
  return x.size() == y.size() && equal(x.begin(), x.end(), y.begin(), y.end());
}

template<typename Key, typename Cmp, typename A>
bool operator<(const set<Key, Cmp, A>& x,
               const set<Key, Cmp, A>& y) {
  return lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}

template<typename Key, typename Cmp, typename A>
bool operator!=(const set<Key, Cmp, A>& x,
                const set<Key, Cmp, A>& y) {
  return !(x == y);
}

template<typename Key, typename Cmp, typename A>
bool operator>(const set<Key, Cmp, A>& x,
               const set<Key, Cmp, A>& y) {
  return y < x;
}

template<typename Key, typename Cmp, typename A>
bool operator<=(const set<Key, Cmp, A>& x,
                const set<Key, Cmp, A>& y) {
  return !(y < x);
}

template<typename Key, typename Cmp, typename A>
bool operator>=(const set<Key, Cmp, A>& x,
                const set<Key, Cmp, A>& y) {
  return !(x < y);
}

template<typename Key, typename Cmp, typename A>
void swap(const set<Key, Cmp, A>& x,
          const set<Key, Cmp, A>& y) noexcept {
  x.swap(y);
}


template<typename Key, typename Cmp, typename A>
multiset<Key, Cmp, A>::multiset(const Cmp& cmp, const A& a)
: alloc_base(a),
  data_(cmp)
{}

template<typename Key, typename Cmp, typename A>
template<typename InputIterator>
multiset<Key, Cmp, A>::multiset(InputIterator b, InputIterator e,
                      const Cmp& cmp, const A& a)
: multiset(cmp, a)
{
  copy(b, e, inserter(*this, end()));
}

template<typename Key, typename Cmp, typename A>
multiset<Key, Cmp, A>::multiset(const multiset& s)
: multiset(s.begin(), s.end(), s.key_comp(), s.get_allocator_for_copy_())
{}

template<typename Key, typename Cmp, typename A>
multiset<Key, Cmp, A>::multiset(multiset&& s)
: alloc_base(move(s)),
  data_(move(s.data_))
{}

template<typename Key, typename Cmp, typename A>
multiset<Key, Cmp, A>::multiset(const multiset& s, const A& a)
: multiset(s.begin(), s.end(), s.key_comp(), a)
{}

template<typename Key, typename Cmp, typename A>
multiset<Key, Cmp, A>::multiset(multiset&& s, const A& a)
: multiset(s.key_comp(), a)
{
  if (this->get_allocator_() == s.get_allocator_()) {
    data_ = move(s.data_);
    size_ = exchange(s.size_, 0);
  } else {
    move(s.begin(), s.end(), inserter(*this, end()));
  }
}

template<typename Key, typename Cmp, typename A>
multiset<Key, Cmp, A>::multiset(initializer_list<value_type> il,
                                const Cmp& cmp, const A& a)
: multiset(il.begin(), il.end(), cmp, a)
{}

template<typename Key, typename Cmp, typename A>
template<typename InputIterator>
multiset<Key, Cmp, A>::multiset(InputIterator b, InputIterator e, const A& a)
: multiset(b, e, Cmp(), a)
{}

template<typename Key, typename Cmp, typename A>
multiset<Key, Cmp, A>::multiset(initializer_list<value_type> il, const A& a)
: multiset(il, Cmp(), a)
{}

template<typename Key, typename Cmp, typename A>
multiset<Key, Cmp, A>::~multiset() noexcept {
  clear();
}

template<typename Key, typename Cmp, typename A>
auto multiset<Key, Cmp, A>::operator=(const multiset& s) -> multiset& {
  if (&s != this) {
    clear();
    insert(s.begin(), s.end());
  }
  return *this;
}

template<typename Key, typename Cmp, typename A>
auto multiset<Key, Cmp, A>::operator=(multiset&& s) -> multiset& {
  assert(&s != this);
  clear();
  if (this->get_allocator_() == s.get_allocator_()) {
    data_type(move(s.data_)).swap(data_);
    size_ = exchange(s.size_, 0);
  } else {
    move(s.begin(), s.end(), inserter(*this, end()));
  }
  return *this;
}

template<typename Key, typename Cmp, typename A>
auto multiset<Key, Cmp, A>::operator=(initializer_list<value_type> il) ->
    multiset& {
  clear();
  insert(il.begin(), il.end());
  return *this;
}

template<typename Key, typename Cmp, typename A>
auto multiset<Key, Cmp, A>::get_allocator() const noexcept -> allocator_type {
  return alloc_base::get_allocator();
}

template<typename Key, typename Cmp, typename A>
auto multiset<Key, Cmp, A>::begin() noexcept -> iterator {
  return data_.begin();
}

template<typename Key, typename Cmp, typename A>
auto multiset<Key, Cmp, A>::begin() const noexcept -> const_iterator {
  return data_.begin();
}

template<typename Key, typename Cmp, typename A>
auto multiset<Key, Cmp, A>::end() noexcept -> iterator {
  return data_.end();
}

template<typename Key, typename Cmp, typename A>
auto multiset<Key, Cmp, A>::end() const noexcept -> const_iterator {
  return data_.end();
}

template<typename Key, typename Cmp, typename A>
auto multiset<Key, Cmp, A>::rbegin() noexcept -> reverse_iterator {
  return data_.rbegin();
}

template<typename Key, typename Cmp, typename A>
auto multiset<Key, Cmp, A>::rbegin() const noexcept -> const_reverse_iterator {
  return data_.rbegin();
}

template<typename Key, typename Cmp, typename A>
auto multiset<Key, Cmp, A>::rend() noexcept -> reverse_iterator {
  return data_.rend();
}

template<typename Key, typename Cmp, typename A>
auto multiset<Key, Cmp, A>::rend() const noexcept -> const_reverse_iterator {
  return data_.rend();
}

template<typename Key, typename Cmp, typename A>
auto multiset<Key, Cmp, A>::cbegin() const noexcept -> const_iterator {
  return data_.cbegin();
}

template<typename Key, typename Cmp, typename A>
auto multiset<Key, Cmp, A>::cend() const noexcept -> const_iterator {
  return data_.cend();
}

template<typename Key, typename Cmp, typename A>
auto multiset<Key, Cmp, A>::crbegin() const noexcept ->
    const_reverse_iterator {
  return data_.crbegin();
}

template<typename Key, typename Cmp, typename A>
auto multiset<Key, Cmp, A>::crend() const noexcept -> const_reverse_iterator {
  return data_.crend();
}

template<typename Key, typename Cmp, typename A>
auto multiset<Key, Cmp, A>::empty() const noexcept -> bool {
  return data_.empty();
}

template<typename Key, typename Cmp, typename A>
auto multiset<Key, Cmp, A>::size() const noexcept -> size_type {
  return size_;
}

template<typename Key, typename Cmp, typename A>
auto multiset<Key, Cmp, A>::max_size() const noexcept -> size_type {
  return allocator_traits<typename alloc_base::allocator_type>::max_size(
      this->get_allocator_());
}

template<typename Key, typename Cmp, typename A>
template<typename... Args>
auto multiset<Key, Cmp, A>::emplace(Args&&... args) -> iterator {
  void* hint = (empty() ?
                static_cast<void*>(this) :
                static_cast<void*>(&*data_.root()));
  auto ptr = impl::new_alloc_deleter<elem>(this->get_allocator_(),
                                           hint,
                                           forward<Args>(args)...);
  auto rv = data_.link(ptr.get(), true);
  if (rv.second) {
    ptr.release();
    ++size_;
  }
  return rv.first;
}

template<typename Key, typename Cmp, typename A>
template<typename... Args>
auto multiset<Key, Cmp, A>::emplace_hint(const_iterator pos, Args&&... args) ->
    iterator {
  void* hint = (empty() ?
                static_cast<void*>(this) :
                static_cast<void*>(&*data_.root()));
  auto ptr = impl::new_alloc_deleter<elem>(this->get_allocator_(),
                                           hint,
                                           forward<Args>(args)...);
  auto rv = data_.link(pos.impl(), ptr.get(), true);
  if (rv.second) {
    ptr.release();
    ++size_;
  }
  return rv.first;
}

template<typename Key, typename Cmp, typename A>
auto multiset<Key, Cmp, A>::insert(const value_type& v) -> iterator {
  void* hint = (empty() ?
                static_cast<void*>(this) :
                static_cast<void*>(&*data_.root()));
  auto ptr = impl::new_alloc_deleter<elem>(this->get_allocator_(),
                                           hint,
                                           v);
  auto rv = data_.link(ptr.get(), true);
  if (rv.second) {
    ptr.release();
    ++size_;
  }
  return rv.first;
}

template<typename Key, typename Cmp, typename A>
auto multiset<Key, Cmp, A>::insert(value_type&& v) -> iterator {
  void* hint = (empty() ?
                static_cast<void*>(this) :
                static_cast<void*>(&*data_.root()));
  auto ptr = impl::new_alloc_deleter<elem>(this->get_allocator_(),
                                           hint,
                                           move(v));
  auto rv = data_.link(ptr.get(), true);
  if (rv.second) {
    ptr.release();
    ++size_;
  }
  return rv.first;
}

template<typename Key, typename Cmp, typename A>
auto multiset<Key, Cmp, A>::insert(const_iterator pos, const value_type& v) ->
    iterator {
  void* hint = (empty() ?
                static_cast<void*>(this) :
                static_cast<void*>(&*data_.root()));
  auto ptr = impl::new_alloc_deleter<elem>(this->get_allocator_(),
                                           hint,
                                           v);
  auto rv = data_.link(pos.impl(), ptr.get(), true);
  if (rv.second) {
    ptr.release();
    ++size_;
  }
  return rv.first;
}

template<typename Key, typename Cmp, typename A>
auto multiset<Key, Cmp, A>::insert(const_iterator pos, value_type&& v) ->
    iterator {
  void* hint = (empty() ?
                static_cast<void*>(this) :
                static_cast<void*>(&*data_.root()));
  auto ptr = impl::new_alloc_deleter<elem>(this->get_allocator_(),
                                           hint,
                                           move(v));
  auto rv = data_.link(pos.impl(), ptr.get(), true);
  if (rv.second) {
    ptr.release();
    ++size_;
  }
  return rv.first;
}

template<typename Key, typename Cmp, typename A>
template<typename InputIterator>
auto multiset<Key, Cmp, A>::insert(InputIterator b, InputIterator e) -> void {
  copy(b, e, inserter(*this, end()));
}

template<typename Key, typename Cmp, typename A>
auto multiset<Key, Cmp, A>::insert(initializer_list<value_type> il) -> void {
  insert(il.begin(), il.end());
}

template<typename Key, typename Cmp, typename A>
auto multiset<Key, Cmp, A>::erase(const_iterator i) noexcept -> iterator {
  assert(i != end());
  iterator rv = data_type::nonconst_iterator(next(i.impl()));
  auto ptr = impl::existing_alloc_deleter(this->get_allocator_(),
                                          data_.unlink(i.impl()));
  if (ptr) --size_;
  return rv;
}

template<typename Key, typename Cmp, typename A>
auto multiset<Key, Cmp, A>::erase(const key_type& k) noexcept -> size_type {
  const_iterator i = find(k);
  if (i == end()) return 0;
  erase(i);
  return 1;
}

template<typename Key, typename Cmp, typename A>
auto multiset<Key, Cmp, A>::erase(const_iterator b, const_iterator e)
    noexcept -> iterator {
  while (b != e) b = erase(b);
  return data_type::nonconst_iterator(e.impl());
}

template<typename Key, typename Cmp, typename A>
auto multiset<Key, Cmp, A>::swap(multiset& s) noexcept -> void {
  using _namespace(std)::swap;
  data_.swap(s.data_);
  swap(size_, s.size_);
}

template<typename Key, typename Cmp, typename A>
auto multiset<Key, Cmp, A>::clear() noexcept -> void {
  data_.unlink_all(impl::alloc_deleter_visitor(this->get_allocator_()));
  size_ = 0;
}

template<typename Key, typename Cmp, typename A>
auto multiset<Key, Cmp, A>::key_comp() const -> key_compare {
  return data_.key_comp().get_compare();
}

template<typename Key, typename Cmp, typename A>
auto multiset<Key, Cmp, A>::value_comp() const -> key_compare {
  return data_.value_comp().get_compare();
}

template<typename Key, typename Cmp, typename A>
auto multiset<Key, Cmp, A>::find(const key_type& k) -> iterator {
  return data_.find(k);
}

template<typename Key, typename Cmp, typename A>
auto multiset<Key, Cmp, A>::find(const key_type& k) const -> const_iterator {
  return data_.find(k);
}

template<typename Key, typename Cmp, typename A>
template<typename K>
auto multiset<Key, Cmp, A>::find(const K& k) -> iterator {
  return data_.find(k);
}

template<typename Key, typename Cmp, typename A>
template<typename K>
auto multiset<Key, Cmp, A>::find(const K& k) const -> const_iterator {
  return data_.find(k);
}

template<typename Key, typename Cmp, typename A>
auto multiset<Key, Cmp, A>::count(const key_type& k) const -> size_type {
  auto r = data_.equal_range(k);
  return distance(r.first, r.second);
}

template<typename Key, typename Cmp, typename A>
template<typename K>
auto multiset<Key, Cmp, A>::count(const K& k) const -> size_type {
  auto r = data_.equal_range(k);
  return distance(k.first, k.second);
}

template<typename Key, typename Cmp, typename A>
auto multiset<Key, Cmp, A>::lower_bound(const key_type& k) -> iterator {
  return data_.lower_bound(k);
}

template<typename Key, typename Cmp, typename A>
auto multiset<Key, Cmp, A>::lower_bound(const key_type& k) const ->
    const_iterator {
  return data_.lower_bound(k);
}

template<typename Key, typename Cmp, typename A>
template<typename K>
auto multiset<Key, Cmp, A>::lower_bound(const K& k) -> iterator {
  return data_.lower_bound(k);
}

template<typename Key, typename Cmp, typename A>
template<typename K>
auto multiset<Key, Cmp, A>::lower_bound(const K& k) const -> const_iterator {
  return data_.lower_bound(k);
}

template<typename Key, typename Cmp, typename A>
auto multiset<Key, Cmp, A>::upper_bound(const key_type& k) -> iterator {
  return data_.upper_bound(k);
}

template<typename Key, typename Cmp, typename A>
auto multiset<Key, Cmp, A>::upper_bound(const key_type& k) const ->
    const_iterator {
  return data_.upper_bound(k);
}

template<typename Key, typename Cmp, typename A>
template<typename K>
auto multiset<Key, Cmp, A>::upper_bound(const K& k) -> iterator {
  return data_.upper_bound(k);
}

template<typename Key, typename Cmp, typename A>
template<typename K>
auto multiset<Key, Cmp, A>::upper_bound(const K& k) const -> const_iterator {
  return data_.upper_bound(k);
}

template<typename Key, typename Cmp, typename A>
auto multiset<Key, Cmp, A>::equal_range(const key_type& k) ->
    pair<iterator, iterator> {
  return data_.equal_range(k);
}

template<typename Key, typename Cmp, typename A>
auto multiset<Key, Cmp, A>::equal_range(const key_type& k) const ->
    pair<const_iterator, const_iterator> {
  return data_.equal_range(k);
}

template<typename Key, typename Cmp, typename A>
template<typename K>
auto multiset<Key, Cmp, A>::equal_range(const K& k) ->
    pair<iterator, iterator> {
  return data_.equal_range(k);
}

template<typename Key, typename Cmp, typename A>
template<typename K>
auto multiset<Key, Cmp, A>::equal_range(const K& k) const ->
    pair<const_iterator, const_iterator> {
  return data_.equal_range(k);
}

template<typename Key, typename Cmp, typename A>
bool operator==(const multiset<Key, Cmp, A>& x,
                const multiset<Key, Cmp, A>& y) {
  return x.size() == y.size() && equal(x.begin(), x.end(), y.begin(), y.end());
}

template<typename Key, typename Cmp, typename A>
bool operator<(const multiset<Key, Cmp, A>& x,
               const multiset<Key, Cmp, A>& y) {
  return lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}

template<typename Key, typename Cmp, typename A>
bool operator!=(const multiset<Key, Cmp, A>& x,
                const multiset<Key, Cmp, A>& y) {
  return !(x == y);
}

template<typename Key, typename Cmp, typename A>
bool operator>(const multiset<Key, Cmp, A>& x,
               const multiset<Key, Cmp, A>& y) {
  return y < x;
}

template<typename Key, typename Cmp, typename A>
bool operator<=(const multiset<Key, Cmp, A>& x,
                const multiset<Key, Cmp, A>& y) {
  return !(y < x);
}

template<typename Key, typename Cmp, typename A>
bool operator>=(const multiset<Key, Cmp, A>& x,
                const multiset<Key, Cmp, A>& y) {
  return !(x < y);
}

template<typename Key, typename Cmp, typename A>
void swap(const multiset<Key, Cmp, A>& x,
          const multiset<Key, Cmp, A>& y) noexcept {
  x.swap(y);
}


_namespace_end(std)

#endif /* _SET_INL_H_ */
