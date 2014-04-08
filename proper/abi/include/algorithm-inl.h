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
