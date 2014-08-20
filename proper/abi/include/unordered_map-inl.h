#ifndef _UNORDERED_MAP_INL_H_
#define _UNORDERED_MAP_INL_H_

#include <unordered_map>
#include <algorithm>
#include <stdimpl/alloc_deleter.h>
#include <stdexcept>

_namespace_begin(std)


template<typename Key, typename T, typename Hash, typename Pred, typename A>
unordered_map<Key, T, Hash, Pred, A>::unordered_map(size_type buckets,
                                                    const hasher& h,
                                                    const key_equal& k,
                                                    const allocator_type& a)
: alloc_base(a),
  data_(buckets,
        impl::map_hash_wrapper<Hash, T, Key, elem>(h),
        impl::map_equal_wrapper<Pred, T, Key, elem>(k),
        a)
{}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
template<typename InputIter>
unordered_map<Key, T, Hash, Pred, A>::unordered_map(InputIter b, InputIter e,
                                                    size_type buckets,
                                                    const hasher& h,
                                                    const key_equal& k,
                                                    const allocator_type& a)
: unordered_map(buckets, h, k, a)
{
  insert(b, e);
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
unordered_map<Key, T, Hash, Pred, A>::unordered_map(const unordered_map& other)
: unordered_map(other.bucket_count(), other.hash_function(), other.key_eq(),
                other.get_allocator_for_copy_())
{
  max_load_factor(other.max_load_factor());
  insert(other.begin(), other.end());
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
unordered_map<Key, T, Hash, Pred, A>::unordered_map(unordered_map&& other)
: alloc_base(move(other)),
  data_(move(other.data_))
{}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
unordered_map<Key, T, Hash, Pred, A>::unordered_map(const allocator_type& a)
: alloc_base(a),
  data_(a)
{}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
unordered_map<Key, T, Hash, Pred, A>::unordered_map(const unordered_map& other,
                                                    const allocator_type& a)
: alloc_base(a),
  data_(0, other.hash_function(), other.key_eq(), a)
{
  max_load_factor(other.max_load_factor());
  insert(other.begin(), other.end());
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
unordered_map<Key, T, Hash, Pred, A>::unordered_map(unordered_map&& other,
                                                    const allocator_type& a)
: alloc_base(a),
  data_(0, other.hash_function(), other.key_eq(), a)
{
  max_load_factor(other.max_load_factor());
  move(other.begin(), other.end(), inserter(*this, end()));
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
unordered_map<Key, T, Hash, Pred, A>::unordered_map(
    initializer_list<value_type> il,
    size_type buckets, const hasher& h, const key_equal& k,
    const allocator_type& a)
: unordered_map(il.begin(), il.end(), buckets, h, k, a)
{}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
unordered_map<Key, T, Hash, Pred, A>::~unordered_map() noexcept {
  clear();
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto unordered_map<Key, T, Hash, Pred, A>::operator=(
    const unordered_map& rhs) -> unordered_map& {
  if (&rhs != this) {
    clear();
    insert(rhs.begin(), rhs.end());
  }
  return *this;
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto unordered_map<Key, T, Hash, Pred, A>::operator=(unordered_map&& rhs) ->
    unordered_map& {
  assert(&rhs != this);
  clear();
  if (this->get_allocator() == rhs.get_allocator_())
    data_type(move(rhs.data_)).swap(data_);
  else
    move(rhs.begin(), rhs.end(), inserter(*this, end()));
  return *this;
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto unordered_map<Key, T, Hash, Pred, A>::operator=(
    initializer_list<value_type> il) -> unordered_map& {
  clear();
  insert(il.begin(), il.end());
  return *this;
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto unordered_map<Key, T, Hash, Pred, A>::get_allocator() const ->
    allocator_type {
  return alloc_base::get_allocator();
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto unordered_map<Key, T, Hash, Pred, A>::empty() const noexcept ->
    bool {
  return data_.empty();
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto unordered_map<Key, T, Hash, Pred, A>::size() const noexcept ->
    size_type {
  return data_.size();
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto unordered_map<Key, T, Hash, Pred, A>::max_size() const noexcept ->
    size_type {
  return data_.max_size();
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto unordered_map<Key, T, Hash, Pred, A>::begin() noexcept ->
    iterator {
  return data_.begin();
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto unordered_map<Key, T, Hash, Pred, A>::begin() const noexcept ->
    const_iterator {
  return data_.begin();
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto unordered_map<Key, T, Hash, Pred, A>::end() noexcept ->
    iterator {
  return data_.end();
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto unordered_map<Key, T, Hash, Pred, A>::end() const noexcept ->
    const_iterator {
  return data_.end();
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto unordered_map<Key, T, Hash, Pred, A>::cbegin() const noexcept ->
    const_iterator {
  return begin();
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto unordered_map<Key, T, Hash, Pred, A>::cend() const noexcept ->
    const_iterator {
  return end();
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
template<typename... Args>
auto unordered_map<Key, T, Hash, Pred, A>::emplace(Args&&... args) ->
    pair<iterator, bool> {
  void* hint = (empty() ?
                static_cast<void*>(this) :
                static_cast<void*>(&*data_.begin()));
  auto ptr = impl::new_alloc_deleter<elem>(this->get_allocator_(),
                                           hint,
                                           forward<Args>(args)...);
  auto rv = data_.link(ptr.get(), false);
  if (rv.second)
    ptr.release();
  return rv;
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
template<typename... Args>
auto unordered_map<Key, T, Hash, Pred, A>::emplace_hint(const_iterator pos,
                                                        Args&&... args) ->
    iterator {
  void* hint = (pos == cend() ?
                static_cast<void*>(this) :
                const_cast<void*>(static_cast<const void*>(&*pos)));
  auto ptr = impl::new_alloc_deleter<elem>(this->get_allocator_(),
                                           hint,
                                           forward<Args>(args)...);
  auto rv = data_.link(ptr.get(), false);
  if (rv.second)
    ptr.release();
  return rv.first;
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto unordered_map<Key, T, Hash, Pred, A>::insert(const value_type& v) ->
    pair<iterator, bool> {
  void* hint = (empty() ?
                static_cast<void*>(this) :
                static_cast<void*>(&*data_.begin()));
  auto ptr = impl::new_alloc_deleter<elem>(this->get_allocator_(),
                                           hint,
                                           v);
  auto rv = data_.link(ptr.get(), false);
  if (rv.second)
    ptr.release();
  return rv;
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
template<typename P>
auto unordered_map<Key, T, Hash, Pred, A>::insert(P&& v) ->
    enable_if_t<is_convertible<P, value_type>::value, pair<iterator, bool>> {
  void* hint = (empty() ?
                static_cast<void*>(this) :
                static_cast<void*>(&*data_.begin()));
  auto ptr = impl::new_alloc_deleter<elem>(this->get_allocator_(),
                                           hint,
                                           move(v));
  auto rv = data_.link(ptr.get(), false);
  if (rv.second)
    ptr.release();
  return rv;
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto unordered_map<Key, T, Hash, Pred, A>::insert(const_iterator pos,
                                                  const value_type& v) ->
    iterator {
  void* hint = (pos == cend() ?
                static_cast<void*>(this) :
                const_cast<void*>(static_cast<const void*>(&*pos)));
  auto ptr = impl::new_alloc_deleter<elem>(this->get_allocator_(),
                                           hint,
                                           v);
  auto rv = data_.link(ptr.get(), false);
  if (rv.second)
    ptr.release();
  return rv.first;
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
template<typename P>
auto unordered_map<Key, T, Hash, Pred, A>::insert(const_iterator pos,
                                                  P&& v) ->
    enable_if_t<is_convertible<P, value_type>::value, iterator> {
  void* hint = (pos == cend() ?
                static_cast<void*>(this) :
                const_cast<void*>(static_cast<const void*>(&*pos)));
  auto ptr = impl::new_alloc_deleter<elem>(this->get_allocator_(),
                                           hint,
                                           move(v));
  auto rv = data_.link(ptr.get(), false);
  if (rv.second)
    ptr.release();
  return rv.first;
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
template<typename InputIter>
auto unordered_map<Key, T, Hash, Pred, A>::insert(InputIter b, InputIter e) ->
    void {
  copy(b, e, inserter(*this, end()));
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto unordered_map<Key, T, Hash, Pred, A>::insert(
    initializer_list<value_type> il) -> void {
  insert(il.begin(), il.end());
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto unordered_map<Key, T, Hash, Pred, A>::erase(const_iterator i) noexcept ->
    iterator {
  iterator rv = data_type::nonconst_iterator(next(i.impl()));
  auto ptr = impl::existing_alloc_deleter(this->get_allocator_(),
                                          data_.unlink(i.impl()));
  return rv;
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto unordered_map<Key, T, Hash, Pred, A>::erase(const key_type& k) noexcept ->
    size_type {
  iterator i = find(k);
  if (i == end()) return 0;
  erase(i);
  return 1;
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto unordered_map<Key, T, Hash, Pred, A>::erase(const_iterator b,
                                                 const_iterator e) noexcept ->
    iterator {
  while (b != e) b = erase(b);
  return data_type::nonconst_iterator(e.impl());
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto unordered_map<Key, T, Hash, Pred, A>::clear() noexcept -> void {
  data_.unlink_all(impl::alloc_deleter_visitor(this->get_allocator_()));
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto unordered_map<Key, T, Hash, Pred, A>::swap(unordered_map& rhs) ->
    void {
  data_.swap(rhs.data_);
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto unordered_map<Key, T, Hash, Pred, A>::hash_function() const -> hasher {
  return data_.hash_function().get_hash();
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto unordered_map<Key, T, Hash, Pred, A>::key_eq() const -> key_equal {
  return data_.key_eq().get_eq();
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto unordered_map<Key, T, Hash, Pred, A>::find(const key_type& k) ->
    iterator {
  return data_.find(k);
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto unordered_map<Key, T, Hash, Pred, A>::find(const key_type& k) const ->
    const_iterator {
  return data_.find(k);
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto unordered_map<Key, T, Hash, Pred, A>::count(const key_type& k) const ->
    size_type {
  return data_.count(k);
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto unordered_map<Key, T, Hash, Pred, A>::lower_bound(const key_type& k) ->
    iterator {
  return data_.lower_bound(k);
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto unordered_map<Key, T, Hash, Pred, A>::lower_bound(const key_type& k)
    const -> const_iterator {
  return data_.lower_bound(k);
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto unordered_map<Key, T, Hash, Pred, A>::upper_bound(const key_type& k) ->
    iterator {
  return data_.upper_bound(k);
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto unordered_map<Key, T, Hash, Pred, A>::upper_bound(const key_type& k)
    const -> const_iterator {
  return data_.upper_bound(k);
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto unordered_map<Key, T, Hash, Pred, A>::equal_range(const key_type& k) ->
    pair<iterator, iterator> {
  return data_.equal_range(k);
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto unordered_map<Key, T, Hash, Pred, A>::equal_range(const key_type& k)
    const -> pair<const_iterator, const_iterator> {
  return data_.equal_range(k);
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto unordered_map<Key, T, Hash, Pred, A>::operator[](const key_type& k) ->
    mapped_type& {
  iterator i = find(k);
  if (i == end()) tie(i, ignore) = emplace(k, mapped_type());
  return i->second;
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto unordered_map<Key, T, Hash, Pred, A>::operator[](key_type&& k) ->
    mapped_type& {
  iterator i = find(k);
  if (i == end()) tie(i, ignore) = emplace(move(k), mapped_type());
  return i->second;
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto unordered_map<Key, T, Hash, Pred, A>::at(const key_type& k) ->
    mapped_type& {
  iterator i = find(k);
  if (i == end()) throw out_of_range("unordered_map::at");
  return i->second;
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto unordered_map<Key, T, Hash, Pred, A>::at(const key_type& k) const ->
    const mapped_type& {
  const_iterator i = find(k);
  if (i == end()) throw out_of_range("unordered_map::at");
  return i->second;
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto unordered_map<Key, T, Hash, Pred, A>::bucket_count() const noexcept ->
    size_type {
  return data_.bucket_count();
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto unordered_map<Key, T, Hash, Pred, A>::max_bucket_count() const noexcept ->
    size_type {
  return data_.max_bucket_count();
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto unordered_map<Key, T, Hash, Pred, A>::bucket_size(size_type idx)
    const noexcept -> size_type {
  return distance(begin(idx), end(idx));
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto unordered_map<Key, T, Hash, Pred, A>::bucket(const key_type& k) const ->
    size_type {
  return data_.bucket(k);
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto unordered_map<Key, T, Hash, Pred, A>::begin(size_type idx) noexcept ->
    local_iterator {
  return data_.begin(idx);
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto unordered_map<Key, T, Hash, Pred, A>::begin(size_type idx)
    const noexcept -> const_local_iterator {
  return data_.begin(idx);
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto unordered_map<Key, T, Hash, Pred, A>::end(size_type idx) noexcept ->
    local_iterator {
  return data_.end(idx);
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto unordered_map<Key, T, Hash, Pred, A>::end(size_type idx) const noexcept ->
    const_local_iterator {
  return data_.end(idx);
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto unordered_map<Key, T, Hash, Pred, A>::cbegin(size_type idx)
    const noexcept -> const_local_iterator {
  return data_.cbegin(idx);
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto unordered_map<Key, T, Hash, Pred, A>::cend(size_type idx)
    const noexcept -> const_local_iterator {
  return data_.cend(idx);
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto unordered_map<Key, T, Hash, Pred, A>::load_factor() const noexcept ->
    float {
  return data_.load_factor();
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto unordered_map<Key, T, Hash, Pred, A>::max_load_factor() const noexcept ->
    float {
  return data_.max_load_factor();
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto unordered_map<Key, T, Hash, Pred, A>::max_load_factor(float lf) -> void {
  return data_.max_load_factor(lf);
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto unordered_map<Key, T, Hash, Pred, A>::rehash(size_type n) -> void {
  data_.rehash(n);
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto unordered_map<Key, T, Hash, Pred, A>::reserve(size_type n) -> void {
  data_.reserve(n);
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto swap(unordered_map<Key, T, Hash, Pred, A>& x,
          unordered_map<Key, T, Hash, Pred, A>& y) -> void {
  x.swap(y);
}

template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto operator==(const unordered_map<Key, T, Hash, Pred, A>& x,
                const unordered_map<Key, T, Hash, Pred, A>& y) -> bool {
  return x.data_ == y.data_;
}
template<typename Key, typename T, typename Hash, typename Pred, typename A>
auto operator!=(const unordered_map<Key, T, Hash, Pred, A>& x,
                const unordered_map<Key, T, Hash, Pred, A>& y) -> bool {
  return !(x == y);
}


_namespace_end(std)

#endif /* _UNORDERED_MAP_INL_H_ */
