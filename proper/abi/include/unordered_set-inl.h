#ifndef _UNORDERED_SET_INL_H_
#define _UNORDERED_SET_INL_H_

#include <unordered_set>

_namespace_begin(std)


template<typename Key, typename Hash, typename Pred, typename A>
unordered_set<Key, Hash, Pred, A>::unordered_set(size_type buckets,
                                                 const hasher& h,
                                                 const key_equal& k,
                                                 const allocator_type& a)
: alloc_base(a),
  data_(buckets,
        impl::set_hash_wrapper<Hash, Key, elem>(h),
        impl::set_equal_wrapper<Pred, Key, elem>(k),
        a)
{}

template<typename Key, typename Hash, typename Pred, typename A>
template<typename InputIter>
unordered_set<Key, Hash, Pred, A>::unordered_set(InputIter b, InputIter e,
                                                 size_type buckets,
                                                 const hasher& h,
                                                 const key_equal& k,
                                                 const allocator_type& a)
: unordered_set(buckets, h, k, a)
{
  insert(b, e);
}

template<typename Key, typename Hash, typename Pred, typename A>
unordered_set<Key, Hash, Pred, A>::unordered_set(const unordered_set& other)
: unordered_set(other.bucket_count(), other.hash_function(), other.key_eq(),
                other.get_allocator_for_copy_())
{
  max_load_factor(other.max_load_factor());
  insert(other.begin(), other.end());
}

template<typename Key, typename Hash, typename Pred, typename A>
unordered_set<Key, Hash, Pred, A>::unordered_set(unordered_set&& other)
: alloc_base(move(other)),
  data_(move(other.data_))
{}

template<typename Key, typename Hash, typename Pred, typename A>
unordered_set<Key, Hash, Pred, A>::unordered_set(const allocator_type& a)
: alloc_base(a),
  data_(a)
{}

template<typename Key, typename Hash, typename Pred, typename A>
unordered_set<Key, Hash, Pred, A>::unordered_set(const unordered_set& other,
                                                 const allocator_type& a)
: alloc_base(a),
  data_(0, other.hash_function(), other.key_eq(), a)
{
  max_load_factor(other.max_load_factor());
  insert(other.begin(), other.end());
}

template<typename Key, typename Hash, typename Pred, typename A>
unordered_set<Key, Hash, Pred, A>::unordered_set(unordered_set&& other,
                                                 const allocator_type& a)
: alloc_base(a),
  data_(0, other.hash_function(), other.key_eq(), a)
{
  max_load_factor(other.max_load_factor());
  move(other.begin(), other.end(), inserter(*this, end()));
}

template<typename Key, typename Hash, typename Pred, typename A>
unordered_set<Key, Hash, Pred, A>::unordered_set(
    initializer_list<value_type> il,
    size_type buckets, const hasher& h, const key_equal& k,
    const allocator_type& a)
: unordered_set(il.begin(), il.end(), buckets, h, k, a)
{}

template<typename Key, typename Hash, typename Pred, typename A>
unordered_set<Key, Hash, Pred, A>::~unordered_set() noexcept {
  clear();
}

template<typename Key, typename Hash, typename Pred, typename A>
auto unordered_set<Key, Hash, Pred, A>::operator=(const unordered_set& rhs) ->
    unordered_set& {
  if (&rhs != this) {
    clear();
    insert(rhs.begin(), rhs.end());
  }
  return *this;
}

template<typename Key, typename Hash, typename Pred, typename A>
auto unordered_set<Key, Hash, Pred, A>::operator=(unordered_set&& rhs) ->
    unordered_set& {
  assert(&rhs != this);
  clear();
  if (this->get_allocator() == rhs.get_allocator_())
    data_type(move(rhs.data_)).swap(data_);
  else
    move(rhs.begin(), rhs.end(), inserter(*this, end()));
  return *this;
}

template<typename Key, typename Hash, typename Pred, typename A>
auto unordered_set<Key, Hash, Pred, A>::operator=(
    initializer_list<value_type> il) -> unordered_set& {
  clear();
  insert(il.begin(), il.end());
  return *this;
}

template<typename Key, typename Hash, typename Pred, typename A>
auto unordered_set<Key, Hash, Pred, A>::get_allocator() const ->
    allocator_type {
  return alloc_base::get_allocator();
}

template<typename Key, typename Hash, typename Pred, typename A>
auto unordered_set<Key, Hash, Pred, A>::empty() const noexcept ->
    bool {
  return data_.empty();
}

template<typename Key, typename Hash, typename Pred, typename A>
auto unordered_set<Key, Hash, Pred, A>::size() const noexcept ->
    size_type {
  return data_.size();
}

template<typename Key, typename Hash, typename Pred, typename A>
auto unordered_set<Key, Hash, Pred, A>::max_size() const noexcept ->
    size_type {
  return data_.max_size();
}

template<typename Key, typename Hash, typename Pred, typename A>
auto unordered_set<Key, Hash, Pred, A>::begin() noexcept ->
    iterator {
  return data_.begin();
}

template<typename Key, typename Hash, typename Pred, typename A>
auto unordered_set<Key, Hash, Pred, A>::begin() const noexcept ->
    const_iterator {
  return data_.begin();
}

template<typename Key, typename Hash, typename Pred, typename A>
auto unordered_set<Key, Hash, Pred, A>::end() noexcept ->
    iterator {
  return data_.end();
}

template<typename Key, typename Hash, typename Pred, typename A>
auto unordered_set<Key, Hash, Pred, A>::end() const noexcept ->
    const_iterator {
  return data_.end();
}

template<typename Key, typename Hash, typename Pred, typename A>
auto unordered_set<Key, Hash, Pred, A>::cbegin() const noexcept ->
    const_iterator {
  return begin();
}

template<typename Key, typename Hash, typename Pred, typename A>
auto unordered_set<Key, Hash, Pred, A>::cend() const noexcept ->
    const_iterator {
  return end();
}


_namespace_end(std)

#endif /* _UNORDERED_SET_INL_H_ */
