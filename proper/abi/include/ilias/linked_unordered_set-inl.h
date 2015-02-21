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
namespace impl {

template<typename Pred>
auto basic_lu_set_algorithms::find_predecessor_for_link(
    const iterator* begin, const iterator* end,
    size_t b, Pred pred,
    const basic_linked_forward_list& list) -> iterator {
  assert(b <= size_t(end - begin));

  /* Set up [i..i_end) iterators for this hash bucket.
   * rv is the predecessor of i. */
  iterator rv = (b == 0 ? list.before_begin() : begin[b - 1U]);
  iterator i = next(rv);
  iterator i_end = (b == size_t(end - begin) ? list.end() : next(begin[b]));

  /* Perform search in this hash bucket. */
  while (i != i_end && !pred(*i))
    rv = i++;  // Skip elements until we find an element for which pred holds.
  if (i != i_end) rv = i++;  // Skip element for which we know pred holds.
  while (i != i_end && pred(*i))
    rv = i++;  // Skip elements for which pred holds.
  return rv;  // Predecessor of i.
}

/*
 * Simple rehash operation.
 * Resets the hash buckets and rehashes the entire data set.
 *
 * hasher: Hash function, bound for iterator::value_type*.
 * returns: true, to indicate the hash succeeded.
 */
template<typename Hash>
auto basic_lu_set_algorithms::rehash_completely(
    iterator* begin, iterator* end, Hash hasher,
    basic_linked_forward_list& list) noexcept -> bool {
  if (begin == end) return true;
  const size_t last_bucket = end - begin;
  const size_t mod = last_bucket + 1U;

  /* Reset hash pointers;
   * this effectively hashes all elements to the last bucket. */
  _namespace(std)::fill(begin, end, list.before_begin());

  /* Splice all elements to their correct buckets,
   * skipping any that are to be in the last bucket
   * (which are already in the correct position). */
  iterator front_pred = list.before_begin();
  iterator front;
  bool update_front_pred = true;
  while ((front = next(front_pred)) != list.end()) {
    /* Skip any element that is supposed to be in the last bucket. */
    size_t front_hash;
    while ((front_hash = hasher(*front) % mod) == last_bucket) {
      update_front_pred = false;  // No longer the head of the last bucket.
      ++front_pred;
      ++front;
      if (front == list.end()) return true;  // Done.
    }

    /* Find all elements that share the same bucket. */
    iterator front_end;
    iterator front_last = front;
    for (front_end = next(front);
         front_end != list.end() && hasher(*front_end) % mod == front_hash;
         front_last = front_end++);

    /* Perform splice operation. */
    rehash_splice_operation_(begin, end, list, front_hash,
                             front_pred, front_last);

    /* update_front_pred indicates that the range is at the start
     * of the last bucket.
     * Since the predecessor of that bucket may now have changed, update it. */
    if (update_front_pred)
      front_pred = begin[last_bucket - 1U];
  }

  return true;
}

template<typename Hash>
auto basic_lu_set_algorithms::rehash(iterator* begin, iterator* end,
                                     iterator* old_end, Hash hasher,
                                     basic_linked_forward_list& list)
    noexcept -> void {
  using _namespace(std)::ref;

  const bool succes =
      rehash_shrink(begin, end, old_end, list) ||
      rehash_completely(begin, end, ref(hasher), list);
  assert(succes);
  if (_predict_false(!succes)) for (;;);  // Prevent failure from corrupting
                                          // the program.
}

} /* namespace std::impl */


template<typename A>
basic_linked_unordered_set<A>::basic_linked_unordered_set(
    bucket_size_type n, basic_linked_forward_list& list,
    const allocator_type& a)
: buckets_(_namespace(std)::max(n, bucket_size_type(1)) - 1U, list.end(), a)
{}

template<typename A>
basic_linked_unordered_set<A>::basic_linked_unordered_set(
    const allocator_type& a)
: buckets_(a)
{}

template<typename A>
basic_linked_unordered_set<A>::basic_linked_unordered_set(
    basic_linked_unordered_set<A>&& rhs,
    basic_linked_forward_list& list,
    basic_linked_forward_list&& rhs_list) noexcept
: buckets_(move(rhs.buckets_))
{
  impl::basic_lu_set_algorithms::on_move(buckets_.begin(), buckets_.end(),
                                         list, rhs_list);
}

template<typename A>
auto basic_linked_unordered_set<A>::swap(basic_linked_unordered_set& rhs,
                                         basic_linked_forward_list& list,
                                         basic_linked_forward_list& rhs_list)
    noexcept -> void {
  buckets_.swap(rhs.buckets_);
  impl::basic_lu_set_algorithms::on_move(buckets_.begin(), buckets_.end(),
                                         list, rhs_list);
  impl::basic_lu_set_algorithms::on_move(rhs.buckets_.begin(),
                                         rhs.buckets_.end(),
                                         rhs_list, list);
}

template<typename A>
auto basic_linked_unordered_set<A>::begin_(
    bucket_size_type b, const basic_linked_forward_list& list)
    const noexcept -> iterator {
  assert(b < bucket_count());
  return (b == 0 ? list.begin() : next(buckets_[b - 1U]));
}

template<typename A>
auto basic_linked_unordered_set<A>::end_(
    bucket_size_type b, const basic_linked_forward_list& list)
    const noexcept -> iterator {
  assert(b < bucket_count());
  return (b == buckets_.size() ? list.end() : next(buckets_[b]));
}

/*
 * Find the predecessor to v, which is in bucket b.
 *
 * before_begin is the predecessor of begin(), as supplied by forward_list.
 */
template<typename A>
auto basic_linked_unordered_set<A>::find_predecessor(
    iterator v, bucket_size_type b, const basic_linked_forward_list& list)
    const noexcept -> iterator {
  return impl::basic_lu_set_algorithms::find_predecessor(buckets_.begin(),
                                                         buckets_.end(),
                                                         v, b, list);
}

/*
 * Find the predecessor to e, which will be linked into bucket b.
 *
 * pred is the equality comparator, bound to e.
 * before_begin: the predecessor of begin(), as supplied by forward_list.
 * lst_end: the end() of the list.
 */
template<typename A>
template<typename Pred>
auto basic_linked_unordered_set<A>::find_predecessor_for_link(
    bucket_size_type b, Pred pred,
    const basic_linked_forward_list& list) const -> iterator {
  return impl::basic_lu_set_algorithms::find_predecessor_for_link(
      buckets_.begin(), buckets_.end(),
      b, _namespace(std)::ref(pred),
      list);
}

/*
 * Update buckets after insertion of i.
 *
 * b: the bucket that both inhabit.
 */
template<typename A>
void basic_linked_unordered_set<A>::update_on_link(iterator i_pred,
                                                   bucket_size_type b)
    noexcept {
  impl::basic_lu_set_algorithms::update_on_link(buckets_.begin(),
                                                buckets_.end(),
                                                i_pred, b);
}

/*
 * Update buckets prior to unlinking of i.
 *
 * b: the bucket that both inhabit.
 */
template<typename A>
void basic_linked_unordered_set<A>::update_on_unlink(iterator i_pred,
                                                     bucket_size_type b)
    noexcept {
  impl::basic_lu_set_algorithms::update_on_unlink(buckets_.begin(),
                                                  buckets_.end(),
                                                  i_pred, b);
}

template<typename A>
void basic_linked_unordered_set<A>::update_on_unlink_all(
    const basic_linked_forward_list& list) noexcept {
  _namespace(std)::fill(buckets_.begin(), buckets_.end(), list.before_begin());
}

template<typename A>
template<typename Hash>
void basic_linked_unordered_set<A>::rehash(
    bucket_size_type n, Hash hasher, basic_linked_forward_list& list) {
  using alg = impl::basic_lu_set_algorithms;

  if (n <= 1) {
    buckets_.clear();
    return;
  }

  auto op = [&]() noexcept {
              const auto old_sz = bucket_count();
              if (n > old_sz)
                buckets_.resize(n - 1U, list.end());
              alg::rehash(this->buckets_.begin(),
                          next(this->buckets_.begin(), n - 1U),
                          next(this->buckets_.begin(), old_sz - 1U),
                          ref(hasher), list);
              if (n < old_sz)
                buckets_.resize(n - 1U);
            };

  buckets_.reserve(n - 1U);  // Ensure resize won't throw.
  op();  // Noexcept operation to resize buckets and rehash the list.
}

template<typename A>
template<typename SizeT, typename Hash>
void basic_linked_unordered_set<A>::rehash_grow(
    SizeT new_sz, float max_lf,
    Hash hasher,
    basic_linked_forward_list& list,
    bucket_size_type mul,
    bucket_size_type div) {
  using _namespace(std)::bad_alloc;
  using _namespace(std)::tie;
  using _namespace(std)::numeric_limits;
  using abi::umul_overflow;
  using abi::addc;

  assert(mul > 0 && div > 0);
  const auto min_bcount = impl::ceil_uls_(new_sz / max_lf, max_bucket_count());
  if (min_bcount > numeric_limits<bucket_size_type>::max()) throw bad_alloc();
  if (min_bcount <= bucket_count()) return;

  bucket_size_type bcount;
  {
    /*
     * Calculate:  bcount = (bucket_count() * mul) / div  -- rounded up.
     * Sets fail to true if any step in the calculation overflows.
     *
     * bcount = (bucket_count() * mul) / div
     *        = floor(bucket_count() / div) * mul +
     *          ((bucket_count() % div) * mul / div)
     */
    bool fail = false;

    // first = floor(bucket_count() / div) * mul
    bucket_size_type first;
    fail |= umul_overflow(bucket_count() / div, mul, &first);

    // second = ((bucket_count % div) * mul) / div
    bucket_size_type second;
    fail |= umul_overflow(bucket_count() % div, mul, &second);
    second /= div;

    // bcount = first + second
    bool overflow;
    tie(bcount, overflow) = addc(first, second, bucket_size_type(0));
    fail |= overflow;

    // Use minimum size if we overflow.
    if (_predict_false(fail)) bcount = min_bcount;
  }

  try {
    this->rehash(bcount, ref(hasher), list);
  } catch (const bad_alloc&) {
    if (bcount == min_bcount) throw;
    this->rehash(min_bcount, ref(hasher), list);
  }
}

template<typename A>
template<typename SizeT, typename Hash>
void basic_linked_unordered_set<A>::rehash_shrink(
    SizeT new_sz, float max_lf,
    Hash hasher,
    basic_linked_forward_list& list,
    bucket_size_type mul,
    bucket_size_type div) noexcept {
  using _namespace(std)::bad_alloc;
  using _namespace(std)::numeric_limits;
  using _namespace(std)::tie;
  using abi::umul_overflow;
  using abi::addc;

  assert(mul > 0 && div > 0);
  const auto min_bcount = impl::ceil_uls_(new_sz / max_lf, max_bucket_count());
  if (min_bcount > numeric_limits<bucket_size_type>::max()) return;
  if (min_bcount >= bucket_count()) return;

  bucket_size_type bcount;
  {
    /*
     * Calculate:  bcount = (bucket_count() * mul) / div  -- rounded up.
     * Sets fail to true if any step in the calculation overflows.
     *
     * bcount = (bucket_count() * mul) / div
     *        = floor(bucket_count() / div) * mul +
     *          ((bucket_count() % div) * mul / div)
     */
    bool fail = false;

    // first = floor(bucket_count() / div) * mul
    bucket_size_type first;
    fail |= umul_overflow(bucket_count() / div, mul, &first);

    // second = ((bucket_count % div) * mul) / div
    bucket_size_type second;
    fail |= umul_overflow(bucket_count() % div, mul, &second);
    second /= div;

    // bcount = first + second
    bool overflow;
    tie(bcount, overflow) = addc(first, second, bucket_size_type(0));
    fail |= overflow;

    // Fudge it, if we overflow.
    if (_predict_false(fail)) bcount = bucket_count() / 2;

    // Only resize if bcount will not violate the load factor constraint.
    if (bcount < min_bcount) return;
  }
  if (bcount >= bucket_count()) return;

  this->rehash(bcount, ref(hasher), list);
}

template<typename A>
float basic_linked_unordered_set<A>::load_factor_for_size(size_t n)
    const noexcept {
  return static_cast<long double>(n) /
         static_cast<long double>(bucket_count());
}

template<typename A>
auto basic_linked_unordered_set<A>::bucket_count() const noexcept ->
    bucket_size_type {
  return buckets_.size() + 1U;
}

template<typename A>
auto basic_linked_unordered_set<A>::max_bucket_count() const noexcept ->
    bucket_size_type {
  using _namespace(std)::numeric_limits;

  const auto buckets_rv = buckets_.max_size();
  if (buckets_rv > numeric_limits<bucket_size_type>::max())
    return numeric_limits<bucket_size_type>::max();
  return buckets_rv;
}

template<typename A>
auto basic_linked_unordered_set<A>::get_allocator() const -> allocator_type {
  return buckets_.get_allocator();
}


template<typename T, class Tag, typename Hash, typename Pred, typename A>
linked_unordered_set<T, Tag, Hash, Pred, A>::linked_unordered_set(
    bucket_size_type n, const hasher& h, const key_equal& eq,
    const allocator_type& a)
: list_type(),
  basic_linked_unordered_set<A>(
      n, basic_list_cast(*this), a),
  params_(h, eq, 1.0f)
{}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
linked_unordered_set<T, Tag, Hash, Pred, A>::linked_unordered_set(
    const allocator_type& a)
: list_type(),
  basic_linked_unordered_set<A>(a),
  params_(hasher(), key_equal(), 1.0f)
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
: list_type(move(rhs)),
  basic_linked_unordered_set<A>(
      move(rhs),
      basic_list_cast(*this),
      basic_list_cast(move(rhs))),
  params_(_namespace(std)::move(rhs.params_)),
  size_(_namespace(std)::exchange(rhs.size_, 0))
{}

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
  using _namespace(std)::placeholders::_1;
  using _namespace(std)::bind;
  using _namespace(std)::ref;
  using _namespace(std)::cref;
  using _namespace(std)::bad_alloc;
  using _namespace(std)::make_pair;
  using _namespace(std)::get;
  using _namespace(std)::tie;
  using abi::addc;

  assert(p != nullptr);
  auto& hasher_ = get<0>(params_);
  auto& key_eq_ = get<1>(params_);
  const auto b = bucket(*p);

  /* Find insertion position. */
  auto basic_pred =
      this->basic_linked_unordered_set<A>::find_predecessor_for_link(
          b,
          bind(cref(key_eq_),
               cref(*p),
               bind(&list_type::up_cast_cref_, _1)),
          basic_list_cast(*this));
  auto pred = iterator::from_unsafe_basic_iter(basic_pred);
  assert(pred != this->list_type::end());

  /*
   * Prevent duplicates.
   */
  if (!allow_dup && pred != this->list_type::before_begin() &&
      key_eq_(*pred, *p))
    return make_pair(pred, false);

  /*
   * Grow hash buckets prior to insertion.
   */
  size_type overflow;
  size_type new_sz;
  tie(new_sz, overflow) = addc(size(), size_type(1), size_type(0));
  if (_predict_false(overflow)) throw bad_alloc();
  try {
    this->rehash_grow(
        new_sz, max_load_factor(),
        bind(cref(hasher_), bind(&list_type::up_cast_cref_, _1)),
        basic_list_cast(*this));
  } catch (const bad_alloc&) {
    if (!rehash_fail) throw;
  }

  /* Link element into list. */
  iterator rv = this->list_type::link_after(pred, p);
  ++size_;

  /* Update hash buckets. */
  this->basic_linked_unordered_set<A>::update_on_link(
      pred.get_unsafe_basic_iter(), b);

  return make_pair(rv, true);
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::unlink(const_iterator p)
    noexcept -> pointer {
  using _namespace(std)::get;
  using _namespace(std)::bind;
  using _namespace(std)::placeholders::_1;
  using _namespace(std)::cref;

  const float lf_shrink_mul = 0.75f;
  auto& hasher_ = get<0>(params_);
  const auto b = bucket(*p);

  /* Locate predecessor of p. */
  iterator pred = iterator::from_unsafe_basic_iter(
      this->find_predecessor(p.get_unsafe_basic_iter(), b,
                             basic_list_cast(*this)));

  /* Update buckets. */
  this->update_on_unlink(pred.get_unsafe_basic_iter(), b);

  /* Unlink from list. */
  const pointer rv = this->list_type::unlink_after(pred);
  --size_;

  /* Update number of buckets (never throws). */
  this->rehash_shrink(size(), max_load_factor() * lf_shrink_mul,
                      bind(cref(hasher_), bind(&list_type::up_cast_cref_, _1)),
                      basic_list_cast(*this));

  return rv;
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::unlink(const_pointer p)
    noexcept -> pointer {
  assert(p != nullptr);
  return unlink(list_type::iterator_to(*p));
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
  this->basic_linked_unordered_set<A>::update_on_unlink_all(
      basic_list_cast(*this));

  /* Unlink all elements. */
  while (!empty())
    visitor(this->list_type::unlink_front());
  size_ = 0;
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
  return this->load_factor_for_size(size());
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::max_load_factor()
    const noexcept -> float {
  using _namespace(std)::get;

  auto& load_factor_ = get<2>(params_);
  return load_factor_;
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::max_load_factor(float lf) ->
    void {
  using impl::ceil_uls_;
  using _namespace(std)::get;
  using _namespace(std)::numeric_limits;

  assert(lf > 0.0f);
  auto& load_factor_ = get<2>(params_);
  load_factor_ = lf;
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::rehash(
    bucket_size_type n) -> void {
  using impl::ceil_uls_;
  using _namespace(std)::get;
  using _namespace(std)::numeric_limits;
  using _namespace(std)::bind;
  using _namespace(std)::placeholders::_1;
  using _namespace(std)::cref;

  auto& hasher_ = get<0>(params_);
  auto buckets_for_lf = ceil_uls_(size() / max_load_factor(),
                                  numeric_limits<bucket_size_type>::max());
  if (buckets_for_lf < 1) buckets_for_lf = 1;
  if (n < buckets_for_lf) n = buckets_for_lf;

  this->basic_linked_unordered_set<A>::rehash(
      n, bind(cref(hasher_), bind(&list_type::up_cast_cref_, _1)),
      basic_list_cast(*this));
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::reserve(
    size_type n) -> void {
  using _namespace(std)::get;
  using _namespace(std)::numeric_limits;
  using _namespace(std)::bind;
  using _namespace(std)::placeholders::_1;
  using _namespace(std)::cref;

  auto& hasher_ = get<0>(params_);
  this->basic_linked_unordered_set<A>::rehash_grow(
      n, max_load_factor(),
      bind(cref(hasher_), bind(&list_type::up_cast_cref_, _1)),
      basic_list_cast(*this));
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
auto linked_unordered_set<T, Tag, Hash, Pred, A>::begin(bucket_size_type idx)
    noexcept -> iterator {
  return iterator::from_unsafe_basic_iter(
      this->begin_(idx, basic_list_cast(*this)));
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::begin(bucket_size_type idx)
    const noexcept -> const_iterator {
  return const_iterator::from_unsafe_basic_iter(
      this->begin_(idx, basic_list_cast(*this)));
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::cbegin(bucket_size_type idx)
    const noexcept -> const_iterator {
  return begin(idx);
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::end(bucket_size_type idx)
    noexcept -> iterator {
  return iterator::from_unsafe_basic_iter(
      this->end_(idx, basic_list_cast(*this)));
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::end(bucket_size_type idx)
    const noexcept -> const_iterator {
  return const_iterator::from_unsafe_basic_iter(
      this->end_(idx, basic_list_cast(*this)));
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
  const linked_unordered_set& const_self = *this;
  return nonconst_iterator(const_self.find(k));
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
template<typename K>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::find(const K& k) const ->
    const_iterator {
  using _namespace(std)::get;
  using _namespace(std)::placeholders::_1;
  using _namespace(std)::bind;
  using _namespace(std)::cref;
  using _namespace(std)::find_if;

  auto& key_eq_ = get<1>(params_);
  const auto idx = bucket(k);
  const_iterator b = begin(idx);
  const_iterator e = end(idx);

  auto i = find_if(b, e, bind(cref(key_eq_), cref(k), _1));
  return (i == e ? end() : i);
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
template<typename K>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::lower_bound(const K& k) ->
    iterator {
  const linked_unordered_set& const_self = *this;
  return nonconst_iterator(const_self.lower_bound(k));
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
template<typename K>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::lower_bound(const K& k)
    const -> const_iterator {
  using _namespace(std)::get;
  using _namespace(std)::placeholders::_1;
  using _namespace(std)::bind;
  using _namespace(std)::cref;
  using _namespace(std)::find_if;

  auto& key_eq_ = get<1>(params_);
  const auto idx = bucket(k);
  const_iterator b = begin(idx);
  const_iterator e = end(idx);

  return find_if(b, e, bind(cref(key_eq_), cref(k), _1));
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
template<typename K>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::upper_bound(const K& k) ->
    iterator {
  const linked_unordered_set& const_self = *this;
  return nonconst_iterator(const_self.upper_bound(k));
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
template<typename K>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::upper_bound(const K& k)
    const -> const_iterator {
  using _namespace(std)::get;
  using _namespace(std)::placeholders::_1;
  using _namespace(std)::bind;
  using _namespace(std)::cref;
  using _namespace(std)::find_if;
  using _namespace(std)::find_if_not;

  auto& key_eq_ = get<1>(params_);
  const auto idx = bucket(k);
  const_iterator b = begin(idx);
  const_iterator e = end(idx);

  b = find_if(b, e, bind(cref(key_eq_), cref(k), _1));
  if (b != e) ++b;
  return find_if_not(b, e, bind(cref(key_eq_), cref(k), _1));
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
template<typename K>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::equal_range(const K& k) ->
    _namespace(std)::pair<iterator, iterator> {
  using _namespace(std)::make_pair;

  const linked_unordered_set& const_self = *this;
  auto rv = const_self.equal_range(k);
  return make_pair(nonconst_iterator(rv.first), nonconst_iterator(rv.second));
}

template<typename T, class Tag, typename Hash, typename Pred, typename A>
template<typename K>
auto linked_unordered_set<T, Tag, Hash, Pred, A>::equal_range(const K& k)
    const -> _namespace(std)::pair<const_iterator, const_iterator> {
  using _namespace(std)::get;
  using _namespace(std)::bind;
  using _namespace(std)::placeholders::_1;
  using _namespace(std)::cref;
  using _namespace(std)::find_if;
  using _namespace(std)::find_if_not;
  using _namespace(std)::make_pair;

  auto& key_eq_ = get<1>(params_);
  const auto idx = bucket(k);
  const_iterator b = begin(idx);
  const_iterator e = end(idx);

  b = find_if(b, e, bind(cref(key_eq_), cref(k), _1));
  if (b != e)
    e = find_if_not(next(b), e, bind(cref(key_eq_), cref(k), _1));
  return make_pair(b, e);
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
  if (_predict_false(this == &other)) return;

  using _namespace(std)::swap;

  this->list_type::swap(other);
  this->basic_linked_unordered_set<A>::swap(other,
                                            basic_list_cast(*this),
                                            basic_list_cast(other));
  swap(params_, other.params_);
  swap(size_, other.size_);
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
