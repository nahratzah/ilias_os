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
    iterator v, size_t b, iterator before_begin)
    noexcept -> iterator {
  assert(b == 0 || b < size_t(end - begin));

  /* Find position at which to start our search for predecessor. */
  iterator search = before_begin;
  if (begin != end) {
    for (const iterator* i = begin + b;; --i) {
      if (*i != v) {
        search = *i;
        break;  // GUARD: search start position found
      }

      if (i == begin) {
        assert(next(search) == v);
        return search;  // v is the very first element.
      }
    }
  }

  /* Step forward until we find search preceding v. */
  while (next(search) != v) ++search;
  return search;
}

auto basic_lu_set_algorithms::update_on_link(iterator* begin, iterator* end,
                                             iterator i, size_t b)
    noexcept -> void {
  using _namespace(std)::next;

  assert(b == 0 || b < size_t(end - begin));
  if (begin == end) return;

  iterator s = next(i);
  iterator* b_iter = begin + b;
  while (*b_iter == s) {
    *b_iter = i;
    if (b_iter == begin) break;  // GUARD: don't cross begin of array
    --b_iter;
  }
}

auto basic_lu_set_algorithms::update_on_unlink(iterator* begin, iterator* end,
                                               iterator i, size_t b)
    noexcept -> void {
  using _namespace(std)::next;

  assert(b == 0 || b < size_t(end - begin));
  if (begin == end) return;

  iterator s = next(i);
  iterator* b_iter = begin + b;
  while (*b_iter == i) {
    *b_iter = s;
    if (b_iter == begin) break;  // GUARD: don't cross begin of array
    --b_iter;
  }
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
                                            iterator before_begin,
                                            iterator lst_end) noexcept ->
    bool {
  using _namespace(std)::next;

  /* Check if the code will work for this case. */
  if (end >= old_end) return false;
  if (begin == end) return false;
  if ((old_end - begin) % (end - begin) != 0) return false;

  /* Find predecessor of splice range.
   * Since the ranges are unlinked from this position and the splicing
   * is done in list order, sp_pred will always be the predecessor of the
   * splice range. */
  const iterator sp_pred = find_predecessor(begin, old_end,
                                            *end, end - begin, before_begin);

  for (auto in = end, out = begin;
       in != old_end;
       ++in, out = (next(out) == end ? begin : next(out))) {
    /* Calculate splice range. */
    iterator* in_succ = next(in);
    iterator sp_first = *in;
    iterator sp_end = (in_succ == old_end ? lst_end : *in_succ);
    assert(next(sp_pred) == sp_first);
    if (sp_first == sp_end) continue;  // Empty bucket.

    /* Calculate splice position. */
    iterator succ = out[1];
    iterator pos = find_predecessor(begin, end, succ, out - begin,
                                    before_begin);

    /* Splice elements. */
    basic_linked_forward_list::splice_after(pos, sp_pred, sp_end);

    /* Update buckets. */
    for (iterator* out_i = out; *out_i == succ; --out_i) {
      *out_i = sp_first;  // Empty destination.
      if (out_i == begin) break;  // GUARD
    }
    for (iterator* in_i = in; *in_i == sp_first; --in_i) {
      *in_i = sp_end;
      if (in_i == begin) break;  // GUARD
    }
  }
  return true;
}

auto basic_lu_set_algorithms::rehash_splice_operation_(
    iterator* begin, iterator* end,
    iterator /*before_begin*/, iterator lst_end,
    size_t b, iterator sp_pred, iterator sp_last,
    pred_array_type& pred_array, iterator bb_hint)
    noexcept -> void {
  using flist = basic_linked_forward_list;
  using ptr_iterator = basic_linked_forward_list::ptr_iterator;

  /* Save first element in the splice range (for bucket update). */
  const iterator sp = next(sp_pred);

  /* Look up where to splice:
   * succ -- successor of splice position,
   * pos  -- predecessor of splice position. */
  iterator succ;
  iterator pos;
  if (_predict_true(pred_array)) {
    pos = pred_array[b];
    succ = next(pos);
  } else {
    succ = (begin + b + 1U == end ? lst_end : begin[b + 1U]);
    pos = find_predecessor(begin, end, succ, b, bb_hint);
  }

  /* Splice data into the correct position. */
  flist::splice(ptr_iterator(pos, basic_linked_forward_list::from_before_iter),
                ptr_iterator(sp_pred,
                             basic_linked_forward_list::from_before_iter),
                ptr_iterator(sp_last,
                             basic_linked_forward_list::from_before_iter));

  /* Update buckets to reflect new state. */
  iterator* b_iter = begin + b;
  while (*b_iter == succ) {
    *b_iter = sp;
    if (b_iter == begin) break;
    --b_iter;
  }

  /* Update predecessor array. */
  if (_predict_true(pred_array)) {
    for (auto pa_i = pred_array.begin() + b;
         pa_i != pred_array.end() && *pa_i == pos;
         ++pa_i)
      *pa_i = sp_last;
  }
}

auto basic_lu_set_algorithms::on_move(iterator* begin, iterator* end,
                                      iterator rhs_end, iterator self_end)
    noexcept -> void {
  using _namespace(std)::prev;

  while (end != begin && *prev(end) == rhs_end)
    *--end = self_end;
}


} /* namespace ilias::impl */
_namespace_end(ilias)
