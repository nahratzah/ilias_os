#ifndef _ILIAS_LINKED_UNORDERED_SET_INL_H_
#define _ILIAS_LINKED_UNORDERED_SET_INL_H_

#include <ilias/linked_unordered_set.h>
#include <new>
#include <limits>
#include <abi/misc_int.h>
#include <cmath>
#include <utility>

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
        resize(size() + 1U);  // May throw.
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
    return { list_.link_back(p), true };
  }

  /* Lookup bucket and insertion position. */
  bucket_size_type idx = bucket(*p);
  const auto bucket_end = end(idx);
  iterator insert_succ;

  /* Prevent duplicate insertion. */
  if (!allow_dup) {
    for (auto i = begin(idx); i != bucket_end; ++i) {
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
              for (typename bucket_set::reverse_iterator i = bucket;
                   i != buckets_.rend() && key_eq_(*i, insert_succ);
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
  using _namespace(std)::prev;
  using _namespace(std)::get;

  if (_predict_false(bucket_count() == 0)) {
    --size_;
    return list_.unlink(p_arg);
  }

  bucket_size_type idx = bucket(*p_arg);
  auto& key_eq_ = get<1>(params_);
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
  if (bucket_end == p) {
    const iterator p_pred = prev(p);
    for (auto i = next(bucket); i != buckets_.end() && *i == p; ++i)
      *i = p_pred;
  }

  /* Remove p from list. */
  --size_;
  return list_.unlink(p_arg);
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::unlink(const_pointer p)
    noexcept -> pointer {
  using _namespace(std)::next;
  using _namespace(std)::prev;
  using _namespace(std)::get;

  if (_predict_false(bucket_count() == 0)) {
    --size_;
    return list_.unlink(p);
  }

  bucket_size_type idx = bucket(*p);
  auto& key_eq_ = get<1>(params_);
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
  } else if (&*bucket_end == p) {
    const iterator p_pred = prev(bucket_end);
    for (auto i = next(bucket); i != buckets_.end() && *i == p; ++i)
      *i = p_pred;
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
  return static_cast<long double>(size()) /
         static_cast<long double>(max(max_size(), 1));
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
  using _namespace(std)::ceil;
  using _namespace(std)::get;
  using _namespace(std)::numeric_limits;

  assert(lf > 0.0f);
  auto& load_factor_ = get<3>(params_);
  auto& buckets_ = get<2>(params_);
  const auto n = ceil(static_cast<long double>(size()) / lf);
  if (lf <= 0 || n > numeric_limits<bucket_size_type>::max()) {
    throw _namespace(std)::invalid_argument("ilias::linked_unordered_set: "
                                            "invalid load factor");
  }

  if (!empty() && buckets_.size() < n) rehash(n);
  load_factor_ = lf;
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::rehash(
    bucket_size_type n) -> void {
  using _namespace(std)::ceil;
  using _namespace(std)::next;
  using _namespace(std)::get;

  auto& buckets_ = get<2>(params_);
  const auto buckets_for_lf = max(ceil(size() / max_load_factor()), 1);
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
  using _namespace(std)::ceil;

  rehash(ceil(n / max_load_factor()));
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
auto swap(linked_unordered_set<T, Tag, Hash, Pred, A>& x,
          linked_unordered_set<T, Tag, Hash, Pred, A>& y) noexcept -> void {
  x.swap(y);
}


_namespace_end(ilias)

#endif /* _ILIAS_LINKED_UNORDERED_SET_INL_H_ */
