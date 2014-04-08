_namespace_begin(std)


template<typename InputIterator, class Predicate>
bool all_of(InputIterator b, InputIterator e, Predicate predicate) {
  while (b != e) {
    if (!predicate(*b)) return false;
    ++b;
  }
  return true;
}

template<typename InputIterator, class Predicate>
bool any_of(InputIterator b, InputIterator e, Predicate predicate) {
  while (b != e) {
    if (predicate(*b)) return true;
    ++b;
  }
  return false;
}

template<typename InputIterator, class Predicate>
bool none_of(InputIterator b, InputIterator e, Predicate predicate) {
  while (b != e) {
    if (predicate(*b)) return false;
    ++b;
  }
  return true;
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
  while (b != e && !(*b == v)) ++b;
  return b;
}

template<typename InputIterator, typename Predicate>
InputIterator find_if(InputIterator b, InputIterator e, Predicate predicate) {
  while (b != e && !predicate(*b)) ++b;
  return b;
}

template<typename InputIterator, typename Predicate>
InputIterator find_if_not(InputIterator b, InputIterator e,
                          Predicate predicate) {
  while (b != e && predicate(*b)) ++b;
  return b;
}

template<typename ForwardIterator1, typename ForwardIterator2>
ForwardIterator1 find_end(ForwardIterator1 haystack_b,
                          ForwardIterator1 haystack_e,
                          ForwardIterator2 needle_b,
                          ForwardIterator2 needle_e) {
  ForwardIterator1 match = haystack_e;

  while (haystack_b != haystack_e) {
    ForwardIterator1 haystack_i = haystack_b;
    ForwardIterator2 needle_i = needle_b;

    while (needle_i != needle_e && haystack_i != haystack_e &&
           *haystack_i == *needle_i) {
      ++needle_i;
      ++needle_e;
    }
    if (needle_i == needle_e) match = haystack_b;
    if (haystack_i == haystack_e) break;
  }
  return match;
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
  }
  return match;
}

template<typename InputIterator, typename ForwardIterator>
InputIterator find_first_of(InputIterator haystack_b,
                            InputIterator haystack_e,
                            ForwardIterator needles_b,
                            ForwardIterator needles_e) {
  while (haystack_b != haystack_e &&
         !find(needles_b, needles_e, *haystack_b))
    ++haystack_b;
  return haystack_b;
}

template<typename InputIterator, typename ForwardIterator,
         typename BinaryPredicate>
InputIterator find_first_of(InputIterator haystack_b,
                            InputIterator haystack_e,
                            ForwardIterator needles_b,
                            ForwardIterator needles_e,
                            BinaryPredicate predicate) {
  while (haystack_b != haystack_e) {
    ForwardIterator i;
    for (i = needles_b;
         i != needles_e && !predicate(*haystack_b, *i);
         ++i);
    if (i != needles_e) break;
    ++haystack_b;
  }
  return haystack_b;
}

template<typename ForwardIterator>
ForwardIterator adjecent_find(ForwardIterator b, ForwardIterator e) {
  if (b == e) return e;
  ForwardIterator b_succ = next(b);

  while (b_succ != e) {
    if (*b == *b_succ) return b;

    /* ++iterators... */
    b = b_succ;
    ++b_succ;
  }
  return e;
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
  typename iterator_traits<InputIterator>::difference_type rv = 0;

  while (b != e) {
    if (*b == v) ++rv;
    ++b;
  }
  return rv;
}

template<typename InputIterator, typename Predicate>
typename iterator_traits<InputIterator>::difference_type count(
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
  while (b1 != e1 && *b1 == *b2) {
    ++b1;
    ++b2;
  }
  return make_pair(b1, b2);
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
  while (b1 != e1 && b2 != e2 && *b1 == *b2) {
    ++b1;
    ++b2;
  }
  return make_pair(b1, b2);
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
auto equal_support__distance(Iter b, Iter e,
                             Tail&&...) ->
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
  while (b1 != e1 && *b1 == *b2) {
    ++b1;
    ++b2;
  }
  return b1 == e1;
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
  if (impl::unequal_distance(b1, e1, b2, e2)) return false;

  while (b1 != e1 && b2 != e2 && *b1 == *b2) {
    ++b1;
    ++b2;
  }
  return b1 == e1 && b2 == e2;
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
  using impl::simple_bitset;

  /* First eliminate anything that is equal. */
  while (b1 != e1 && *b1 == *b2) {
    ++b1;
    ++b2;
  }
  if (b1 == e1) return true;  // Everything is equal.

  /* Use a collection to find the remaining items. */
  simple_bitset::size_type count;
  for (auto i = b1; i != e1; ++i) ++count;
  simple_bitset s{ count };

  while (count > 0) {
    auto found = b1;
    simple_bitset::size_type off = 0;
    while (found != e1 && (s[off] || !(*found == *b2))) {
      ++found;
      ++off;
    }
    if (found == e1) return false;
    s[off] = true;

    ++b2;
    --count;
  }
  return true;
}

template<typename ForwardIterator1, typename ForwardIterator2,
         typename BinaryPredicate>
bool is_permutation(ForwardIterator1 b1, ForwardIterator1 e1,
                    ForwardIterator2 b2,
                    BinaryPredicate predicate) {
  using impl::simple_bitset;

  /* First eliminate anything that is equal. */
  while (b1 != e1 && predicate(*b1, *b2)) {
    ++b1;
    ++b2;
  }
  if (b1 == e1) return true;  // Everything is equal.

  /* Use a collection to find the remaining items. */
  simple_bitset::size_type count;
  for (auto i = b1; i != e1; ++i) ++count;
  simple_bitset s{ count };

  while (count > 0) {
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
  return true;
}

template<typename ForwardIterator1, typename ForwardIterator2>
bool is_permutation(ForwardIterator1 b1, ForwardIterator1 e1,
                    ForwardIterator2 b2, ForwardIterator2 e2) {
  using impl::simple_bitset;

  if (impl::unequal_distance(b1, e1, b2, e2)) return false;

  /* First eliminate anything that is equal. */
  while (b1 != e1 && b2 != e2 && *b1 == *b2) {
    ++b1;
    ++b2;
  }
  if (b1 == e1 && b2 == e2) return true;  // Everything is equal.
  if (b1 == e1 || b2 == e2) return false;  // Unequal length.

  /* Use a collection to find the remaining items. */
  simple_bitset::size_type count;
  for (auto i = b1; i != e1; ++i) ++count;
  simple_bitset s{ count };

  while (count > 0 && b2 != e2) {
    auto found = b1;
    simple_bitset::size_type off = 0;
    while (found != e1 && (s[off] || !(*found == *b2))) {
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

template<typename ForwardIterator1, typename ForwardIterator2,
         typename BinaryPredicate>
bool is_permutation(ForwardIterator1 b1, ForwardIterator1 e1,
                    ForwardIterator2 b2, ForwardIterator2 e2,
                    BinaryPredicate predicate) {
  using impl::simple_bitset;

  if (impl::unequal_distance(b1, e1, b2, e2)) return false;

  /* First eliminate anything that is equal. */
  while (b1 != e1 && b2 != e2 && predicate(*b1, *b2)) {
    ++b1;
    ++b2;
  }
  if (b1 == e1 && b2 == e2) return true;  // Everything is equal.
  if (b1 == e1 || b2 == e2) return false;  // Unequal length.

  /* Use a collection to find the remaining items. */
  simple_bitset::size_type count;
  for (auto i = b1; i != e1; ++i) ++count;
  simple_bitset s{ count };

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

template<typename ForwardIterator1, typename ForwardIterator2>
ForwardIterator1 search(ForwardIterator1 haystack_b,
                        ForwardIterator1 haystack_e,
                        ForwardIterator2 needle_b,
                        ForwardIterator2 needle_e) {
  while (haystack_b != haystack_e) {
    ForwardIterator1 haystack_i = haystack_b;
    ForwardIterator2 needle_i = needle_b;

    while (needle_i != needle_e && haystack_i != haystack_e &&
           *haystack_i == *needle_i) {
      ++needle_i;
      ++needle_e;
    }
    if (needle_i == needle_e) return haystack_b;
    if (haystack_i == haystack_e) break;
  }
  return haystack_e;
}

template<typename ForwardIterator1, typename ForwardIterator2,
         typename BinaryPredicate>
ForwardIterator1 search(ForwardIterator1 haystack_b,
                        ForwardIterator1 haystack_e,
                        ForwardIterator2 needle_b,
                        ForwardIterator2 needle_e,
                        BinaryPredicate predicate) {
  while (haystack_b != haystack_e) {
    ForwardIterator1 haystack_i = haystack_b;
    ForwardIterator2 needle_i = needle_b;

    while (needle_i != needle_e && haystack_i != haystack_e &&
           predicate(*haystack_i, *needle_i)) {
      ++needle_i;
      ++needle_e;
    }
    if (needle_i == needle_e) return haystack_b;
    if (haystack_i == haystack_e) break;
  }
  return haystack_e;
}

template<typename ForwardIterator, class Size, typename T>
ForwardIterator search_n(ForwardIterator b, ForwardIterator e,
                         Size n, const T& v) {
  ForwardIterator match;
  Size c = 0;
  while (b != e) {
    if (c == 0) match = b;
    if (c == n) return match;

    if (*b == v)
      ++c;
    else
      c = 0;

    ++b;
  }
  return e;
}

template<typename ForwardIterator, class Size, typename T,
         typename BinaryPredicate>
ForwardIterator search_n(ForwardIterator b, ForwardIterator e,
                         Size n, const T& v,
                         BinaryPredicate predicate) {
  ForwardIterator match;
  Size c = 0;
  while (b != e) {
    if (c == 0) match = b;
    if (c == n) return match;

    if (predicate(*b, v))
      ++c;
    else
      c = 0;

    ++b;
  }
  return e;
}


/* Some specializations for copy. */
namespace impl {

template<typename T>
auto algorithm_copy(const T* b, const T* e, T* out) ->
    enable_if_t<is_trivially_copyable<T>::value, T> {
  const auto delta = e - b;
  memcpy(out, b, delta * sizeof(T));
  return out + delta;
}

template<typename T>
auto algorithm_copy(T* b, T* e, T* out) ->
    enable_if_t<is_trivially_copyable<T>::value, T> {
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
  return impl::algorithm_copy(move(b), move(e), move(out));
}

/* Some specializations for copy_n. */
namespace impl {

template<typename T, typename Size>
auto algorithm_copy_n(const T* b, Size n, T* out) ->
    enable_if_t<is_trivially_copyable<T>::value, T> {
  memcpy(out, b, n * sizeof(T));
  return out + n;
}

template<typename T, typename Size>
auto algorithm_copy_n(T* b, Size n, T* out) ->
    enable_if_t<is_trivially_copyable<T>::value, T> {
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
  return impl::algorithm_copy_n(move(b), move(n), move(out));
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
    enable_if_t<is_trivially_copyable<T>::value, T> {
  const auto delta = e - b;
  memcpy(out, b, delta * sizeof(T));
  return out + delta;
}

template<typename T>
auto algorithm_move(T* b, T* e, T* out) ->
    enable_if_t<is_trivially_copyable<T>::value, T> {
  const auto delta = e - b;
  memcpy(out, b, delta * sizeof(T));
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
  return impl::algorithm_move(move(b), move(e), move(out));
}

template<typename BidirectionalIterator1, typename BidirectionalIterator2>
BidirectionalIterator2 move_backward(BidirectionalIterator1 b,
                                     BidirectionalIterator1 e,
                                     BidirectionalIterator2 out) {
  while (b != e) *--out = move(*--e);
  return out;
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
  b = find(move(b), e, v);
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
  return unique_copy(move(b), move(e), move(out), move(predicate),
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
  return unique_copy(move(b), move(e), move(out), impl::unique_cmp());
}

template<typename InputIterator, typename OutputIterator,
         typename BinaryPredicate>
OutputIterator unique_copy(InputIterator b, InputIterator e,
                           OutputIterator out,
                           BinaryPredicate predicate) {
  return impl::algorithm_unique_copy(
      move(b), move(e), move(out), move(predicate),
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
  if (first == middle) return last;  // Rotate 0 elements.

  const auto shift = distance(first, middle);
  auto len = shift;
  for (ForwardIterator i = middle; i != last; ++first, ++i, ++len)
    iter_swap(first, i);

  /*
   * This recursion is bounded:
   * it will be swapping at most last-first elements, decreasing at each step.
   */
  const auto rotate_right = len % shift;
  if (rotate_right != 0) {
    auto rotate_left = shift - rotate_right;
    // XXX try to eliminate the recursion
    rotate(first, next(first, rotate_left), last);
  }

  return first;
}

template<typename ForwardIterator, typename OutputIterator>
OutputIterator rotate_copy(ForwardIterator b, ForwardIterator middle,
                           ForwardIterator e,
                           OutputIterator out) {
  for (ForwardIterator i = middle; i != e; ++i) {
    *out = *i;
    ++out;
  }

  while (b != middle) {
    *out = *b;
    ++b;
    ++out;
  }

  return out;
}

template<typename InputIterator, typename Predicate>
bool is_partitioned(InputIterator b, InputIterator e, Predicate predicate) {
  b = find_if_not(b, e, predicate);
  b = find_if(b, e, move(predicate));
  return b == e;
}

template<typename ForwardIterator, typename Predicate>
ForwardIterator partition(ForwardIterator b, ForwardIterator e,
                          Predicate predicate) {
  b = find_if_not(b, e, predicate);
  if (b == e) return b;

  ForwardIterator i = next(b);
  while ((i = find_if(i, e, predicate)) != e)
    iter_swap(b++, i++);
  return b;
}

/* Support for stable_partition. */
namespace impl {

/* stable partition, but requires that *b is true. */
template<typename BidirectionalIterator, typename Predicate>
BidirectionalIterator stable_partition_inplace(
    BidirectionalIterator b, BidirectionalIterator e, Predicate predicate) {
  if (b == e) return b;
  assert(predicate(b) == false);

  BidirectionalIterator middle = find_if(next(b), e, predicate);

  if (middle != e) {
    /*
     * To meet the assert above during recursion, we need an element for which
     * predicate yields false.
     */
    BidirectionalIterator next_fail = find_if_not(next(middle), e, predicate);
    e = stable_partition_inplace(move(next_fail), e, move(predicate));
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
  /* Tiny class that provides temporary array storage. */
  class heap_array {
   public:
    using value_type =
        typename iterator_traits<BidirectionalIterator>::value_type;
    using pointer = value_type*;
    using iterator = pointer;

    static constexpr size_t vt_size() { return sizeof(value_type); };

    heap_array(size_t n)
    : max_size_(n),
      ptr_(::operator new(n * vt_size(), nothrow))
    {}

    ~heap_array() noexcept {
      if (*this) {
        while (size_-- > 0)
          ptr_[size_].~value_type();
        ::operator delete(ptr_);
      }
    }

    explicit operator bool() const noexcept { return ptr_; }

    iterator begin() const noexcept { return ptr_; }
    iterator end() const noexcept { return begin() + size_; }

    void emplace_back(value_type&& v) {
      assert(size_ < max_size_);
      new (&ptr_[size_]) value_type(forward<value_type>(v));
      ++size_;
    }

   private:
    const size_t max_size_;
    size_t size_ = 0;
    const pointer ptr_;
  };

  /* Skip all elements that are in the correct position already. */
  b = find_if_not(b, e, predicate);
  if (b == e) return make_tuple(b, true);

  /*
   * Allocate array that is sufficiently large to hold elements that need
   * to be at the end of the range.  Worst case: all remaining elements.
   */
  heap_array array{ distance(b, e) };
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
  b = find_if_not(b, e, move(predicate));
  return make_tuple(b, b == e);
}

template<typename BidirectionalIterator, typename Predicate>
BidirectionalIterator stable_partition_impl(BidirectionalIterator b,
                                            BidirectionalIterator e,
                                            Predicate predicate) {
  bool succes;
  tie(b, succes) = stable_partition_using_heap(b, e, predicate);
  if (!succes) b = stable_partition_inplace(b, e, predicate);
  return b;
}

} /* namespace std::impl */

template<typename BidirectionalIterator, typename Predicate>
BidirectionalIterator stable_partition(BidirectionalIterator b,
                                       BidirectionalIterator e,
                                       Predicate predicate) {
  return impl::stable_partition_impl(move(b), move(e), move(predicate));
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
  assert(is_partitioned(b, e, predicate));
  return find_if_not(move(b), move(e), move(predicate));
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
  T* ptr = nullptr;
  for (T& i : il) {
    if (ptr == nullptr)
      ptr = &i;
    else
      ptr = &min(*ptr, i);
  }
  return *ptr;
}

template<typename T>
auto max(initializer_list<T> il) -> T {
  T* ptr = nullptr;
  for (T& i : il) {
    if (ptr == nullptr)
      ptr = &i;
    else
      ptr = &max(*ptr, i);
  }
  return *ptr;
}

template<typename T, typename Comp>
auto min(initializer_list<T> il, Comp cmp) -> T {
  T* ptr = nullptr;
  for (T& i : il) {
    if (ptr == nullptr)
      ptr = &i;
    else
      ptr = &min(*ptr, i, cmp);
  }
  return *ptr;
}

template<typename T, typename Comp>
auto max(initializer_list<T> il, Comp cmp) -> T {
  T* ptr = nullptr;
  for (T& i : il) {
    if (ptr == nullptr)
      ptr = &i;
    else
      ptr = &max(*ptr, i, cmp);
  }
  return *ptr;
}


_namespace_end(std)
