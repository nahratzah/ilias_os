#include <ilias/linked_unordered_set.h>
#include <algorithm>
#include <abi/ext/log2.h>

_namespace_begin(ilias)
namespace impl {


uint64_t ceil_uls_(long double f, uint64_t max) noexcept {
  if (f >= static_cast<long double>(max)) return max;

  uint64_t truncated = static_cast<uint64_t>(f);
  if (static_cast<long double>(truncated) < f) ++truncated;
  return _namespace(std)::min(truncated, max);
}


auto basic_lu_set_algorithms::find_predecessor(
    const iterator* begin, const iterator* end,
    iterator v, size_t b, const basic_linked_forward_list& list)
    noexcept -> iterator {
  assert(b <= size_t(end - begin));

  iterator search = (b == 0 ? list.before_begin() : begin[b - 1U]);
  while (next(search) != v) ++search;
  return search;
}

auto basic_lu_set_algorithms::update_on_link(iterator* begin, iterator* end,
                                             iterator i_pred, size_t b)
    noexcept -> void {
  using _namespace(std)::next;

  assert(b <= size_t(end - begin));

  iterator i = next(i_pred);
  for (iterator* b_iter = begin + b;
       b_iter != end && *b_iter == i_pred;
       ++b_iter)
    *b_iter = i;
}

auto basic_lu_set_algorithms::update_on_unlink(iterator* begin, iterator* end,
                                               iterator i_pred, size_t b)
    noexcept -> void {
  using _namespace(std)::next;

  assert(b <= size_t(end - begin));

  iterator i = next(i_pred);
  for (iterator* b_iter = begin + b;
       b_iter != end && *b_iter == i;
       ++b_iter)
    *b_iter = i_pred;
}

/*
 * Simple hash function, attempts to update hash after it has been shrunk.
 * This function attempts to take advantage of masking to avoid recalculating
 * the hash keys and move entire buckets at a time.
 *
 * Since this does not recalculate the hashes,
 * it can only operate on clean divisions (old_sz % new_sz == 0).
 * Returns true on success.
 */
auto basic_lu_set_algorithms::rehash_shrink(iterator* begin,
                                            iterator* end,
                                            iterator* old_end,
                                            basic_linked_forward_list& list)
    noexcept -> bool {
  using _namespace(std)::next;

  /* Check if the code will work for this case. */
  if (end >= old_end) return false;
  if (begin == end) return false;
  const size_t old_mod = size_t(old_end - begin) + 1U;
  const size_t new_mod = size_t(end - begin) + 1U;
  if (old_mod % new_mod != 0) return false;

  iterator* in = end;
  size_t b = 0;  // Destination bucket.

  /* Move all in [end..old_end) but the last bucket. */
  while (next(in) != old_end) {
    iterator sp_pred = *in;
    iterator sp_last = *next(in);

    /* Inform successive buckets that sp_last will no longer be
     * their (before) start position. */
    *next(in) = sp_pred;
    for (iterator* i = next(in, 2); i != old_end && *i == sp_last; ++i)
      *i = sp_pred;

    /* Move to modulo bucket. */
    rehash_splice_operation_(begin, old_end, list, b, sp_pred, sp_last);

    /* Prepare for next iteration. */
    ++in;
    ++b;
    b %= new_mod;
  }

  /* Move the last bucket. */
  iterator sp_pred = *in;
  if (sp_pred != list.end()) {
    /* Find predecessor of last element;
     * since the bucket set doesn't provide this, we have to search
     * for it. */
    iterator sp_last = sp_pred;
    while (next(sp_last) != list.end()) ++sp_last;

    // No successive buckets to update.

    /* Move to modulo bucket. */
    rehash_splice_operation_(begin, old_end, list, b, sp_pred, sp_last);
  }

  return true;  // Done
}

auto basic_lu_set_algorithms::rehash_splice_operation_(
    iterator* begin, iterator* end,
    basic_linked_forward_list& list,
    size_t b, iterator sp_pred, iterator sp_last) noexcept -> void {
  using ptr_iterator = basic_linked_forward_list::ptr_iterator;

  assert(b <= size_t(end - begin));
  iterator pos = (b == 0 ? list.before_begin() : begin[b - 1U]);

  /* Update buckets.
   * Since we splice at the head of the bucket (see splice operation below)
   * we need to update the start position of successive buckets only. */
  for (iterator* i = begin + b;
       i != end && *i == pos;
       ++i)
    *i = sp_last;

  /* Actual splicing, range is spliced at the head of the bucket. */
  basic_linked_forward_list::splice(
      ptr_iterator(pos, basic_linked_forward_list::from_before_iter),
      ptr_iterator(sp_pred, basic_linked_forward_list::from_before_iter),
      ptr_iterator(sp_last, basic_linked_forward_list::from_before_iter));
}

auto basic_lu_set_algorithms::on_move(iterator* begin, iterator* end,
                                      basic_linked_forward_list& list,
                                      basic_linked_forward_list& rhs_list)
    noexcept -> void {
  while (begin != end && *begin == rhs_list.before_begin())
    *begin++ = list.before_begin();
}


} /* namespace ilias::impl */
_namespace_end(ilias)
