#ifndef _ILIAS_LINKED_UNORDERED_SET_INL_H_
#define _ILIAS_LINKED_UNORDERED_SET_INL_H_

#include <ilias/linked_unordered_set.h>
#include <new>
#include <limits>
#include <abi/misc_int.h>
#include <utility>
#include <algorithm>
#include <functional>

_namespace_begin(ilias)


template<typename T, class Tag, typename Hash, typename Pred, typename A>
linked_unordered_set<T, Tag, Hash, Pred, A>::linked_unordered_set(
    bucket_size_type n, const hasher& h, const key_equal& eq,
    const allocator_type& a)
: params_(h, eq, bucket_set(a), 1.0f)
{
  using _namespace(std)::get;

  if (n < 0) n = 0;
  auto& buckets_ = get<2>(params_);
  buckets_.resize(n, end());
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
linked_unordered_set<T, Tag, Hash, Pred, A>::linked_unordered_set(
    const allocator_type& a)
: linked_unordered_set(0, hasher(), key_equal(), a)
{}

/*
 * Move constructor.
 *
 * Leaves rhs into a semi-valid (read: unusable) state.
 * Only operations on rhs that will work are those not using any buckets.
 */
template<typename T, class Tag, typename Hash, typename Pred, typename A>
linked_unordered_set<T, Tag, Hash, Pred, A>::linked_unordered_set(
    linked_unordered_set&& rhs) noexcept
: list_(_namespace(std)::move(rhs.list_)),
  params_(_namespace(std)::move(rhs.params_)),
  size_(_namespace(std)::exchange(rhs.size_, 0))
{
  using _namespace(std)::get;

  auto& buckets_ = get<2>(params_);
  for (auto i = buckets_.rbegin();
       i != buckets_.rend() && *i == rhs.list_.end();
       ++i)
    *i = list_.end();
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::operator=(
    linked_unordered_set&& rhs) noexcept -> linked_unordered_set& {
  linked_unordered_set(_namespace(std)::move(rhs)).swap(*this);
  return *this;
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::link(pointer p,
                                                       bool allow_dup,
                                                       bool rehash_fail) ->
    _namespace(std)::pair<iterator, bool> {
  using abi::umul_overflow;
  using _namespace(std)::next;
  using _namespace(std)::bad_alloc;
  using _namespace(std)::get;

  assert(p != nullptr);
  auto& key_eq_ = get<1>(params_);
  auto& buckets_ = get<2>(params_);

  /* Resize the hash buckets, prior to insertion. */
  if (_predict_false((static_cast<long double>(size() + 1U) /
                      static_cast<long double>(buckets_.size())) <
                     max_load_factor())) {
    bool fail = false;
    try {
      /* Calculate new number of hash buckets, but beware of overflow. */
      typename bucket_set::size_type new_sz;
      fail = umul_overflow(buckets_.size(), 2U, &new_sz);

      if (_predict_true(!fail))  // if (new_sz did not overflow) ...
        rehash(new_sz);  // May throw.
    } catch (const bad_alloc&) {
      fail = true;  // Handled below.
    }

    /* Try minimum resize operation (slower per insertion). */
    if (_predict_false(fail)) {
      try {
        reserve(size() + 1U);  // May throw.
      } catch (const bad_alloc&) {
        if (!rehash_fail) throw;
      }
    }
  }

  /* Handle case for no buckets. */
  if (_predict_false(buckets_.empty())) {
    if (!allow_dup) {
      for (auto i = begin(); i != end(); ++i) {
        if (key_eq_(*i, *p))
          return { i, false };
      }
    }

    ++size_;
    return { list_.link(list_.end(), p), true };
  }

  /* Lookup bucket and insertion position. */
  bucket_size_type idx = bucket(*p);
  const auto bucket_begin = begin(idx);
  const auto bucket_end = end(idx);
  iterator insert_succ;

  /* Prevent duplicate insertion. */
  if (!allow_dup) {
    for (auto i = bucket_begin; i != bucket_end; ++i) {
      if (key_eq_(*i, *p))
        return { i, false };
    }

    insert_succ = bucket_end;
  } else {
    insert_succ = upper_bound(*p);
  }

  /*
   * No exceptions past this point.
   */
  auto op = [&]() noexcept -> _namespace(std)::pair<iterator, bool> {
              /* Link element into bucket set. */
              typename bucket_set::iterator bucket = buckets_.begin() + idx;
              const auto p_iter = list_.link(insert_succ, p);
              ++size_;

              /* Update iterators into other buckets. */
              for (auto i =
                       typename bucket_set::reverse_iterator{ next(bucket) };
                   i != buckets_.rend() && *i == insert_succ;
                   ++i)
                *i = p_iter;
              return { p_iter, true };
  };
  return op();
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::unlink(const_iterator p_arg)
    noexcept -> pointer {
  using _namespace(std)::next;
  using _namespace(std)::get;

  if (_predict_false(bucket_count() == 0)) {
    --size_;
    return list_.unlink(p_arg);
  }

  bucket_size_type idx = bucket(*p_arg);
  auto& buckets_ = get<2>(params_);
  const auto bucket_end = end(idx);
  const auto bucket_begin = begin(idx);
  const auto bucket = buckets_.begin() + idx;
  auto p = list_type::nonconst_iterator(p_arg);

  /* Bucket may not be empty. */
  assert_msg(bucket_end != bucket_begin,
             "unlink called for element in empty bucket "
             "(i.e. it is not in this list)");

  /* Remove p from all bucket positions. */
  if (bucket_begin == p) {
    assert(*bucket == p);
    const iterator p_succ = next(p);
    for (auto i = bucket; *i == p; --i) {
      *i = p_succ;
      if (i == buckets_.begin()) break;  // GUARD
    }
  }

  /* Remove p from list. */
  --size_;
  return list_.unlink(p_arg);
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::unlink(const_pointer p)
    noexcept -> pointer {
  using _namespace(std)::next;
  using _namespace(std)::get;

  if (_predict_false(bucket_count() == 0)) {
    --size_;
    return list_.unlink(p);
  }

  bucket_size_type idx = bucket(*p);
  auto& buckets_ = get<2>(params_);
  const auto bucket_end = end(idx);
  const auto bucket_begin = begin(idx);
  const auto bucket = buckets_.begin() + idx;

  /* Bucket may not be empty. */
  assert_msg(bucket_end != bucket_begin,
             "unlink called for element in empty bucket "
             "(i.e. it is not in this list)");

  /* Remove p from all bucket positions. */
  if (&*bucket_begin == p) {
    assert(&**bucket == p);
    const iterator p_succ = next(bucket_begin);
    for (auto i = bucket; *i == p; --i) {
      *i = p_succ;
      if (i == buckets_.begin()) break;  // GUARD
    }
  }

  /* Remove p from list. */
  --size_;
  return list_.unlink(p);
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::unlink_all()
    noexcept -> void {
  unlink_all([](pointer) -> void {});
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
template<typename Visitor>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::unlink_all(Visitor visitor)
    noexcept -> void {
  using _namespace(std)::get;
  using _namespace(std)::fill;

  /* Clear hash buckets. */
  auto& buckets_ = get<2>(params_);
  fill(buckets_.begin(), buckets_.end(), end());

  /* Unlink all elements. */
  while (!list_.empty())
    visitor(list_.unlink_front());
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::bucket_count()
    const noexcept -> bucket_size_type {
  using _namespace(std)::get;

  auto& buckets_ = get<2>(params_);
  return buckets_.size();
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::max_bucket_count()
    const noexcept -> bucket_size_type {
  using _namespace(std)::get;

  auto& buckets_ = get<2>(params_);
  auto max = _namespace(std)::numeric_limits<bucket_size_type>::max();
  auto sz = buckets_.max_size();
  return (sz < max ? sz : max);
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::empty()
    const noexcept -> bool {
  return list_.empty();
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::size()
    const noexcept -> size_type {
  return size_;
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::max_size()
    const noexcept -> size_type {
  return _namespace(std)::numeric_limits<size_type>::max();
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::load_factor()
    const noexcept -> float {
  using _namespace(std)::max;

  return static_cast<long double>(size()) /
         static_cast<long double>(max(bucket_count(), bucket_size_type(1)));
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::max_load_factor()
    const noexcept -> float {
  using _namespace(std)::get;

  auto& load_factor_ = get<3>(params_);
  return load_factor_;
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::max_load_factor(float lf) ->
    void {
  using impl::ceil_uls_;
  using _namespace(std)::get;
  using _namespace(std)::numeric_limits;

  assert(lf > 0.0f);
  auto& load_factor_ = get<3>(params_);
  auto& buckets_ = get<2>(params_);
  const auto n = ceil_uls_(static_cast<long double>(size()) / lf,
                           numeric_limits<bucket_size_type>::max());

  if (!empty() && buckets_.size() < n) rehash(n);
  load_factor_ = lf;
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::rehash(
    bucket_size_type n) -> void {
  using impl::ceil_uls_;
  using _namespace(std)::next;
  using _namespace(std)::get;
  using _namespace(std)::max;
  using _namespace(std)::numeric_limits;

  auto& buckets_ = get<2>(params_);
  auto buckets_for_lf = ceil_uls_(size() / max_load_factor(),
                                  numeric_limits<bucket_size_type>::max());
  if (buckets_for_lf < 1) buckets_for_lf = 1;
  if (n < buckets_for_lf) n = buckets_for_lf;
  buckets_.reserve(n);  // May throw.

  /*
   * No exceptions beyond this point.
   */
  auto op = [&]() noexcept {
              buckets_.clear();
              buckets_.resize(n, this->list_.end());  // Won't throw
              for (iterator i_next, i = list_.begin();
                   i != buckets_.front();
                   i = i_next) {
                i_next = next(i);

                const auto idx = bucket(*i);
                const auto old_idx = buckets_[idx];
                this->list_.splice(old_idx, i, i_next);
                for (auto update = buckets_.begin(),
                          update_end = buckets_.begin() + idx;
                     update != update_end;
                     ++update)
                  if (*update == old_idx) *update = i;
              }
            };
  op();
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::reserve(
    bucket_size_type n) -> void {
  using impl::ceil_uls_;
  using _namespace(std)::numeric_limits;

  rehash(ceil_uls_(n / max_load_factor(),
                   numeric_limits<bucket_size_type>::max()));
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
template<typename K>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::bucket(const K& k)
    const -> bucket_size_type {
  using _namespace(std)::get;

  if (_predict_false(bucket_count() == 0)) return 0;
  return get<0>(params_)(k) % bucket_count();
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::hash_function()
    const -> hasher {
  using _namespace(std)::get;

  return get<0>(params_);
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::key_eq()
    const -> key_equal {
  using _namespace(std)::get;

  return get<1>(params_);
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::get_allocator()
    const -> allocator_type {
  using _namespace(std)::get;

  return get<2>(params_).get_allocator();
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::begin() noexcept ->
    iterator {
  return list_.begin();
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::begin() const noexcept ->
    const_iterator {
  return list_.begin();
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::cbegin() const noexcept ->
    const_iterator {
  return begin();
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::end() noexcept ->
    iterator {
  return list_.end();
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::end() const noexcept ->
    const_iterator {
  return list_.end();
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::cend() const noexcept ->
    const_iterator {
  return end();
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::begin(bucket_size_type idx)
    noexcept -> iterator {
  using _namespace(std)::get;

  if (_predict_false(bucket_count() == 0 && idx == 0)) return begin();

  assert(idx >= 0 && idx < bucket_count());

  auto& buckets_ = get<2>(params_);
  return buckets_.at(idx);
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::begin(bucket_size_type idx)
    const noexcept -> const_iterator {
  using _namespace(std)::get;

  if (_predict_false(bucket_count() == 0 && idx == 0)) return begin();

  assert(idx >= 0 && idx < bucket_count());

  auto& buckets_ = get<2>(params_);
  return buckets_.at(idx);
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::cbegin(bucket_size_type idx)
    const noexcept -> const_iterator {
  return begin(idx);
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::end(bucket_size_type idx)
    noexcept -> iterator {
  using _namespace(std)::get;

  if (_predict_false(bucket_count() == 0 && idx == 0)) return end();

  assert(idx >= 0 && idx < bucket_count());

  auto& buckets_ = get<2>(params_);
  if (idx == buckets_.size() - 1) return list_.end();
  return buckets_.at(idx + 1);
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::end(bucket_size_type idx)
    const noexcept -> const_iterator {
  using _namespace(std)::get;

  if (_predict_false(bucket_count() == 0 && idx == 0)) return end();

  assert(idx >= 0 && idx < bucket_count());

  auto& buckets_ = get<2>(params_);
  if (idx == buckets_.size() - 1) return list_.end();
  return buckets_.at(idx + 1);
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::cend(bucket_size_type idx)
    const noexcept -> const_iterator {
  return end(idx);
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
template<typename K>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::find(const K& k) ->
    iterator {
  using _namespace(std)::get;

  auto& key_eq_ = get<1>(params_);
  iterator b, e;
  if (_predict_false(bucket_count() == 0)) {
    b = begin();
    e = end();
  } else {
    const auto idx = bucket(k);
    b = begin(idx);
    e = end(idx);
  }

  auto i = find_if(b, e, [&k, &key_eq_](const_reference v) {
                           return key_eq_(v, k);
                         });
  return (i == e ? end() : i);
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
template<typename K>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::find(const K& k) const ->
    const_iterator {
  using _namespace(std)::get;

  auto& key_eq_ = get<1>(params_);
  const_iterator b, e;
  if (_predict_false(bucket_count() == 0)) {
    b = begin();
    e = end();
  } else {
    const auto idx = bucket(k);
    b = begin(idx);
    e = end(idx);
  }

  auto i = find_if(b, e, [&k, &key_eq_](const_reference v) {
                           return key_eq_(v, k);
                         });
  return (i == e ? end() : i);
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
template<typename K>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::lower_bound(const K& k) ->
    iterator {
  using _namespace(std)::get;
  using _namespace(std)::find_if;

  auto& key_eq_ = get<1>(params_);
  iterator b, e;
  if (_predict_false(bucket_count() == 0)) {
    b = begin();
    e = end();
  } else {
    const auto idx = bucket(k);
    b = begin(idx);
    e = end(idx);
  }

  return find_if(b, e, [&k, &key_eq_](const_reference v) {
                         return key_eq_(v, k);
                       });
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
template<typename K>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::lower_bound(const K& k)
    const -> const_iterator {
  using _namespace(std)::get;
  using _namespace(std)::find_if;

  auto& key_eq_ = get<1>(params_);
  const_iterator b, e;
  if (_predict_false(bucket_count() == 0)) {
    b = begin();
    e = end();
  } else {
    const auto idx = bucket(k);
    b = begin(idx);
    e = end(idx);
  }

  return find_if(b, e, [&k, &key_eq_](const_reference v) {
                         return key_eq_(v, k);
                       });
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
template<typename K>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::upper_bound(const K& k) ->
    iterator {
  using _namespace(std)::get;
  using _namespace(std)::ref;
  using _namespace(std)::find_if;
  using _namespace(std)::find_if_not;

  auto& key_eq_ = get<1>(params_);
  iterator b, e;
  if (_predict_false(bucket_count() == 0)) {
    b = begin();
    e = end();
  } else {
    const auto idx = bucket(k);
    b = begin(idx);
    e = end(idx);
  }

  auto pred = [&k, &key_eq_](const_reference v) {
                return key_eq_(v, k);
              };
  return find_if_not(find_if(b, e, ref(pred)), e, ref(pred));
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
template<typename K>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::upper_bound(const K& k)
    const -> const_iterator {
  using _namespace(std)::get;
  using _namespace(std)::ref;
  using _namespace(std)::find_if;
  using _namespace(std)::find_if_not;

  auto& key_eq_ = get<1>(params_);
  const_iterator b, e;
  if (_predict_false(bucket_count() == 0)) {
    b = begin();
    e = end();
  } else {
    const auto idx = bucket(k);
    b = begin(idx);
    e = end(idx);
  }

  auto pred = [&k, &key_eq_](const_reference v) {
                return key_eq_(v, k);
              };
  return find_if_not(find_if(b, e, ref(pred)), e, ref(pred));
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
template<typename K>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::equal_range(const K& k) ->
    _namespace(std)::pair<iterator, iterator> {
  using _namespace(std)::get;
  using _namespace(std)::ref;
  using _namespace(std)::find_if;
  using _namespace(std)::find_if_not;

  auto& key_eq_ = get<1>(params_);
  iterator b, e;
  if (_predict_false(bucket_count() == 0)) {
    b = begin();
    e = end();
  } else {
    const auto idx = bucket(k);
    b = begin(idx);
    e = end(idx);
  }

  auto pred = [&k, &key_eq_](const_reference v) {
                return key_eq_(v, k);
              };
  _namespace(std)::pair<iterator, iterator> rv;
  rv.first = find_if(b, e, ref(pred));
  rv.second = find_if_not(rv.first, e, ref(pred));
  return rv;
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
template<typename K>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::equal_range(const K& k)
    const -> _namespace(std)::pair<const_iterator, const_iterator> {
  using _namespace(std)::get;
  using _namespace(std)::ref;
  using _namespace(std)::find_if;
  using _namespace(std)::find_if_not;

  auto& key_eq_ = get<1>(params_);
  const_iterator b, e;
  if (_predict_false(bucket_count() == 0)) {
    b = begin();
    e = end();
  } else {
    const auto idx = bucket(k);
    b = begin(idx);
    e = end(idx);
  }

  auto pred = [&k, &key_eq_](const_reference v) {
                return key_eq_(v, k);
              };
  _namespace(std)::pair<const_iterator, const_iterator> rv;
  rv.first = find_if(b, e, ref(pred));
  rv.second = find_if_not(rv.first, e, ref(pred));
  return rv;
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
template<typename K>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::count(const K& k) const ->
    size_type {
  auto r = equal_range(k);
  return _namespace(std)::distance(r.first, r.second);
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::swap(
    linked_unordered_set& other) noexcept -> void {
  using _namespace(std)::swap;
  using _namespace(std)::get;

  list_.swap(other.list_);
  swap(params_, other.params_);
  swap(size_, other.size_);

  /* Update buckets pointing at other.end() to point at this->end(). */
  for (auto i = get<2>(params_).rbegin();
       i != get<2>(params_).rend() && *i == other.list_.end();
       ++i)
    *i = end();

  /* Update buckets pointing at this->end() to point at other.end(). */
  for (auto i = get<2>(other.params_).rbegin();
       i != get<2>(other.params_).rend() && *i == list_.end();
       ++i)
    *i = other.end();
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::nonconst_iterator(
    const_iterator i) noexcept -> iterator {
  return list_type::nonconst_iterator(i);
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
auto swap(linked_unordered_set<T, Tag, Hash, Pred, A>& x,
          linked_unordered_set<T, Tag, Hash, Pred, A>& y) noexcept -> void {
  x.swap(y);
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
auto operator==(const linked_unordered_set<T, Tag, Hash, Pred, A>& x,
                const linked_unordered_set<T, Tag, Hash, Pred, A>& y) -> bool {
  using iterator =
      typename linked_unordered_set<T, Tag, Hash, Pred, A>::const_iterator;
  using reference =
      typename linked_unordered_set<T, Tag, Hash, Pred, A>::const_reference;
  using size_type =
      typename linked_unordered_set<T, Tag, Hash, Pred, A>::size_type;

  if (x.size() != y.size()) return false;
  auto eq = x.key_eq();
  for (iterator x_b = x.begin(), x_e; x_b != x.end(); x_b = x_e) {
    x_e = _namespace(std)::find_if_not(next(x_b), x.end(),
                                       [&eq, &x_b](reference v) -> bool {
                                         return eq(*x_b, v);
                                       });

    iterator y_b, y_e;
    _namespace(std)::tie(y_b, y_e) = y.equal_range(*x_b);
    if (!_namespace(std)::is_permutation(x_b, x_e, y_b, y_e)) return false;
  }

  return true;
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
auto operator!=(const linked_unordered_set<T, Tag, Hash, Pred, A>& x,
                const linked_unordered_set<T, Tag, Hash, Pred, A>& y) -> bool {
  return !(x == y);
}


_namespace_end(ilias)

#endif /* _ILIAS_LINKED_UNORDERED_SET_INL_H_ */