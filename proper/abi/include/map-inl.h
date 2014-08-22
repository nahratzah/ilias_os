#ifndef _MAP_INL_H_
#define _MAP_INL_H_

#include <map>
#include <cassert>
#include <tuple>
#include <iterator>
#include <stdimpl/alloc_deleter.h>

_namespace_begin(std)


template<typename Key, typename T, typename Compare, typename Allocator>
auto map<Key, T, Compare, Allocator>::value_compare::operator()(
    const value_type& x, const value_type& y) const -> bool {
  return comp(x.first, y.first);
}


template<typename Key, typename T, typename Cmp, typename A>
map<Key, T, Cmp, A>::map(const Cmp& cmp, const A& a)
: alloc_base(a),
  data_(cmp)
{}

template<typename Key, typename T, typename Cmp, typename A>
template<typename InputIter>
map<Key, T, Cmp, A>::map(InputIter b, InputIter e, const Cmp& cmp, const A& a)
: map(cmp, a)
{
  copy(b, e, inserter(*this, end()));
}

template<typename Key, typename T, typename Cmp, typename A>
map<Key, T, Cmp, A>::map(const map& m)
: map(m.begin(), m.end(), m.key_comp(), m.get_allocator_for_copy_())
{}

template<typename Key, typename T, typename Cmp, typename A>
map<Key, T, Cmp, A>::map(map&& m)
: alloc_base(move(m)),
  data_(move(m.data_))
{}

template<typename Key, typename T, typename Cmp, typename A>
map<Key, T, Cmp, A>::map(const map& m, const A& a)
: map(m.begin(), m.end(), m.key_comp(), a)
{}

template<typename Key, typename T, typename Cmp, typename A>
map<Key, T, Cmp, A>::map(map&& m, const A& a)
: map(m.key_comp(), a)
{
  if (this->get_allocator_() == m.get_allocator_()) {
    data_ = move(m.data_);
    size_ = exchange(m.size_, 0);
  } else {
    move(m.begin(), m.end(), inserter(*this, end()));
  }
}

template<typename Key, typename T, typename Cmp, typename A>
map<Key, T, Cmp, A>::map(initializer_list<value_type> il,
                         const Cmp& cmp, const A& a)
: map(il.begin(), il.end(), cmp, a)
{}

template<typename Key, typename T, typename Cmp, typename A>
map<Key, T, Cmp, A>::~map() noexcept {
  clear();
}

template<typename Key, typename T, typename Cmp, typename A>
auto map<Key, T, Cmp, A>::operator=(const map& m) -> map& {
  if (&m != this) {
    clear();
    insert(m.begin(), m.end());
  }
  return *this;
}

template<typename Key, typename T, typename Cmp, typename A>
auto map<Key, T, Cmp, A>::operator=(map&& m) -> map& {
  assert(&m != this);
  clear();
  if (this->get_allocator_() == m.get_allocator_()) {
    data_type(move(m.data_)).swap(data_);
    size_ = exchange(m.size_, 0);
  } else {
    move(m.begin(), m.end(), inserter(*this, end()));
  }
  return *this;
}

template<typename Key, typename T, typename Cmp, typename A>
auto map<Key, T, Cmp, A>::operator=(initializer_list<value_type> il) -> map& {
  clear();
  insert(il.begin(), il.end());
  return *this;
}

template<typename Key, typename T, typename Cmp, typename A>
auto map<Key, T, Cmp, A>::get_allocator() const noexcept -> allocator_type {
  return alloc_base::get_allocator();
}

template<typename Key, typename T, typename Cmp, typename A>
auto map<Key, T, Cmp, A>::begin() noexcept -> iterator {
  return data_.begin();
}

template<typename Key, typename T, typename Cmp, typename A>
auto map<Key, T, Cmp, A>::begin() const noexcept -> const_iterator {
  return data_.begin();
}

template<typename Key, typename T, typename Cmp, typename A>
auto map<Key, T, Cmp, A>::end() noexcept -> iterator {
  return data_.end();
}

template<typename Key, typename T, typename Cmp, typename A>
auto map<Key, T, Cmp, A>::end() const noexcept -> const_iterator {
  return data_.end();
}

template<typename Key, typename T, typename Cmp, typename A>
auto map<Key, T, Cmp, A>::rbegin() noexcept -> reverse_iterator {
  return data_.rbegin();
}

template<typename Key, typename T, typename Cmp, typename A>
auto map<Key, T, Cmp, A>::rbegin() const noexcept -> const_reverse_iterator {
  return data_.rbegin();
}

template<typename Key, typename T, typename Cmp, typename A>
auto map<Key, T, Cmp, A>::rend() noexcept -> reverse_iterator {
  return data_.rend();
}

template<typename Key, typename T, typename Cmp, typename A>
auto map<Key, T, Cmp, A>::rend() const noexcept -> const_reverse_iterator {
  return data_.rend();
}

template<typename Key, typename T, typename Cmp, typename A>
auto map<Key, T, Cmp, A>::cbegin() const noexcept -> const_iterator {
  return data_.cbegin();
}

template<typename Key, typename T, typename Cmp, typename A>
auto map<Key, T, Cmp, A>::cend() const noexcept -> const_iterator {
  return data_.cend();
}

template<typename Key, typename T, typename Cmp, typename A>
auto map<Key, T, Cmp, A>::crbegin() const noexcept -> const_reverse_iterator {
  return data_.crbegin();
}

template<typename Key, typename T, typename Cmp, typename A>
auto map<Key, T, Cmp, A>::crend() const noexcept -> const_reverse_iterator {
  return data_.crend();
}

template<typename Key, typename T, typename Cmp, typename A>
auto map<Key, T, Cmp, A>::empty() const noexcept -> bool {
  return data_.empty();
}

template<typename Key, typename T, typename Cmp, typename A>
auto map<Key, T, Cmp, A>::size() const noexcept -> size_type {
  return size_;
}

template<typename Key, typename T, typename Cmp, typename A>
auto map<Key, T, Cmp, A>::max_size() const noexcept -> size_type {
  return allocator_traits<typename alloc_base::allocator_type>::max_size(
      this->get_allocator_());
}

template<typename Key, typename T, typename Cmp, typename A>
auto map<Key, T, Cmp, A>::operator[](const key_type& k) -> mapped_type& {
  iterator i = find(k);
  if (i == end()) tie(i, ignore) = emplace(k, mapped_type());
  return i->second;
}

template<typename Key, typename T, typename Cmp, typename A>
auto map<Key, T, Cmp, A>::operator[](key_type&& k) -> mapped_type& {
  iterator i = find(k);
  if (i == end()) tie(i, ignore) = emplace(move(k), mapped_type());
  return i->second;
}

template<typename Key, typename T, typename Cmp, typename A>
auto map<Key, T, Cmp, A>::at(const key_type& k) -> mapped_type& {
  iterator i = find(k);
  if (i == end()) throw out_of_range("map::at");
  return i->second;
}

template<typename Key, typename T, typename Cmp, typename A>
auto map<Key, T, Cmp, A>::at(const key_type& k) const -> const mapped_type& {
  const_iterator i = find(k);
  if (i == end()) throw out_of_range("map::at");
  return i->second;
}

template<typename Key, typename T, typename Cmp, typename A>
template<typename... Args>
auto map<Key, T, Cmp, A>::emplace(Args&&... args) -> pair<iterator, bool> {
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

template<typename Key, typename T, typename Cmp, typename A>
template<typename... Args>
auto map<Key, T, Cmp, A>::emplace_hint(const_iterator pos, Args&&... args) ->
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

template<typename Key, typename T, typename Cmp, typename A>
auto map<Key, T, Cmp, A>::insert(const value_type& v) -> pair<iterator, bool> {
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

template<typename Key, typename T, typename Cmp, typename A>
template<typename P>
auto map<Key, T, Cmp, A>::insert(P&& v) ->
    enable_if_t<is_convertible<P, value_type>::value, pair<iterator, bool>> {
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

template<typename Key, typename T, typename Cmp, typename A>
auto map<Key, T, Cmp, A>::insert(const_iterator pos, const value_type& v) ->
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

template<typename Key, typename T, typename Cmp, typename A>
template<typename P>
auto map<Key, T, Cmp, A>::insert(const_iterator pos, P&& v) ->
    enable_if_t<is_convertible<P, value_type>::value, iterator> {
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

template<typename Key, typename T, typename Cmp, typename A>
template<typename InputIter>
auto map<Key, T, Cmp, A>::insert(InputIter b, InputIter e) -> void {
  copy(b, e, inserter(*this, end()));
}

template<typename Key, typename T, typename Cmp, typename A>
auto map<Key, T, Cmp, A>::insert(initializer_list<value_type> il) -> void {
  insert(il.begin(), il.end());
}

template<typename Key, typename T, typename Cmp, typename A>
auto map<Key, T, Cmp, A>::erase(const_iterator i) noexcept -> iterator {
  assert(i != end());
  iterator rv = data_type::nonconst_iterator(next(i.impl()));
  auto ptr = impl::existing_alloc_deleter(this->get_allocator_(),
                                          data_.unlink(i.impl()));
  if (ptr) --size_;
  return rv;
}

template<typename Key, typename T, typename Cmp, typename A>
auto map<Key, T, Cmp, A>::erase(const key_type& k) noexcept -> size_type {
  const_iterator i = find(k);
  if (i == end()) return 0;
  erase(i);
  return 1;
}

template<typename Key, typename T, typename Cmp, typename A>
auto map<Key, T, Cmp, A>::erase(const_iterator b, const_iterator e) noexcept ->
    iterator {
  while (b != e) b = erase(b);
  return data_type::nonconst_iterator(e.impl());
}

template<typename Key, typename T, typename Cmp, typename A>
auto map<Key, T, Cmp, A>::swap(map& m) noexcept -> void {
  using _namespace(std)::swap;
  data_.swap(m.data_);
  swap(size_, m.size_);
}

template<typename Key, typename T, typename Cmp, typename A>
auto map<Key, T, Cmp, A>::clear() noexcept -> void {
  data_.unlink_all(impl::alloc_deleter_visitor(this->get_allocator_()));
  size_ = 0;
}

template<typename Key, typename T, typename Cmp, typename A>
auto map<Key, T, Cmp, A>::key_comp() const -> key_compare {
  return data_.key_comp().get_compare();
}

template<typename Key, typename T, typename Cmp, typename A>
auto map<Key, T, Cmp, A>::value_comp() const -> value_compare {
  return value_compare(data_.value_comp().get_compare());
}

template<typename Key, typename T, typename Cmp, typename A>
auto map<Key, T, Cmp, A>::find(const key_type& k) -> iterator {
  return data_.find(k);
}

template<typename Key, typename T, typename Cmp, typename A>
auto map<Key, T, Cmp, A>::find(const key_type& k) const -> const_iterator {
  return data_.find(k);
}

template<typename Key, typename T, typename Cmp, typename A>
auto map<Key, T, Cmp, A>::count(const key_type& k) const -> size_type {
  auto r = data_.equal_range(k);
  return distance(r.first, r.second);
}

template<typename Key, typename T, typename Cmp, typename A>
auto map<Key, T, Cmp, A>::lower_bound(const key_type& k) -> iterator {
  return data_.lower_bound(k);
}

template<typename Key, typename T, typename Cmp, typename A>
auto map<Key, T, Cmp, A>::lower_bound(const key_type& k) const ->
    const_iterator {
  return data_.lower_bound(k);
}

template<typename Key, typename T, typename Cmp, typename A>
auto map<Key, T, Cmp, A>::upper_bound(const key_type& k) -> iterator {
  return data_.upper_bound(k);
}

template<typename Key, typename T, typename Cmp, typename A>
auto map<Key, T, Cmp, A>::upper_bound(const key_type& k) const ->
    const_iterator {
  return data_.upper_bound(k);
}

template<typename Key, typename T, typename Cmp, typename A>
auto map<Key, T, Cmp, A>::equal_range(const key_type& k) ->
    pair<iterator, iterator> {
  return data_.equal_range(k);
}

template<typename Key, typename T, typename Cmp, typename A>
auto map<Key, T, Cmp, A>::equal_range(const key_type& k) const ->
    pair<const_iterator, const_iterator> {
  return data_.equal_range(k);
}

template<typename Key, typename T, typename Cmp, typename A>
bool operator==(const map<Key, T, Cmp, A>& x,
                const map<Key, T, Cmp, A>& y) {
  return x.size() == y.size() && equal(x.begin(), x.end(), y.begin(), y.end());
}

template<typename Key, typename T, typename Cmp, typename A>
bool operator<(const map<Key, T, Cmp, A>& x,
               const map<Key, T, Cmp, A>& y) {
  return lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}

template<typename Key, typename T, typename Cmp, typename A>
bool operator!=(const map<Key, T, Cmp, A>& x,
                const map<Key, T, Cmp, A>& y) {
  return !(x == y);
}

template<typename Key, typename T, typename Cmp, typename A>
bool operator>(const map<Key, T, Cmp, A>& x,
               const map<Key, T, Cmp, A>& y) {
  return y < x;
}

template<typename Key, typename T, typename Cmp, typename A>
bool operator<=(const map<Key, T, Cmp, A>& x,
                const map<Key, T, Cmp, A>& y) {
  return !(y < x);
}

template<typename Key, typename T, typename Cmp, typename A>
void swap(const map<Key, T, Cmp, A>& x,
          const map<Key, T, Cmp, A>& y) noexcept {
  x.swap(y);
}


template<typename Key, typename T, typename Compare, typename Allocator>
auto multimap<Key, T, Compare, Allocator>::value_compare::operator()(
    const value_type& x, const value_type& y) const -> bool {
  return comp(x.first, y.first);
}


template<typename Key, typename T, typename Cmp, typename A>
multimap<Key, T, Cmp, A>::multimap(const Cmp& cmp, const A& a)
: alloc_base(a),
  data_(cmp)
{}

template<typename Key, typename T, typename Cmp, typename A>
template<typename InputIter>
multimap<Key, T, Cmp, A>::multimap(InputIter b, InputIter e,
                                   const Cmp& cmp, const A& a)
: multimap(cmp, a)
{
  copy(b, e, inserter(*this, end()));
}

template<typename Key, typename T, typename Cmp, typename A>
multimap<Key, T, Cmp, A>::multimap(const multimap& m)
: multimap(m.begin(), m.end(), m.key_comp(), m.get_allocator_for_copy_())
{}

template<typename Key, typename T, typename Cmp, typename A>
multimap<Key, T, Cmp, A>::multimap(multimap&& m)
: alloc_base(move(m)),
  data_(move(m.data_))
{}

template<typename Key, typename T, typename Cmp, typename A>
multimap<Key, T, Cmp, A>::multimap(const multimap& m, const A& a)
: multimap(m.begin(), m.end(), m.key_comp(), a)
{}

template<typename Key, typename T, typename Cmp, typename A>
multimap<Key, T, Cmp, A>::multimap(multimap&& m, const A& a)
: multimap(m.key_comp(), a)
{
  if (this->get_allocator_() == m.get_allocator_()) {
    data_ = move(m.data_);
    size_ = exchange(m.size_, 0);
  } else {
    move(m.begin(), m.end(), inserter(*this, end()));
  }
}

template<typename Key, typename T, typename Cmp, typename A>
multimap<Key, T, Cmp, A>::multimap(initializer_list<value_type> il,
                                   const Cmp& cmp, const A& a)
: multimap(il.begin(), il.end(), cmp, a)
{}

template<typename Key, typename T, typename Cmp, typename A>
multimap<Key, T, Cmp, A>::~multimap() noexcept {
  clear();
}

template<typename Key, typename T, typename Cmp, typename A>
auto multimap<Key, T, Cmp, A>::operator=(const multimap& m) -> multimap& {
  if (&m != this) {
    clear();
    insert(m.begin(), m.end());
  }
  return *this;
}

template<typename Key, typename T, typename Cmp, typename A>
auto multimap<Key, T, Cmp, A>::operator=(multimap&& m) -> multimap& {
  assert(&m != this);
  clear();
  if (this->get_allocator_() == m.get_allocator_()) {
    data_type(move(m.data_)).swap(data_);
    size_ = exchange(m.size_, 0);
  } else {
    move(m.begin(), m.end(), inserter(*this, end()));
  }
  return *this;
}

template<typename Key, typename T, typename Cmp, typename A>
auto multimap<Key, T, Cmp, A>::operator=(initializer_list<value_type> il) ->
    multimap& {
  clear();
  insert(il.begin(), il.end());
  return *this;
}

template<typename Key, typename T, typename Cmp, typename A>
auto multimap<Key, T, Cmp, A>::get_allocator() const noexcept ->
    allocator_type {
  return alloc_base::get_allocator();
}

template<typename Key, typename T, typename Cmp, typename A>
auto multimap<Key, T, Cmp, A>::begin() noexcept -> iterator {
  return data_.begin();
}

template<typename Key, typename T, typename Cmp, typename A>
auto multimap<Key, T, Cmp, A>::begin() const noexcept -> const_iterator {
  return data_.begin();
}

template<typename Key, typename T, typename Cmp, typename A>
auto multimap<Key, T, Cmp, A>::end() noexcept -> iterator {
  return data_.end();
}

template<typename Key, typename T, typename Cmp, typename A>
auto multimap<Key, T, Cmp, A>::end() const noexcept -> const_iterator {
  return data_.end();
}

template<typename Key, typename T, typename Cmp, typename A>
auto multimap<Key, T, Cmp, A>::rbegin() noexcept -> reverse_iterator {
  return data_.rbegin();
}

template<typename Key, typename T, typename Cmp, typename A>
auto multimap<Key, T, Cmp, A>::rbegin() const noexcept ->
    const_reverse_iterator {
  return data_.rbegin();
}

template<typename Key, typename T, typename Cmp, typename A>
auto multimap<Key, T, Cmp, A>::rend() noexcept -> reverse_iterator {
  return data_.rend();
}

template<typename Key, typename T, typename Cmp, typename A>
auto multimap<Key, T, Cmp, A>::rend() const noexcept ->
    const_reverse_iterator {
  return data_.rend();
}

template<typename Key, typename T, typename Cmp, typename A>
auto multimap<Key, T, Cmp, A>::cbegin() const noexcept -> const_iterator {
  return data_.cbegin();
}

template<typename Key, typename T, typename Cmp, typename A>
auto multimap<Key, T, Cmp, A>::cend() const noexcept -> const_iterator {
  return data_.cend();
}

template<typename Key, typename T, typename Cmp, typename A>
auto multimap<Key, T, Cmp, A>::crbegin() const noexcept ->
    const_reverse_iterator {
  return data_.crbegin();
}

template<typename Key, typename T, typename Cmp, typename A>
auto multimap<Key, T, Cmp, A>::crend() const noexcept ->
    const_reverse_iterator {
  return data_.crend();
}

template<typename Key, typename T, typename Cmp, typename A>
auto multimap<Key, T, Cmp, A>::empty() const noexcept -> bool {
  return data_.empty();
}

template<typename Key, typename T, typename Cmp, typename A>
auto multimap<Key, T, Cmp, A>::size() const noexcept -> size_type {
  return size_;
}

template<typename Key, typename T, typename Cmp, typename A>
auto multimap<Key, T, Cmp, A>::max_size() const noexcept -> size_type {
  return allocator_traits<typename alloc_base::allocator_type>::max_size(
      this->get_allocator_());
}

template<typename Key, typename T, typename Cmp, typename A>
auto multimap<Key, T, Cmp, A>::operator[](const key_type& k) -> mapped_type& {
  iterator i = find(k);
  if (i == end()) i = emplace(k, mapped_type());
  return i->second;
}

template<typename Key, typename T, typename Cmp, typename A>
auto multimap<Key, T, Cmp, A>::operator[](key_type&& k) -> mapped_type& {
  iterator i = find(k);
  if (i == end()) i = emplace(move(k), mapped_type());
  return i->second;
}

template<typename Key, typename T, typename Cmp, typename A>
auto multimap<Key, T, Cmp, A>::at(const key_type& k) -> mapped_type& {
  iterator i = find(k);
  if (i == end()) throw out_of_range("map::at");
  return i->second;
}

template<typename Key, typename T, typename Cmp, typename A>
auto multimap<Key, T, Cmp, A>::at(const key_type& k) const ->
    const mapped_type& {
  const_iterator i = find(k);
  if (i == end()) throw out_of_range("map::at");
  return i->second;
}

template<typename Key, typename T, typename Cmp, typename A>
template<typename... Args>
auto multimap<Key, T, Cmp, A>::emplace(Args&&... args) -> iterator {
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

template<typename Key, typename T, typename Cmp, typename A>
template<typename... Args>
auto multimap<Key, T, Cmp, A>::emplace_hint(const_iterator pos,
                                            Args&&... args) -> iterator {
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

template<typename Key, typename T, typename Cmp, typename A>
auto multimap<Key, T, Cmp, A>::insert(const value_type& v) -> iterator {
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

template<typename Key, typename T, typename Cmp, typename A>
template<typename P>
auto multimap<Key, T, Cmp, A>::insert(P&& v) ->
    enable_if_t<is_convertible<P, value_type>::value, iterator> {
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

template<typename Key, typename T, typename Cmp, typename A>
auto multimap<Key, T, Cmp, A>::insert(const_iterator pos,
                                      const value_type& v) -> iterator {
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

template<typename Key, typename T, typename Cmp, typename A>
template<typename P>
auto multimap<Key, T, Cmp, A>::insert(const_iterator pos, P&& v) ->
    enable_if_t<is_convertible<P, value_type>::value, iterator> {
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

template<typename Key, typename T, typename Cmp, typename A>
template<typename InputIter>
auto multimap<Key, T, Cmp, A>::insert(InputIter b, InputIter e) -> void {
  copy(b, e, inserter(*this, end()));
}

template<typename Key, typename T, typename Cmp, typename A>
auto multimap<Key, T, Cmp, A>::insert(initializer_list<value_type> il) ->
    void {
  insert(il.begin(), il.end());
}

template<typename Key, typename T, typename Cmp, typename A>
auto multimap<Key, T, Cmp, A>::erase(const_iterator i) noexcept -> iterator {
  assert(i != end());
  iterator rv = data_type::nonconst_iterator(next(i.impl()));
  auto ptr = impl::existing_alloc_deleter(this->get_allocator_(),
                                          data_.unlink(i.impl()));
  if (ptr) --size_;
  return rv;
}

template<typename Key, typename T, typename Cmp, typename A>
auto multimap<Key, T, Cmp, A>::erase(const key_type& k) noexcept -> size_type {
  const_iterator i = find(k);
  if (i == end()) return 0;
  erase(i);
  return 1;
}

template<typename Key, typename T, typename Cmp, typename A>
auto multimap<Key, T, Cmp, A>::erase(const_iterator b, const_iterator e)
    noexcept -> iterator {
  while (b != e) b = erase(b);
  return data_type::nonconst_iterator(e.impl());
}

template<typename Key, typename T, typename Cmp, typename A>
auto multimap<Key, T, Cmp, A>::swap(multimap& m) noexcept -> void {
  using _namespace(std)::swap;
  data_.swap(m.data_);
  swap(size_, m.size_);
}

template<typename Key, typename T, typename Cmp, typename A>
auto multimap<Key, T, Cmp, A>::clear() noexcept -> void {
  data_.unlink_all(impl::alloc_deleter_visitor(this->get_allocator_()));
  size_ = 0;
}

template<typename Key, typename T, typename Cmp, typename A>
auto multimap<Key, T, Cmp, A>::key_comp() const -> key_compare {
  return data_.key_comp().get_compare();
}

template<typename Key, typename T, typename Cmp, typename A>
auto multimap<Key, T, Cmp, A>::value_comp() const -> value_compare {
  return value_compare(data_.value_comp().get_compare());
}

template<typename Key, typename T, typename Cmp, typename A>
auto multimap<Key, T, Cmp, A>::find(const key_type& k) -> iterator {
  return data_.find(k);
}

template<typename Key, typename T, typename Cmp, typename A>
auto multimap<Key, T, Cmp, A>::find(const key_type& k) const ->
    const_iterator {
  return data_.find(k);
}

template<typename Key, typename T, typename Cmp, typename A>
auto multimap<Key, T, Cmp, A>::count(const key_type& k) const -> size_type {
  auto r = data_.equal_range(k);
  return distance(r.first, r.second);
}

template<typename Key, typename T, typename Cmp, typename A>
auto multimap<Key, T, Cmp, A>::lower_bound(const key_type& k) -> iterator {
  return data_.lower_bound(k);
}

template<typename Key, typename T, typename Cmp, typename A>
auto multimap<Key, T, Cmp, A>::lower_bound(const key_type& k) const ->
    const_iterator {
  return data_.lower_bound(k);
}

template<typename Key, typename T, typename Cmp, typename A>
auto multimap<Key, T, Cmp, A>::upper_bound(const key_type& k) -> iterator {
  return data_.upper_bound(k);
}

template<typename Key, typename T, typename Cmp, typename A>
auto multimap<Key, T, Cmp, A>::upper_bound(const key_type& k) const ->
    const_iterator {
  return data_.upper_bound(k);
}

template<typename Key, typename T, typename Cmp, typename A>
auto multimap<Key, T, Cmp, A>::equal_range(const key_type& k) ->
    pair<iterator, iterator> {
  return data_.equal_range(k);
}

template<typename Key, typename T, typename Cmp, typename A>
auto multimap<Key, T, Cmp, A>::equal_range(const key_type& k) const ->
    pair<const_iterator, const_iterator> {
  return data_.equal_range(k);
}

template<typename Key, typename T, typename Cmp, typename A>
bool operator==(const multimap<Key, T, Cmp, A>& x,
                const multimap<Key, T, Cmp, A>& y) {
  return x.size() == y.size() && equal(x.begin(), x.end(), y.begin(), y.end());
}

template<typename Key, typename T, typename Cmp, typename A>
bool operator<(const multimap<Key, T, Cmp, A>& x,
               const multimap<Key, T, Cmp, A>& y) {
  return lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}

template<typename Key, typename T, typename Cmp, typename A>
bool operator!=(const multimap<Key, T, Cmp, A>& x,
                const multimap<Key, T, Cmp, A>& y) {
  return !(x == y);
}

template<typename Key, typename T, typename Cmp, typename A>
bool operator>(const multimap<Key, T, Cmp, A>& x,
               const multimap<Key, T, Cmp, A>& y) {
  return y < x;
}

template<typename Key, typename T, typename Cmp, typename A>
bool operator<=(const multimap<Key, T, Cmp, A>& x,
                const multimap<Key, T, Cmp, A>& y) {
  return !(y < x);
}

template<typename Key, typename T, typename Cmp, typename A>
void swap(const multimap<Key, T, Cmp, A>& x,
          const multimap<Key, T, Cmp, A>& y) noexcept {
  x.swap(y);
}


_namespace_end(std)

#endif /* _MAP_INL_H_ */
