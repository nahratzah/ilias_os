#ifndef _ALGORITHM_INL_H_
#define _ALGORITHM_INL_H_

#include <algorithm>
#include <stdimpl/heap_array.h>
#include <stdimpl/heap_support.h>

_namespace_begin(std)


template<typename InputIterator, class Predicate>
bool all_of(InputIterator b, InputIterator e, Predicate predicate) {
  while (b != e && predicate(*b)) ++b;
  return b == e;
}

template<typename InputIterator, class Predicate>
bool any_of(InputIterator b, InputIterator e, Predicate predicate) {
  return !none_of(b, e, ref(predicate));
}

template<typename InputIterator, class Predicate>
bool none_of(InputIterator b, InputIterator e, Predicate predicate) {
  using placeholders::_1;
  return all_of(b, e,
                bind(logical_not<void>(), bind(ref(predicate), _1)));
}


template<typename InputIterator, class Function>
Function for_each(InputIterator b, InputIterator e, Function fun) {
  while (b != e) {
    fun(*b);
    ++b;
  }
  return fun;
}

template<typename InputIterator, typename T>
InputIterator find(InputIterator b, InputIterator e, const T& v) {
  using placeholders::_1;

  return find_if(b, e, bind(equal_to<void>(), _1, ref(v)));
}

template<typename InputIterator, typename Predicate>
InputIterator find_if(InputIterator b, InputIterator e, Predicate predicate) {
  while (b != e && !predicate(*b)) ++b;
  return b;
}

template<typename InputIterator, typename Predicate>
InputIterator find_if_not(InputIterator b, InputIterator e,
                          Predicate predicate) {
  using placeholders::_1;
  return find_if(b, e,
                 bind(logical_not<void>(), bind(ref(predicate), _1)));
}

template<typename ForwardIterator1, typename ForwardIterator2>
ForwardIterator1 find_end(ForwardIterator1 haystack_b,
                          ForwardIterator1 haystack_e,
                          ForwardIterator2 needle_b,
                          ForwardIterator2 needle_e) {
  return find_end(haystack_b, haystack_e,
                  needle_b, needle_e, equal_to<void>());
}

template<typename ForwardIterator1, typename ForwardIterator2,
         typename BinaryPredicate>
ForwardIterator1 find_end(ForwardIterator1 haystack_b,
                          ForwardIterator1 haystack_e,
                          ForwardIterator2 needle_b,
                          ForwardIterator2 needle_e,
                          BinaryPredicate predicate) {
  ForwardIterator1 match = haystack_e;

  while (haystack_b != haystack_e) {
    ForwardIterator1 haystack_i = haystack_b;
    ForwardIterator2 needle_i = needle_b;

    while (needle_i != needle_e && haystack_i != haystack_e &&
           predicate(*haystack_i, *needle_i)) {
      ++needle_i;
      ++needle_e;
    }
    if (needle_i == needle_e) match = haystack_b;
    if (haystack_i == haystack_e) break;
    ++haystack_b;
  }
  return match;
}

template<typename InputIterator, typename ForwardIterator>
InputIterator find_first_of(InputIterator haystack_b,
                            InputIterator haystack_e,
                            ForwardIterator needles_b,
                            ForwardIterator needles_e) {
  return find_first_of(haystack_b, haystack_e,
                       needles_b, needles_e,
                       equal_to<void>());
}

template<typename InputIterator, typename ForwardIterator,
         typename BinaryPredicate>
InputIterator find_first_of(InputIterator haystack_b,
                            InputIterator haystack_e,
                            ForwardIterator needles_b,
                            ForwardIterator needles_e,
                            BinaryPredicate predicate) {
  using placeholders::_1;

  while (haystack_b != haystack_e) {
    if (find_if(needles_b, needles_e,
                bind(ref(predicate), _1, ref(*haystack_b))) != needles_e)
      return haystack_b;
    ++haystack_b;
  }
  return haystack_b;
}

template<typename ForwardIterator>
ForwardIterator adjecent_find(ForwardIterator b, ForwardIterator e) {
  return adjecent_find(b, e, equal_to<void>());
}

template<typename ForwardIterator, typename BinaryPredicate>
ForwardIterator adjecent_find(ForwardIterator b, ForwardIterator e,
                              BinaryPredicate predicate) {
  if (b == e) return e;
  ForwardIterator b_succ = next(b);

  while (b_succ != e) {
    if (predicate(*b, *b_succ)) return b;

    /* ++iterators... */
    b = b_succ;
    ++b_succ;
  }
  return e;
}

template<typename InputIterator, typename T>
typename iterator_traits<InputIterator>::difference_type count(
    InputIterator b, InputIterator e, const T& v) {
  using placeholders::_1;

  return count_if(b, e, bind(equal_to<void>(), _1, ref(v)));
}

template<typename InputIterator, typename Predicate>
typename iterator_traits<InputIterator>::difference_type count_if(
    InputIterator b, InputIterator e, Predicate predicate) {
  typename iterator_traits<InputIterator>::difference_type rv = 0;

  while (b != e) {
    if (predicate(*b)) ++rv;
    ++b;
  }
  return rv;
}

template<typename InputIterator1, typename InputIterator2>
pair<InputIterator1, InputIterator2> mismatch(InputIterator1 b1,
                                              InputIterator1 e1,
                                              InputIterator2 b2) {
  return mismatch(b1, e1, b2, equal_to<void>());
}

template<typename InputIterator1, typename InputIterator2,
         typename BinaryPredicate>
pair<InputIterator1, InputIterator2> mismatch(InputIterator1 b1,
                                              InputIterator1 e1,
                                              InputIterator2 b2,
                                              BinaryPredicate predicate) {
  while (b1 != e1 && predicate(*b1, *b2)) {
    ++b1;
    ++b2;
  }
  return make_pair(b1, b2);
}

template<typename InputIterator1, typename InputIterator2>
pair<InputIterator1, InputIterator2> mismatch(InputIterator1 b1,
                                              InputIterator1 e1,
                                              InputIterator2 b2,
                                              InputIterator2 e2) {
  return mismatch(b1, e1, b2, e2, equal_to<void>());
}

template<typename InputIterator1, typename InputIterator2,
         typename BinaryPredicate>
pair<InputIterator1, InputIterator2> mismatch(InputIterator1 b1,
                                              InputIterator1 e1,
                                              InputIterator2 b2,
                                              InputIterator2 e2,
                                              BinaryPredicate predicate) {
  while (b1 != e1 && b2 != e2 && predicate(*b1, *b2)) {
    ++b1;
    ++b2;
  }
  return make_pair(b1, b2);
}

namespace impl {

template<typename Iter>
auto equal_support__distance(Iter b, Iter e,
                             const random_access_iterator_tag&) ->
    pair<bool, typename iterator_traits<Iter>::difference_type> {
  return make_pair(true, e - b);
}

template<typename Iter, typename... Tail>
auto equal_support__distance(Iter, Iter, Tail&&...) ->
    pair<bool, typename iterator_traits<Iter>::difference_type> {
  return make_pair(false, 0);
}

template<typename Iter1, typename Iter2>
bool unequal_distance(Iter1 b1, Iter1 e1, Iter2 b2, Iter2 e2) {
  auto d1 = equal_support__distance(b1, e1);
  auto d2 = equal_support__distance(b2, e2);

  return d1.first && d2.first && d1.second != d2.second;
}

} /* namespace std::impl */

template<typename InputIterator1, typename InputIterator2>
bool equal(InputIterator1 b1, InputIterator1 e1, InputIterator2 b2) {
  return equal(b1, e1, b2, equal_to<void>());
}

template<typename InputIterator1, typename InputIterator2,
         typename BinaryPredicate>
bool equal(InputIterator1 b1, InputIterator1 e1, InputIterator2 b2,
           BinaryPredicate predicate) {
  while (b1 != e1 && predicate(*b1, *b2)) {
    ++b1;
    ++b2;
  }
  return b1 == e1;
}

template<typename InputIterator1, typename InputIterator2>
bool equal(InputIterator1 b1, InputIterator1 e1,
           InputIterator2 b2, InputIterator2 e2) {
  return equal(b1, e1, b2, e2, equal_to<void>());
}

template<typename InputIterator1, typename InputIterator2,
         typename BinaryPredicate>
bool equal(InputIterator1 b1, InputIterator1 e1,
           InputIterator2 b2, InputIterator2 e2,
           BinaryPredicate predicate) {
  if (impl::unequal_distance(b1, e1, b2, e2)) return false;

  while (b1 != e1 && b2 != e2 && predicate(*b1, *b2)) {
    ++b1;
    ++b2;
  }
  return b1 == e1 && b2 == e2;
}

template<typename ForwardIterator1, typename ForwardIterator2>
bool is_permutation(ForwardIterator1 b1, ForwardIterator1 e1,
                    ForwardIterator2 b2) {
  return is_permutation(b1, e1, b2, equal_to<void>());
}

template<typename ForwardIterator1, typename ForwardIterator2,
         typename BinaryPredicate>
bool is_permutation(ForwardIterator1 b1, ForwardIterator1 e1,
                    ForwardIterator2 b2,
                    BinaryPredicate predicate) {
  return is_permutation(b1, e1, b2, next(b2, distance(b1, e1)),
                        ref(predicate));
}

template<typename ForwardIterator1, typename ForwardIterator2>
bool is_permutation(ForwardIterator1 b1, ForwardIterator1 e1,
                    ForwardIterator2 b2, ForwardIterator2 e2) {
  return is_permutation(b1, e1, b2, e2,
                        equal_to<void>());
}

template<typename ForwardIterator1, typename ForwardIterator2,
         typename BinaryPredicate>
bool is_permutation(ForwardIterator1 b1, ForwardIterator1 e1,
                    ForwardIterator2 b2, ForwardIterator2 e2,
                    BinaryPredicate predicate) {
  using impl::simple_bitset;
  using placeholders::_1;

  if (impl::unequal_distance(b1, e1, b2, e2)) return false;

  /* First eliminate anything that is equal. */
  tie(b1, b2) = mismatch(b1, e1, b2, e2, ref(predicate));
  if (b1 == e1) return b2 == e2;  // Everything is equal.
  if (b2 == e2) return false;  // Unequal length.
  if (impl::unequal_distance(b1, e1, b2, e2)) return false;

  /* Use a collection to find the remaining items. */
  auto count = distance(b1, e1);
  auto s = simple_bitset(count);

  if (s) {
    while (count > 0 && b2 != e2) {
      auto found = b1;
      simple_bitset::size_type off = 0;
      while (found != e1 && (s[off] || !predicate(*found, *b2))) {
        ++found;
        ++off;
      }
      if (found == e1) return false;
      s[off] = true;

      ++b2;
      --count;
    }
    return count == 0 && b2 == e2;
  }

  /*
   * Insufficient memory to create bitset, use count instead.
   * This method requires more invocations of the predicate than
   * the method above;  it is not so bad, unless the predicate is
   * really expensive to run (yielding a big 'c' component in our
   * complexity analysis).
   */
  for (ForwardIterator1 i = b1; i != e1; ++i) {
    auto c2 = count_if(b2, e2, bind(ref(predicate), ref(*i), _1));
    if (c2 == 0) return false;
    auto c1 = count_if(b1, e1, bind(ref(predicate), ref(*i), _1));
    if (c1 != c2) return false;
  }
  return true;
}

template<typename ForwardIterator1, typename ForwardIterator2>
ForwardIterator1 search(ForwardIterator1 haystack_b,
                        ForwardIterator1 haystack_e,
                        ForwardIterator2 needle_b,
                        ForwardIterator2 needle_e) {
  return search(haystack_b, haystack_e,
                needle_b, needle_e, equal_to<void>());
}

template<typename ForwardIterator1, typename ForwardIterator2,
         typename BinaryPredicate>
ForwardIterator1 search(ForwardIterator1 haystack_b,
                        ForwardIterator1 haystack_e,
                        ForwardIterator2 needle_b,
                        ForwardIterator2 needle_e,
                        BinaryPredicate predicate) {
  ForwardIterator1 h_mismatch;
  ForwardIterator2 n_mismatch;

  for (;;) {
    haystack_b = find(haystack_b, haystack_e, *needle_b);
    tie(h_mismatch, n_mismatch) = mismatch(haystack_b, haystack_e,
                                           needle_b, needle_e, ref(predicate));
    if (n_mismatch == needle_e) return haystack_b;
    if (h_mismatch == haystack_e) return haystack_e;
  }
  /* UNREACHABLE */
}

template<typename ForwardIterator, class Size, typename T>
ForwardIterator search_n(ForwardIterator b, ForwardIterator e,
                         Size n, const T& v) {
  return search_n(b, e, n, v, equal_to<void>());
}

template<typename ForwardIterator, class Size, typename T,
         typename BinaryPredicate>
ForwardIterator search_n(ForwardIterator b, ForwardIterator e,
                         Size n, const T& v,
                         BinaryPredicate predicate) {
  using placeholders::_1;

  if (n <= 0) return b;

  for (;;) {
    /* Find first match. */
    ForwardIterator match = b = find_if(b, e,
                                        bind(ref(predicate), _1, v));
    if (b == e) return b;

    /* Skip checking b again, since we already know it to be true. */
    ++b;
    Size count = 1;

    /* Search through the collection. */
    while (count < n && b != e && predicate(*b, v)) {
      ++count;
      ++b;
    }

    if (count == 0) return match;
    if (b == e) return b;
    ++b;
  }
  /* UNREACHABLE */
}


/* Some specializations for copy. */
namespace impl {

template<typename T>
auto algorithm_copy(const T* b, const T* e, T* out) ->
    enable_if_t<is_trivially_copyable<T>::value, T*> {
  const auto delta = e - b;
  memcpy(out, b, delta * sizeof(T));
  return out + delta;
}

template<typename T>
auto algorithm_copy(T* b, T* e, T* out) ->
    enable_if_t<is_trivially_copyable<T>::value, T*> {
  const auto delta = e - b;
  memcpy(out, b, delta * sizeof(T));
  return out + delta;
}

template<typename InputIterator, typename OutputIterator>
auto algorithm_copy(InputIterator b, InputIterator e, OutputIterator out) ->
    OutputIterator {
  while (b != e) {
    *out = *b;
    ++out;
    ++b;
  }
  return out;
}

} /* namespace std::impl */

template<typename InputIterator, typename OutputIterator>
OutputIterator copy(InputIterator b, InputIterator e, OutputIterator out) {
  return impl::algorithm_copy(b, e, out);
}

/* Some specializations for copy_n. */
namespace impl {

template<typename T, typename Size>
auto algorithm_copy_n(const T* b, Size n, T* out) ->
    enable_if_t<is_trivially_copyable<T>::value, T*> {
  memcpy(out, b, n * sizeof(T));
  return out + n;
}

template<typename T, typename Size>
auto algorithm_copy_n(T* b, Size n, T* out) ->
    enable_if_t<is_trivially_copyable<T>::value, T*> {
  memcpy(out, b, n * sizeof(T));
  return out + n;
}

template<typename InputIterator, typename Size, typename OutputIterator>
auto algorithm_copy_n(InputIterator b, Size n, OutputIterator out) ->
    OutputIterator {
  while (n > 0) {
    *out = *b;
    ++out;
    ++b;
    --n;
  }
  return out;
}

} /* namespace std::impl */

template<typename InputIterator, typename Size, typename OutputIterator>
OutputIterator copy_n(InputIterator b, Size n, OutputIterator out) {
  return impl::algorithm_copy_n(b, n, out);
}

template<typename InputIterator, typename OutputIterator, typename Predicate>
OutputIterator copy_if(InputIterator b, InputIterator e, OutputIterator out,
                       Predicate predicate) {
  while (b != e) {
    if (predicate(*b)) {
      *out = *b;
      ++out;
    }
    ++b;
  }
  return out;
}

template<typename BidirectionalIterator1, typename BidirectionalIterator2>
BidirectionalIterator2 copy_backward(BidirectionalIterator1 b,
                                     BidirectionalIterator1 e,
                                     BidirectionalIterator2 out) {
  while (e != b) *--out = *--e;
  return out;
}

/* Some specializations for move. */
namespace impl {

template<typename T>
auto algorithm_move(const T* b, const T* e, T* out) ->
    enable_if_t<is_trivially_copyable<T>::value, T*> {
  const auto delta = e - b;
  memmove(out, b, delta * sizeof(T));
  return out + delta;
}

template<typename T>
auto algorithm_move(T* b, T* e, T* out) ->
    enable_if_t<is_trivially_copyable<T>::value, T*> {
  const auto delta = e - b;
  memmove(out, b, delta * sizeof(T));
  return out + delta;
}

template<typename InputIterator, typename OutputIterator>
auto algorithm_move(InputIterator b, InputIterator e, OutputIterator out) ->
    OutputIterator {
  while (b != e) {
    *out = move(*b);
    ++out;
    ++b;
  }
  return out;
}

} /* namespace std::impl */

template<typename InputIterator, typename OutputIterator>
OutputIterator move(InputIterator b, InputIterator e, OutputIterator out) {
  return impl::algorithm_move(b, e, out);
}

/* Some specializations for move_backward. */
namespace impl {

template<typename T>
auto algorithm_move_backward(const T* b, const T* e, T* out) ->
    enable_if_t<is_trivially_copyable<T>::value, T*> {
  const auto delta = e - b;
  memmove(out - delta, b, delta * sizeof(T));
  return out + delta;
}

template<typename T>
auto algorithm_move_backward(T* b, T* e, T* out) ->
    enable_if_t<is_trivially_copyable<T>::value, T*> {
  const auto delta = e - b;
  memmove(out - delta, b, delta * sizeof(T));
  return out + delta;
}

template<typename InputIterator, typename OutputIterator>
auto algorithm_move_backward(InputIterator b, InputIterator e,
                             OutputIterator out) -> OutputIterator {
  while (b != e) *--out = move(*--e);
  return out;
}

} /* namespace std::impl */

template<typename BidirectionalIterator1, typename BidirectionalIterator2>
BidirectionalIterator2 move_backward(BidirectionalIterator1 b,
                                     BidirectionalIterator1 e,
                                     BidirectionalIterator2 out) {
  return impl::algorithm_move_backward(b, e, out);
}

template<typename ForwardIterator1, typename ForwardIterator2>
ForwardIterator2 swap_ranges(ForwardIterator1 b1, ForwardIterator1 e1,
                             ForwardIterator2 b2) {
  while (b1 != e1) {
    iter_swap(b1, b2);
    ++b1;
    ++b2;
  }
  return b2;
}

template<typename ForwardIterator1, typename ForwardIterator2>
void iter_swap(ForwardIterator1 x, ForwardIterator2 y) {
  swap(*x, *y);
}

template<typename InputIterator, typename OutputIterator,
         typename UnaryOperation>
OutputIterator transform(InputIterator b, InputIterator e,
                         OutputIterator out, UnaryOperation operation) {
  while (b != e) {
    *out = operation(*b);
    ++b;
    ++out;
  }
  return *out;
}

template<typename InputIterator1, typename InputIterator2,
         typename OutputIterator, typename BinaryOperation>
OutputIterator transform(InputIterator1 b1, InputIterator1 e1,
                         InputIterator2 b2,
                         OutputIterator out, BinaryOperation operation) {
  while (b1 != e1) {
    *out = operation(*b1, *b2);
    ++b1;
    ++b2;
    ++out;
  }
  return *out;
}

template<typename ForwardIterator, typename T>
void replace(ForwardIterator b, ForwardIterator e,
             const T& old_val, const T& new_val) {
  while (b != e) {
    if (*b == old_val) *b = new_val;
    ++b;
  }
}

template<typename ForwardIterator, typename Predicate, typename T>
void replace_if(ForwardIterator b, ForwardIterator e,
                Predicate predicate, const T& new_val) {
  while (b != e) {
    if (predicate(*b)) *b = new_val;
    ++b;
  }
}

template<typename ForwardIterator, typename OutputIterator, typename T>
void replace_copy(ForwardIterator b, ForwardIterator e, OutputIterator out,
                  const T& old_val, const T& new_val) {
  while (b != e) {
    *out = (*b == old_val ? new_val : *b);
    ++b;
    ++out;
  }
}

template<typename ForwardIterator, typename OutputIterator,
         typename Predicate, typename T>
void replace_copy_if(ForwardIterator b, ForwardIterator e, OutputIterator out,
                     Predicate predicate, const T& new_val) {
  while (b != e) {
    *out = (predicate(*b) ? new_val : *b);
    ++b;
    ++out;
  }
}

template<typename ForwardIterator, typename T>
void fill(ForwardIterator b, ForwardIterator e, const T& v) {
  while (b != e) {
    *b = v;
    ++b;
  }
}

template<typename ForwardIterator, typename Size, typename T>
ForwardIterator fill_n(ForwardIterator b, Size n, const T& v) {
  while (n > 0) {
    *b = v;
    ++b;
    --n;
  }
  return b;
}

template<typename OutputIterator, typename Generator>
void generate(OutputIterator b, OutputIterator e, Generator gen) {
  while (b != e) {
    *b = gen();
    ++b;
  }
}

template<typename OutputIterator, typename Size, typename Generator>
OutputIterator generate_n(OutputIterator b, Size n, Generator gen) {
  while (n > 0) {
    *b = gen();
    ++b;
    --n;
  }
  return b;
}

template<typename ForwardIterator, typename T>
ForwardIterator remove(ForwardIterator b, ForwardIterator e, const T& v) {
  b = find(b, e, v);
  if (b == e) return b;

  ForwardIterator out = b;
  ++b;
  while (b != e) {
    if (!(*b == v)) {
      *out = move(*b);
      ++out;
    }
    ++b;
  }
  return out;
}

template<typename ForwardIterator, typename Predicate>
ForwardIterator remove_if(ForwardIterator b, ForwardIterator e,
                          Predicate predicate) {
  while (b != e && !predicate(*b)) ++b;
  if (b == e) return b;

  ForwardIterator out = b;
  ++b;
  while (b != e) {
    if (!predicate(*b)) {
      *out = move(*b);
      ++out;
    }
    ++b;
  }
  return out;
}

template<typename InputIterator, typename OutputIterator, typename T>
OutputIterator remove_copy(InputIterator b, InputIterator e,
                           OutputIterator out, const T& v) {
  while (b != e) {
    if (!(*b == v)) {
      *out = move(*b);
      ++out;
    }
    ++b;
  }
  return out;
}

template<typename InputIterator, typename OutputIterator, typename Predicate>
OutputIterator remove_copy_if(InputIterator b, InputIterator e,
                              OutputIterator out, Predicate predicate) {
  while (b != e) {
    if (!predicate(*b)) {
      *out = move(*b);
      ++out;
    }
    ++b;
  }
  return out;
}

template<typename ForwardIterator>
ForwardIterator unique(ForwardIterator b, ForwardIterator e) {
  if (b == e) return b;  // Algorithm requires at least 1 element.

  /* Move b toward the first element that is to be eliminated. */
  {
    ForwardIterator b_pred = b;
    ++b;
    while (b != e && !(*b_pred == *b)) {
      b_pred = b;
      ++b;
    }
  }

  /*
   * From this point onward, anything that stays needs to be moved
   * backward.
   */
  ForwardIterator out = b;
  ++b;
  for (;;) {
    /* Skip current duplicates. */
    while (b != e && *out == *b) ++b;
    if (b == e) return out;

    *out = move(*b);
    ++b;
    ++out;
  }
  /* UNREACHABLE */
}

template<typename ForwardIterator, typename BinaryPredicate>
ForwardIterator unique(ForwardIterator b, ForwardIterator e,
                       BinaryPredicate predicate) {
  if (b == e) return b;  // Algorithm requires at least 1 element.

  /* Move b toward the first element that is to be eliminated. */
  {
    ForwardIterator b_pred = b;
    ++b;
    while (b != e && !predicate(*b_pred, *b)) {
      b_pred = b;
      ++b;
    }
  }

  /*
   * From this point onward, anything that stays needs to be moved
   * backward.
   */
  ForwardIterator out = b;
  ++b;
  for (;;) {
    /* Skip current duplicates. */
    while (b != e && predicate(*out, *b)) ++b;
    if (b == e) return out;

    *out = move(*b);
    ++b;
    ++out;
  }
  /* UNREACHABLE */
}

/* Support for unique_copy. */
namespace impl {

template<typename InputIterator, typename OutputIterator, typename Predicate>
OutputIterator algorithm_unique_copy(InputIterator b, InputIterator e,
                                     OutputIterator out,
                                     Predicate predicate,
                                     const input_iterator_tag&,
                                     const output_iterator_tag&) {
  if (b == e) return out;

  *out = *b;
  ++out;
  typename iterator_traits<InputIterator>::value_type last = *b;
  ++b;

  for (;;) {
    while (b != e && predicate(last, *b)) ++b;
    if (b == e) return out;

    *out = last = *b;
    ++out;
    ++b;
  }
}

template<typename InputIterator, typename ForwardIterator, typename Predicate>
ForwardIterator algorithm_unique_copy(InputIterator b, InputIterator e,
                                      ForwardIterator out,
                                      Predicate predicate,
                                      const input_iterator_tag&,
                                      const forward_iterator_tag&) {
  if (b == e) return out;

  *out = *b;
  ForwardIterator last = out;
  ++out;
  ++b;

  for (;;) {
    while (b != e && predicate(*last, *b)) ++b;
    if (b == e) return out;

    *out = *b;
    last = out;
    ++out;
    ++b;
  }
}

template<typename ForwardIterator, typename OutputIterator, typename Predicate>
OutputIterator algorithm_unique_copy(ForwardIterator b, ForwardIterator e,
                                     OutputIterator out,
                                     Predicate predicate,
                                     const forward_iterator_tag&,
                                     const output_iterator_tag&) {
  if (b == e) return out;

  *out = *b;
  ForwardIterator last = b;
  ++out;
  ++b;

  for (;;) {
    while (b != e && predicate(*last, *b)) ++b;
    if (b == e) return out;

    *out = *b;
    last = b;
    ++out;
    ++b;
  }
}

/* Disambiguation. */
template<typename ForwardIterator, typename OutputIterator, typename Predicate>
OutputIterator algorithm_unique_copy(ForwardIterator b, ForwardIterator e,
                                     OutputIterator out,
                                     Predicate predicate,
                                     const forward_iterator_tag& t,
                                     const forward_iterator_tag&) {
  return unique_copy(b, e, out, ref(predicate),
                     t, output_iterator_tag());
}

struct unique_cmp {
  template<typename T, typename U>
  bool operator()(const T& t, const U& u) {
    return t == u;
  }
};


} /* namespace std::impl */

template<typename InputIterator, typename OutputIterator>
OutputIterator unique_copy(InputIterator b, InputIterator e,
                           OutputIterator out) {
  return unique_copy(b, e, out, impl::unique_cmp());
}

template<typename InputIterator, typename OutputIterator,
         typename BinaryPredicate>
OutputIterator unique_copy(InputIterator b, InputIterator e,
                           OutputIterator out,
                           BinaryPredicate predicate) {
  return impl::algorithm_unique_copy(
      b, e, out, ref(predicate),
      typename iterator_traits<InputIterator>::iterator_tag(),
      typename iterator_traits<OutputIterator>::iterator_tag());
}

template<typename BidirectionalIterator>
void reverse(BidirectionalIterator b, BidirectionalIterator e) {
  if (b == e) return;
  --e;

  while (b != e) {
    iter_swap(b, e);
    ++b;
    if (b == e) break;
    --e;
  }
}

template<typename BidirectionalIterator, typename OutputIterator>
OutputIterator reverse(BidirectionalIterator b, BidirectionalIterator e,
                       OutputIterator out) {
  while (b != e) {
    *out = *--e;
    ++out;
  }
  return out;
}

template<typename ForwardIterator>
ForwardIterator rotate(ForwardIterator first, ForwardIterator middle,
                       ForwardIterator last) {
  ForwardIterator result = last;
  bool result_set = false;

  auto shift = distance(first, middle);
  while (shift != 0) {
    auto len_mod_shift_base_1 = shift;
    for (ForwardIterator i = middle;
         i != last;
         ++first, ++i) {
      iter_swap(first, i);
      len_mod_shift_base_1 = (len_mod_shift_base_1 == shift ?
                              1U :
                              len_mod_shift_base_1 + 1U);
    }
    if (!exchange(result_set, true)) result = first;

    /*
     * Shift the last few elements to fix ordering.
     */
    shift -= len_mod_shift_base_1;
    middle = next(first, shift);
  }

  return result;
}

template<typename ForwardIterator, typename OutputIterator>
OutputIterator rotate_copy(ForwardIterator b, ForwardIterator middle,
                           ForwardIterator e,
                           OutputIterator out) {
  return copy(b, middle, copy(middle, e, out));
}

template<typename InputIterator, typename Predicate>
bool is_partitioned(InputIterator b, InputIterator e, Predicate predicate) {
  b = find_if_not(b, e, ref(predicate));
  return b == e || none_of(next(b), e, ref(predicate));
}

template<typename ForwardIterator, typename Predicate>
ForwardIterator partition(ForwardIterator b, ForwardIterator e,
                          Predicate predicate) {
  b = find_if_not(b, e, ref(predicate));
  if (b == e) return b;

  ForwardIterator i = next(b);
  while ((i = find_if(i, e, ref(predicate))) != e)
    iter_swap(b++, i++);
  return b;
}

/* Support for stable_partition. */
namespace impl {

/* stable partition, but requires that *b is false. */
template<typename BidirectionalIterator, typename Predicate>
BidirectionalIterator stable_partition_inplace(
    BidirectionalIterator b, BidirectionalIterator e, Predicate predicate) {
  if (b == e) return b;
  assert(predicate(*b) == false);

  BidirectionalIterator middle = find_if(next(b), e, ref(predicate));

  if (middle != e) {
    /*
     * To meet the assert above during recursion, we need an element for which
     * predicate yields false.
     */
    BidirectionalIterator next_fail = find_if_not(next(middle), e,
                                                  ref(predicate));
    e = stable_partition_inplace(next_fail, e, ref(predicate));
  }
  return rotate(b, middle, e);
}

template<typename BidirectionalIterator, typename Predicate>
auto stable_partition_using_heap(BidirectionalIterator b,
                                 BidirectionalIterator e,
                                 Predicate predicate) ->
    enable_if_t<is_nothrow_move_constructible<
                    typename iterator_traits<BidirectionalIterator>::value_type
                    >::value &&
                is_nothrow_move_assignable<
                    typename iterator_traits<BidirectionalIterator>::value_type
                    >::value &&
                noexcept(predicate(*b)),
                tuple<BidirectionalIterator, bool>> {
  using value_type =
      typename iterator_traits<BidirectionalIterator>::value_type;

  /* Skip all elements that are in the correct position already. */
  b = find_if_not(b, e, ref(predicate));
  if (b == e) return make_tuple(b, true);

  /*
   * Allocate array that is sufficiently large to hold elements that need
   * to be at the end of the range.  Worst case: all remaining elements.
   */
  auto array = impl::heap_array<value_type>(distance(b, e));
  if (!array) return make_tuple(b, false);  // No memory.

  /*
   * Since b needs to be at the end (predicate is known to be false),
   * move it to the temporary storage now.
   */
  array.emplace_back(move(*b));
  /*
   * Move remaining elements into position:
   * - all elements that need to be at the head are compacted using b,
   * - all elements that need to be at the tail are moved to the array.
   */
  for (BidirectionalIterator i = next(b); i != e; ++i) {
    if (predicate(*i))
      *b++ = move(*i);
    else
      array.emplace_back(move(*i));
  }

  /* Put elements in temporary array back. */
  move(array.begin(), array.end(), b);
  return make_tuple(b, true);
}

/*
 * Cannot guarantee that we won't loose elements if move or predicate throws.
 * Fallback by not doing the heap based code.
 */
template<typename BidirectionalIterator, typename Predicate>
auto stable_partition_using_heap(BidirectionalIterator b,
                                 BidirectionalIterator e,
                                 Predicate predicate) ->
    enable_if_t<!(is_nothrow_move_constructible<
                      typename iterator_traits<BidirectionalIterator>::
                      value_type
                      >::value &&
                  is_nothrow_move_assignable<
                      typename iterator_traits<BidirectionalIterator>::
                      value_type
                      >::value &&
                  noexcept(predicate(*b))),
                tuple<BidirectionalIterator, bool>> {
  b = find_if_not(b, e, ref(predicate));
  return make_tuple(b, b == e);
}

template<typename BidirectionalIterator, typename Predicate>
BidirectionalIterator stable_partition_impl(BidirectionalIterator b,
                                            BidirectionalIterator e,
                                            Predicate predicate) {
  bool succes;
  tie(b, succes) = stable_partition_using_heap(b, e, ref(predicate));
  if (!succes) b = stable_partition_inplace(b, e, ref(predicate));
  return b;
}

} /* namespace std::impl */

template<typename BidirectionalIterator, typename Predicate>
BidirectionalIterator stable_partition(BidirectionalIterator b,
                                       BidirectionalIterator e,
                                       Predicate predicate) {
  return impl::stable_partition_impl(b, e, ref(predicate));
}

template<typename InputIterator, typename OutputIterator1,
         typename OutputIterator2, typename Predicate>
pair<OutputIterator1, OutputIterator2> partition_copy(InputIterator b,
                                                      InputIterator e,
                                                      OutputIterator1 o_true,
                                                      OutputIterator2 o_false,
                                                      Predicate predicate) {
  while (b != e) {
    if (predicate(*b))
      *o_true++ = *b;
    else
      *o_false++ = *b;
  }
  return make_pair(o_true, o_false);
}

template<typename ForwardIterator, typename Predicate>
ForwardIterator partition_point(ForwardIterator b, ForwardIterator e,
                                Predicate predicate) {
  assert(is_partitioned(b, e, ref(predicate)));
  return find_if_not(b, e, ref(predicate));
}


template<typename RandomAccessIterator>
void sort(RandomAccessIterator b, RandomAccessIterator e) {
  sort(b, e, less<void>());
}

template<typename RandomAccessIterator, typename Predicate>
void sort(RandomAccessIterator b, RandomAccessIterator e,
          Predicate predicate) {
  using placeholders::_1;

  if (b == e || next(b) == e) return;  // Already sorted.

  /*
   * Elements in b, e may not be copyable, so we can't simply bind an argument
   * to the predicate and use that to directly partition the collection.
   * Using a reference to an element in the collection doesn't work either,
   * since the elements will be shuffled, causing our predicate to change
   * behind our back.
   *
   * Instead, use some song and dance:
   * - select an element mid as the pivot
   * - partition the space before and after mid (exclusing mid itself)
   * - rotate the collection to become correctly ordered
   * Sort will proceed recursively on the two partitions.
   */
  RandomAccessIterator mid = next(b, distance(b, e) / 2U);  // XXX use random generator!
  auto left_mid = partition(
      b, mid,
      bind(logical_not<void>(), bind(ref(predicate), ref(*mid), _1)));
  auto right_mid = partition(next(mid), e,
                             bind(ref(predicate), _1, ref(*mid)));

  /*
   * Layout:
   * - [b, left_mid)           before pivot            \o/
   * - [left_mid, mid)         should be after pivot
   * - pivot = mid             pivot point
   * - [next(mid), right_mid)  should be before pivot
   * - [right_mid, e)          after pivot             \o/
   */
  rotate(left_mid, mid, next(mid));

  /*
   * Layout:
   * - [b, left_mid)           before pivot            \o/
   * - pivot = left_mid        pivot point
   * - (left_mid, mid]         should be after pivot   \o/
   * - [next(mid), right_mid)  should be before pivot
   * - [right_mid, e)          after pivot             \o/
   */
  RandomAccessIterator pivot = rotate(left_mid, next(mid), right_mid);

  /*
   * This time, use the original names for the segments.
   * Layout:
   * - [b, left_mid)                                   \o/
   * - (mid, right_mid)                                \o/
   * - pivot
   * - [left_mid, mid)                                 \o/
   * - [right_mid, e)                                  \o/
   */
  sort(b, pivot, ref(predicate));
  sort(next(pivot), e, ref(predicate));
}

template<typename RandomAccessIterator>
void stable_sort(RandomAccessIterator b, RandomAccessIterator e) {
  stable_sort(b, e, less<void>());
}


template<typename RandomAccessIterator, typename Predicate>
void stable_sort(RandomAccessIterator b, RandomAccessIterator e,
                 Predicate predicate) {
  using placeholders::_1;

  if (b == e || next(b) == e) return;  // Already sorted.

  /*
   * Elements in b, e may not be copyable, so we can't simply bind an argument
   * to the predicate and use that to directly partition the collection.
   * Using a reference to an element in the collection doesn't work either,
   * since the elements will be shuffled, causing our predicate to change
   * behind our back.
   *
   * Instead, use some song and dance:
   * - select an element mid as the pivot
   * - partition the space before and after mid (exclusing mid itself)
   * - rotate the collection to become correctly ordered
   * Sort will proceed recursively on the two partitions.
   */
  RandomAccessIterator mid = next(b, distance(b, e) / 2U);  // XXX use random generator!
  auto left_mid = stable_partition(b, mid,
      bind(logical_not<void>(), bind(ref(predicate), ref(*mid), _1)));
  auto right_mid = stable_partition(next(mid), e,
                                    bind(ref(predicate), _1, ref(*mid)));

  /*
   * Layout:
   * - [b, left_mid)           before pivot            \o/
   * - [left_mid, mid)         should be after pivot
   * - pivot = mid             pivot point
   * - [next(mid), right_mid)  should be before pivot
   * - [right_mid, e)          after pivot             \o/
   */
  rotate(left_mid, mid, next(mid));

  /*
   * Layout:
   * - [b, left_mid)           before pivot            \o/
   * - pivot = left_mid        pivot point
   * - (left_mid, mid]         should be after pivot   \o/
   * - [next(mid), right_mid)  should be before pivot
   * - [right_mid, e)          after pivot             \o/
   */
  RandomAccessIterator pivot = rotate(left_mid, next(mid), right_mid);

  /*
   * This time, use the original names for the segments.
   * Layout:
   * - [b, left_mid)                                   \o/
   * - (mid, right_mid)                                \o/
   * - pivot
   * - [left_mid, mid)                                 \o/
   * - [right_mid, e)                                  \o/
   */
  stable_sort(b, pivot, ref(predicate));
  stable_sort(next(pivot), e, ref(predicate));
}

template<typename RandomAccessIterator>
void partial_sort(RandomAccessIterator b, RandomAccessIterator mid,
                  RandomAccessIterator e) {
  partial_sort(b, mid, e, less<void>());
}

template<typename RandomAccessIterator, typename Predicate>
void partial_sort(RandomAccessIterator b, RandomAccessIterator mid,
                  RandomAccessIterator e, Predicate predicate) {
  if (_predict_false(mid == e)) {
    sort(b, e, ref(predicate));
    return;
  }

  auto n = distance(b, e);
  auto mid_n = distance(b, mid);
  RandomAccessIterator half = next(b, n / 2);

  if (distance(b, half) <= mid_n)
    sort(b, half, ref(predicate));
  else
    partial_sort(b, mid, half, ref(predicate));

  if (distance(half, e) <= mid_n)
    sort(half, e, ref(predicate));
  else
    partial_sort(half, next(half, mid_n), e, ref(predicate));

  /* Merge half, e into mid. */
  while (half != e &&
         (b = upper_bound(b, mid, *half, ref(predicate))) != mid) {
    rotate(b, prev(mid), mid);
    iter_swap(b, half);
  }
}

template<typename InputIterator, typename RandomAccessIterator>
RandomAccessIterator partial_sort_copy(InputIterator b, InputIterator e,
                                       RandomAccessIterator out_b,
                                       RandomAccessIterator out_e) {
  return partial_sort_copy(b, e, out_b, out_e, less<void>());
}

template<typename InputIterator, typename RandomAccessIterator,
         typename Predicate>
RandomAccessIterator partial_sort_copy(InputIterator b, InputIterator e,
                                       RandomAccessIterator out_b,
                                       RandomAccessIterator out_e,
                                       Predicate predicate) {
  const auto N = distance(out_b, out_e);
  InputIterator i = b;
  /* Advance i up to N positions. */
  for (auto i_N = N; i_N > 0 && i != e; --i_N) ++i;

  out_e = copy(b, i, out_b);
  sort(out_b, out_e, ref(predicate));

  while (i != e) {
    auto out_ipos = upper_bound(out_b, out_e, *i, ref(predicate));
    if (out_ipos != out_e) {
      move_backward(out_ipos, prev(out_e), out_e);
      *out_ipos = *i;
    }
    ++i;
  }
  return out_e;
}

template<typename InputIterator>
bool is_sorted(InputIterator b, InputIterator e) {
  return is_sorted_until(b, e) == e;
}

template<typename InputIterator, typename Predicate>
bool is_sorted(InputIterator b, InputIterator e, Predicate predicate) {
  return is_sorted_until(b, e, predicate) == e;
}

template<typename InputIterator>
InputIterator is_sorted_until(InputIterator b, InputIterator e) {
  return is_sorted_until(b, e, less<void>());
}

template<typename InputIterator, typename Predicate>
InputIterator is_sorted_until(InputIterator b, InputIterator e,
                              Predicate predicate) {
  if (b == e) return e;

  InputIterator i = next(b);
  while (i != e && predicate(*b, *i)) {
    ++b;
    ++i;
  }
  return i;
}

template<typename RandomAccessIterator>
void nth_element(RandomAccessIterator b, RandomAccessIterator nth,
                 RandomAccessIterator e) {
  nth_element(b, nth, e, less<void>());
}

template<typename RandomAccessIterator, typename Predicate>
void nth_element(RandomAccessIterator b, RandomAccessIterator nth,
                 RandomAccessIterator e, Predicate predicate) {
  using placeholders::_1;

  assert(b == nth || distance(b, nth) < distance(b, e));

  /*
   * Use pivot based selection, as in sort, but only traverse one leg of
   * the two partitions instead of both.
   *
   * Since each stage divides the range in half,
   * at most 2 * distance(b, e) - 1 invocations of the predicate are required,
   * given that the pivot is well chosen.
   */
  while (b != e && next(b) != e) {
    RandomAccessIterator mid = next(b, distance(b, e) / 2U);  // XXX use random generator!
    auto left_mid = partition(
        b, mid,
        bind(logical_not<void>(), bind(ref(predicate), ref(*mid), _1)));
    auto right_mid = partition(next(mid), e,
                               bind(ref(predicate), _1, ref(*mid)));

    rotate(left_mid, mid, next(mid));
    RandomAccessIterator pivot = rotate(left_mid, next(mid), right_mid);

    if (pivot < nth)
      b = next(pivot);
    else if (pivot > nth)
      e = pivot;
    else
      return;  // pivot == nth, which is partitioned into the right place.
  }
}


template<typename ForwardIterator, typename T>
ForwardIterator lower_bound(ForwardIterator b, ForwardIterator e, const T& v) {
  return lower_bound(b, e, v, less<void>());
}

template<typename ForwardIterator, typename T, typename Predicate>
ForwardIterator lower_bound(ForwardIterator b, ForwardIterator e, const T& v,
                            Predicate predicate) {
  while (b != e) {
    ForwardIterator m = next(b, distance(b, e) / 2);
    if (predicate(*m, v))  // *m is too small
      b = next(m);
    else
      e = m;
  }
  return b;
}

template<typename ForwardIterator, typename T>
ForwardIterator upper_bound(ForwardIterator b, ForwardIterator e, const T& v) {
  return upper_bound(b, e, v, less<void>());
}

template<typename ForwardIterator, typename T, typename Predicate>
ForwardIterator upper_bound(ForwardIterator b, ForwardIterator e, const T& v,
                            Predicate predicate) {
  using placeholders::_1;
  using placeholders::_2;

  /* This invokes lower_bound with a <= variation of the predicate. */
  return lower_bound(b, e, v,
                     bind(logical_not<void>(), bind(ref(predicate), _2, _1)));
}

template<typename ForwardIterator, typename T>
pair<ForwardIterator, ForwardIterator> equal_range(ForwardIterator b,
                                                   ForwardIterator e,
                                                   const T& v) {
  return equal_range(b, e, v, less<void>());
}

template<typename ForwardIterator, typename T, typename Predicate>
pair<ForwardIterator, ForwardIterator> equal_range(ForwardIterator b,
                                                   ForwardIterator e,
                                                   const T& v,
                                                   Predicate predicate) {
  b = lower_bound(b, e, v, ref(predicate));
  e = upper_bound(b, e, v, ref(predicate));
  return make_pair(b, e);
}

template<typename ForwardIterator, typename T>
bool binary_search(ForwardIterator b, ForwardIterator e, const T& v) {
  return binary_search(b, e, v, less<void>());
}

template<typename ForwardIterator, typename T, typename Predicate>
bool binary_search(ForwardIterator b, ForwardIterator e, const T& v,
                   Predicate predicate) {
  using placeholders::_1;
  using placeholders::_2;

  /* This invokes lower_bound with a <= variation of the predicate. */
  b = lower_bound(b, e, v, ref(predicate));
  return b != e && !predicate(v, *b);
}


template<typename InputIterator1, typename InputIterator2,
         typename OutputIterator>
OutputIterator merge(InputIterator1 b1, InputIterator1 e1,
                     InputIterator2 b2, InputIterator2 e2,
                     OutputIterator out) {
  return merge(b1, e1, b2, e2, out, less<void>());
}

template<typename InputIterator1, typename InputIterator2,
         typename OutputIterator, typename Predicate>
OutputIterator merge(InputIterator1 b1, InputIterator1 e1,
                     InputIterator2 b2, InputIterator2 e2,
                     OutputIterator out, Predicate predicate) {
  while (b1 != e1 && b2 != e2) {
    if (predicate(*b1, *b2))
      *out++ = *b1++;
    else
      *out++ = *b2++;
  }

  out = copy(b1, e1, out);
  out = copy(b2, e2, out);
  return out;
}

template<typename BidirectionalIterator>
void inplace_merge(BidirectionalIterator b, BidirectionalIterator mid,
                   BidirectionalIterator e) {
  inplace_merge(b, mid, e, less<void>());
}

template<typename BidirectionalIterator, typename Predicate>
void inplace_merge(BidirectionalIterator b, BidirectionalIterator mid,
                   BidirectionalIterator e, Predicate predicate) {
  using value_type =
      typename iterator_traits<BidirectionalIterator>::value_type;

  assert(distance(b, mid) <= distance(b, e));

  if (is_nothrow_move_constructible<value_type>::value) {
    /* Attempt to use the heap and a temporary copy. */
    auto array1 = impl::heap_array<value_type>(distance(b, mid));
    auto array2 = impl::heap_array<value_type>(distance(mid, e));

    if (_predict_true(array1 && array2)) {
      move(b, mid, back_inserter(array1));
      move(mid, e, back_inserter(array2));
      merge(make_move_iterator(array1.begin()),
            make_move_iterator(array1.end()),
            make_move_iterator(array2.begin()),
            make_move_iterator(array2.end()),
            b);
      return;
    }
  }

  auto be = mid;
  while (mid != e &&
         (b = upper_bound(b, mid, *mid, ref(predicate))) != mid) {
    /* mid should be before b. */

    /* Find all instance in [mid, e) that should be before b. */
    auto mid_e = upper_bound(next(mid), e, *mid, ref(predicate));

    /* Rotate elements to fix mid. */
    rotate(b, mid, mid_e);
    b = next(b, distance(mid, mid_e));
    be = next(be, distance(mid, mid_e));
    mid = mid_e;
  }
}


template<typename InputIterator1, typename InputIterator2>
bool includes(InputIterator1 b1, InputIterator1 e1,
              InputIterator2 b2, InputIterator2 e2) {
  return includes(b1, e1, b2, e2, less<void>());
}

template<typename InputIterator1, typename InputIterator2, typename Predicate>
bool includes(InputIterator1 b1, InputIterator1 e1,
              InputIterator2 b2, InputIterator2 e2, Predicate predicate) {
  using value1_type = typename iterator_traits<InputIterator1>::value_type;
  using value2_type = typename iterator_traits<InputIterator2>::value_type;

  while (b2 != e2) {
    if (b1 == e1) return false;
    value2_type v2 = *b2;
    value1_type v1 = *b1;
    while (predicate(v1, v2)) {
      ++b1;
      if (b1 == e1) return false;
      v1 = *b1;
    }
    if (!predicate(v2, v1)) return false;
    ++b2;
  }
  return true;
}

template<typename InputIterator1, typename InputIterator2,
         typename OutputIterator>
OutputIterator set_union(InputIterator1 b1, InputIterator1 e1,
                         InputIterator2 b2, InputIterator2 e2,
                         OutputIterator out) {
  return set_union(b1, e1, b2, e2, out, less<void>());
}

template<typename InputIterator1, typename InputIterator2,
         typename OutputIterator, typename Predicate>
OutputIterator set_union(InputIterator1 b1, InputIterator1 e1,
                         InputIterator2 b2, InputIterator2 e2,
                         OutputIterator out, Predicate predicate) {
  using value1_type = typename iterator_traits<InputIterator1>::value_type;
  using value2_type = typename iterator_traits<InputIterator2>::value_type;

  /*
   * Loop initialization requires both collections to be non-empty.
   * Handle the cases with empty collections first.
   */
  if (b1 == e1) return copy(b2, e2, out);
  if (b2 == e2) return copy(b1, e1, out);

  value1_type v1 = *b1;
  value2_type v2 = *b2;
  do {
    /*
     * Figure out if *b1 or *b2 is earlier.
     *
     * Note that if both are equal, neither will be true;
     * in which case we must advance both, but only copy 1 of them.
     *
     * Note that the loop guard guarantees at least one of the sets
     * will be non-empty.
     */
    const bool pick1 = (b2 == e2 || (b1 != e1 && predicate(v1, v2)));
    const bool pick2 = (b1 == e1 || (b2 != e2 && predicate(v2, v1)));

    /*
     * Copy the one that compares as before.
     * If both are the same, copy v1.
     */
    if (!pick2)
      *out++ = move(v1);
    else
      *out++ = move(v2);

    /*
     * Skip the element that was copied,
     * if both the same, skip them both.
     */
    if (!pick2) ++b1;
    if (!pick1) ++b2;

    /* Update v1 and v2 according to the skip step above. */
    if (b1 != e1 && !pick2) v1 = *b1;
    if (b2 != e2 && !pick1) v2 = *b2;
  } while (b1 != e1 || b2 != e2);
  return out;
}

template<typename InputIterator1, typename InputIterator2,
         typename OutputIterator>
OutputIterator set_intersection(InputIterator1 b1, InputIterator1 e1,
                                InputIterator2 b2, InputIterator2 e2,
                                OutputIterator out) {
  return set_intersection(b1, e1, b2, e2, out, less<void>());
}

template<typename InputIterator1, typename InputIterator2,
         typename OutputIterator, typename Predicate>
OutputIterator set_intersection(InputIterator1 b1, InputIterator1 e1,
                                InputIterator2 b2, InputIterator2 e2,
                                OutputIterator out, Predicate predicate) {
  using value1_type = typename iterator_traits<InputIterator1>::value_type;
  using value2_type = typename iterator_traits<InputIterator2>::value_type;

  /* Deal with empty sets. */
  if (b1 == e1 || b2 == e2) return out;

  value1_type v1 = *b1;
  value2_type v2 = *b2;
  do {
    if (predicate(v1, v2)) {
      /*
       * v1 before v2: since not equal, skip v1
       * (which will reach at/after v2 eventually).
       */
      ++b1;
      if (b1 != e1) v1 = *b1;
    } else if (predicate(v2, v1)) {
      /*
       * v2 before v1: since not equal, skip v2
       * (which will reach at/after v1 eventually).
       */
      ++b2;
      if (b2 != e2) v2 = *b2;
    } else {
      /*
       * v1 equal to v2: copy v1 to output,
       * increment both iterators.
       */
      *out++ = move(v1);
      ++b1;
      if (b1 != e1) v1 = *b1;
      ++b2;
      if (b2 != e2) v2 = *b2;
    }
  } while (b1 != e1 && b2 != e2);
  return out;
}

template<typename InputIterator1, typename InputIterator2,
         typename OutputIterator>
OutputIterator set_symmetric_difference(InputIterator1 b1, InputIterator1 e1,
                                        InputIterator2 b2, InputIterator2 e2,
                                        OutputIterator out) {
  return set_symmetric_difference(b1, e1, b2, e2, out, less<void>());
}

template<typename InputIterator1, typename InputIterator2,
         typename OutputIterator, typename Predicate>
OutputIterator set_symmetric_difference(InputIterator1 b1, InputIterator1 e1,
                                        InputIterator2 b2, InputIterator2 e2,
                                        OutputIterator out,
                                        Predicate predicate) {
  using value1_type = typename iterator_traits<InputIterator1>::value_type;
  using value2_type = typename iterator_traits<InputIterator2>::value_type;

  /* Deal with empty sets. */
  if (b1 == e1) return copy(b2, e2, out);
  if (b2 == e2) return copy(b1, e1, out);

  value1_type v1 = *b1;
  value2_type v2 = *b2;
  do {
    /*
     * Figure out if *b1 or *b2 is earlier.
     *
     * Note that if both are equal, neither will be true;
     * in which case we must advance both, but copy none of them.
     *
     * Note that the loop guard guarantees at least one of the sets
     * will be non-empty.
     */
    const bool pick1 = (b2 == e2 || (b1 != e1 && predicate(v1, v2)));
    const bool pick2 = (b1 == e1 || (b2 != e2 && predicate(v2, v1)));

    /*
     * Copy the one that compares as before.
     * If both are the same, copy none of them.
     */
    if (pick1)
      *out++ = move(v1);
    else if (pick2)
      *out++ = move(v2);

    /*
     * Skip the element that was copied,
     * if both the same, skip them both.
     */
    if (!pick2) ++b1;
    if (!pick1) ++b2;

    /* Update v1 and v2 according to the skip step above. */
    if (b1 != e1 && !pick2) v1 = *b1;
    if (b2 != e2 && !pick1) v2 = *b2;
  } while (b1 != e1 || b2 != e2);
  return out;
}


template<typename RandomAccessIterator>
void push_heap(RandomAccessIterator b, RandomAccessIterator e) {
  push_heap(b, e, less<void>());
}

template<typename RandomAccessIterator, typename Predicate>
void push_heap(RandomAccessIterator b, RandomAccessIterator e,
               Predicate predicate) {
  using namespace _namespace(std)::impl::heap;

  assert(b != e);

  e = prev(e);  // Change e into the inserted element.
  auto idx = distance(b, e);

  while (idx != 0) {
    auto parent_idx = elem_parent(idx);
    RandomAccessIterator parent = b + parent_idx;
    if (!predicate(*parent, *e)) break;  // GUARD

    iter_swap(parent, e);
    idx = parent_idx;
    e = parent;
  }
}

/* Fix a single violation of the heap invariant, located at position i. */
namespace impl {

template<typename RandomAccessIterator, typename Predicate>
void fix_heap(RandomAccessIterator b, RandomAccessIterator i,
              RandomAccessIterator e, Predicate predicate) {
  using namespace _namespace(std)::impl::heap;
  using _namespace(std)::distance;

  const auto len = distance(b, e);

  for (;;) {
    const auto c1_idx = elem_child(distance(b, i));
    const auto c2_idx = elem_child(distance(b, i)) + 1U;
    if (c1_idx >= len) break;  // GUARD: end of list
    RandomAccessIterator c1 = next(b, c1_idx);
    RandomAccessIterator c2 = next(b, c2_idx);

    RandomAccessIterator biggest_child = c1;
    if (c2 != e && !predicate(c2, c1)) biggest_child = c2;
    if (!predicate(i, biggest_child)) break;  // GUARD: i at correct position

    iter_swap(i, biggest_child);
    i = biggest_child;
  }
}

} /* namespace std::impl */

template<typename RandomAccessIterator>
void pop_heap(RandomAccessIterator b, RandomAccessIterator e) {
  pop_heap(b, e, less<void>());
}

template<typename RandomAccessIterator, typename Predicate>
void pop_heap(RandomAccessIterator b, RandomAccessIterator e,
              Predicate predicate) {
  assert(b != e);

  e = prev(e);  // Change e into the popped element.
  iter_swap(b, e);
  impl::fix_heap(b, b, e, ref(predicate));
}

template<typename RandomAccessIterator>
void make_heap(RandomAccessIterator b, RandomAccessIterator e) {
  make_heap(b, e, less<void>());
}

template<typename RandomAccessIterator, typename Predicate>
void make_heap(RandomAccessIterator b, RandomAccessIterator e,
               Predicate predicate) {
  if (b == e) return;  // Skip empty set.

  /*
   * All leaves are already heaps,
   * start with the first element that has children.
   */
  const auto idx = distance(b, e) / 2U - 1U;
  auto i = next(b, idx);
  for (;;) {
    fix_heap(b, i, e, ref(predicate));
    if (i == b) break;  // GUARD
    --i;
  }
}

template<typename RandomAccessIterator>
void sort_heap(RandomAccessIterator b, RandomAccessIterator e) {
  sort_heap(b, e, less<void>());
}

template<typename RandomAccessIterator, typename Predicate>
void sort_heap(RandomAccessIterator b, RandomAccessIterator e,
               Predicate predicate) {
  while (b != e) {
    pop_heap(b, e, ref(predicate));
    --e;
  }
}

template<typename RandomAccessIterator>
bool is_heap(RandomAccessIterator b, RandomAccessIterator e) {
  return is_heap(b, e, less<void>());
}

template<typename RandomAccessIterator, typename Predicate>
bool is_heap(RandomAccessIterator b, RandomAccessIterator e,
             Predicate predicate) {
  return is_heap_until(b, e, ref(predicate)) == e;
}

template<typename RandomAccessIterator>
RandomAccessIterator is_heap_until(RandomAccessIterator b,
                                   RandomAccessIterator e) {
  return is_heap_until(b, e, less<void>());
}

template<typename RandomAccessIterator, typename Predicate>
RandomAccessIterator is_heap_until(RandomAccessIterator b,
                                   RandomAccessIterator e,
                                   Predicate predicate) {
  using namespace _namespace(std)::impl::heap;
  using _namespace(std)::distance;
  using difference_type =
      typename iterator_traits<RandomAccessIterator>::difference_type;

  const difference_type len = distance(b, e);
  RandomAccessIterator i = b;
  for (difference_type idx = 0; idx < (len - 1) / 2; ++idx, ++i) {
    const auto c1_idx = elem_child(idx);
    const auto c2_idx = elem_child(idx) + 1U;
    assert(c1_idx < len);
    RandomAccessIterator c1 = next(b, c1_idx);
    RandomAccessIterator c2 = next(b, c2_idx);

    RandomAccessIterator biggest_child = c1;
    if (c2 != e && !predicate(c2, c1)) biggest_child = c2;
    if (predicate(i, biggest_child)) return i;  // not a heap
  }
  return e;  // all elements except leaves checked (since leaves are trivial)
}


template<typename T>
auto min(const T& a, const T& b) -> const T& {
  return (a < b ? a : b);
}

template<typename T>
auto max(const T& a, const T& b) -> const T& {
  return (a > b ? a : b);
}

template<typename T, typename Comp>
auto min(const T& a, const T& b, Comp cmp) -> const T& {
  return (cmp(b, a) ? b : a);
}

template<typename T, typename Comp>
auto max(const T& a, const T& b, Comp cmp) -> const T& {
  return (cmp(a, b) ? b : a);
}

template<typename T>
auto min(initializer_list<T> il) -> T {
  const T* ptr = nullptr;
  for (const T& i : il) {
    if (ptr == nullptr)
      ptr = addresof(i);
    else
      ptr = addresof(min(*ptr, i));
  }
  return *ptr;
}

template<typename T>
auto max(initializer_list<T> il) -> T {
  const T* ptr = nullptr;
  for (const T& i : il) {
    if (ptr == nullptr)
      ptr = addressof(i);
    else
      ptr = addressof(max(*ptr, i));
  }
  return *ptr;
}

template<typename T, typename Comp>
auto min(initializer_list<T> il, Comp cmp) -> T {
  const T* ptr = nullptr;
  for (const T& i : il) {
    if (ptr == nullptr)
      ptr = addressof(i);
    else
      ptr = addressof(min(*ptr, i, cmp));
  }
  return *ptr;
}

template<typename T, typename Comp>
auto max(initializer_list<T> il, Comp cmp) -> T {
  const T* ptr = nullptr;
  for (const T& i : il) {
    if (ptr == nullptr)
      ptr = addressof(i);
    else
      ptr = addressof(max(*ptr, i, cmp));
  }
  return *ptr;
}

template<typename T>
auto minmax(const T& a, const T& b) -> pair<const T&, const T&> {
  return minmax(a, b, less<T>());
}

template<typename T, typename Compare>
auto minmax(const T& a, const T& b, Compare comp) -> pair<const T&, const T&> {
  return (comp(b, a) ?
          pair<const T&, const T&>(b, a) :
          pair<const T&, const T&>(a, b));
}

template<typename T>
auto minmax(initializer_list<T> il) -> pair<T, T> {
  return minmax(il, less<T>());
}

template<typename T, typename Compare>
auto minmax(initializer_list<T> il, Compare comp) -> pair<T, T> {
  assert(il.size() > 0);
  auto b = il.begin();
  pair<T, T> rv = pair<T, T>(*b, *b);

  for (b = next(b); b != il.end(); ++b) {
    if (comp(rv.second, *b))
      rv.second = *b;
    else if (comp(*b, rv.first))
      rv.first = *b;
  }
  return rv;
}

template<typename ForwardIterator>
ForwardIterator min_element(ForwardIterator b, ForwardIterator e) {
  return min_element(b, e, less<void>());
}

template<typename ForwardIterator, typename Predicate>
ForwardIterator min_element(ForwardIterator b, ForwardIterator e,
                            Predicate predicate) {
  if (b == e) return b;
  ForwardIterator rv = b;
  while (++b != e) if (predicate(*b, *rv)) rv = b;
  return rv;
}

template<typename ForwardIterator>
ForwardIterator max_element(ForwardIterator b, ForwardIterator e) {
  return max_element(b, e, less<void>());
}

template<typename ForwardIterator, typename Predicate>
ForwardIterator max_element(ForwardIterator b, ForwardIterator e,
                            Predicate predicate) {
  if (b == e) return b;
  ForwardIterator rv = b;
  while (++b != e) if (predicate(*rv, *b)) rv = b;
  return rv;
}

template<typename ForwardIterator>
pair<ForwardIterator, ForwardIterator> minmax_element(ForwardIterator b,
                                                      ForwardIterator e) {
  return minmax_element(b, e, less<void>());
}

template<typename ForwardIterator, typename Predicate>
pair<ForwardIterator, ForwardIterator> minmax_element(ForwardIterator b,
                                                      ForwardIterator e,
                                                      Predicate predicate) {
  pair<ForwardIterator, ForwardIterator> rv = make_pair(b, b);
  if (b == e) return rv;

  while (++b != e) {
    if (predicate(*rv.second, *b))
      rv.second = b;
    else if (predicate(*b, *rv.first))
      rv.first = b;
  }
  return rv;
}


template<typename InputIterator1, typename InputIterator2>
bool lexicographical_compare(InputIterator1 b1, InputIterator1 e1,
                             InputIterator2 b2, InputIterator2 e2) {
  return lexicographical_compare(b1, e1, b2, e2, less<void>());
}

template<typename InputIterator1, typename InputIterator2, typename Compare>
bool lexicographical_compare(InputIterator1 b1, InputIterator1 e1,
                             InputIterator2 b2, InputIterator2 e2,
                             Compare comp) {
  while (b1 != e1 && b2 != e2) {
    if (comp(*b1, *b2)) return true;
    if (comp(*b2, *b1)) return false;
    ++b1;
    ++b2;
  }
  return (b1 == e1 && b2 != e2);
}


template<typename BidirectionalIterator>
bool next_permutation(BidirectionalIterator b, BidirectionalIterator e) {
  return next_permutation(b, e, less<void>());
}

template<typename BidirectionalIterator, typename Compare>
bool next_permutation(BidirectionalIterator b, BidirectionalIterator e,
                      Compare comp) {
  if (b == e || next(b) == e) return false;

  BidirectionalIterator k, next_of_k, l;

  /*
   * Find the largest k, such that array[k] < array[k + 1].
   *
   * If no such index exists, this is the last permutation.
   * (Note that if this is the last permutation, reversing the sequence
   * will sort it.)
   */
  for (next_of_k = prev(e);
       next_of_k != b && !comp(*prev(next_of_k), *next_of_k);
       --next_of_k);
  if (next_of_k == b) {
    reverse(b, e);
    return false;
  }
  k = prev(next_of_k);

  /*
   * Find the largest l, suck that array[k] < array[l].
   *
   * Note that we know there exists at least one element for which
   * array[k] < array[l] holds: k+1.
   */
  for (l = prev(e);
       l != next_of_k && !comp(*k, *l);
       --l);

  /* Swap array[k] with array[l]. */
  iter_swap(k, l);

  /* Reverse the sequence array[k + 1 .. e]. */
  reverse(next_of_k, e);

  return true;
}


_namespace_end(std)

#endif /* _ALGORITHM_INL_H_ */
