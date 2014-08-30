#ifndef _ILIAS_NESTED_ITERATOR_H_
#define _ILIAS_NESTED_ITERATOR_H_

#include <cdecl.h>
#include <iterator>
#include <type_traits>
#include <utility>
#include <ilias/fixed_size_collection.h>

_namespace_begin(ilias)


/* Forward declaration. */
template<typename Outer,
         typename = decltype(_namespace(std)::declval<Outer>()->begin())>
class nested_iterator;


namespace impl {


/*
 * Calculate the size of the nested collection, used by a nested_iterator.
 */
template<typename Outer, typename Inner>
struct _nested_collection_size {
 private:
  using outer_value_type = _namespace(std)::remove_cv_t<
      typename _namespace(std)::iterator_traits<Outer>::value_type>;

 public:
  static constexpr size_t value =
      fixed_size_collection<outer_value_type>::value;
};

/*
 * Specialization, for nested iterator iterating a nested iterator.
 */
template<typename Outer, typename N0, typename N1>
struct _nested_collection_size<Outer, nested_iterator<N0, N1>> {
 private:
  using outer_value_type = _namespace(std)::remove_cv_t<
      typename _namespace(std)::iterator_traits<Outer>::value_type>;

 public:
  static constexpr size_t value =
      fixed_size_collection<outer_value_type>::value *
      _nested_collection_size<N0, N1>::value;
};

template<typename Outer, typename Inner> using nested_collection_size =
    _namespace(std)::integral_constant<
        size_t, _nested_collection_size<Outer, Inner>::value>;


template<typename Outer, typename... Nested>
struct nested_iterator_tag {
  using type = typename nested_iterator_tag<
      Outer, typename nested_iterator_tag<Nested...>::type>::type;
};

template<typename Outer, typename Inner>
struct nested_iterator_tag<Outer, Inner> {
  using type = _namespace(std)::common_type_t<
      typename nested_iterator_tag<Inner>::type,
      typename nested_iterator_tag<Outer>::type>;
};

template<typename Tag>
struct nested_iterator_tag<Tag> {
  static_assert(_namespace(std)::is_base_of<
                    _namespace(std)::input_iterator_tag, Tag>::value,
                "Iterator must be an input iterator.  "
                "(Not sure what you're trying to accomplish here...)");

  using type = Tag;
};

template<typename... Tags> using nested_iterator_tag_t =
    typename nested_iterator_tag<Tags...>::type;


} /* namespace ilias::impl */


/*
 * Nested iterator.
 *
 * This iterator adapter takes an iterator to a collection.
 * The iterator will iterate over each element in the derived collections.
 *
 * The value type T of the outer iterator must provide:
 * - begin(T&),
 * - end(T&),
 * - begin(const T&),
 * - end(const T&),
 * which will be located using ADL.
 */
template<typename Outer, typename Inner>
class nested_iterator
: public _namespace(std)::iterator<
    impl::nested_iterator_tag_t<
        _namespace(std)::conditional_t<
            (impl::nested_collection_size<Outer, Inner>::value == 0),
            typename _namespace(std)::bidirectional_iterator_tag,
            typename _namespace(std)::random_access_iterator_tag>,
        typename _namespace(std)::iterator_traits<Outer>::iterator_category,
        typename _namespace(std)::iterator_traits<Inner>::iterator_category>,
    typename _namespace(std)::iterator_traits<Inner>::value_type,
    _namespace(std)::common_type_t<
        typename _namespace(std)::iterator_traits<Outer>::difference_type,
        typename _namespace(std)::iterator_traits<Inner>::difference_type>,
    typename _namespace(std)::iterator_traits<Inner>::pointer,
    typename _namespace(std)::iterator_traits<Inner>::reference>
{
  template<typename, typename> friend class nested_iterator;

 private:
  using __iterator_spec = _namespace(std)::iterator<
      impl::nested_iterator_tag_t<
        _namespace(std)::conditional_t<
            (impl::nested_collection_size<Outer, Inner>::value == 0),
            typename _namespace(std)::bidirectional_iterator_tag,
            typename _namespace(std)::random_access_iterator_tag>,
          typename _namespace(std)::iterator_traits<Outer>::iterator_category,
          typename _namespace(std)::iterator_traits<Inner>::iterator_category>,
      typename _namespace(std)::iterator_traits<Inner>::value_type,
      _namespace(std)::common_type_t<
          typename _namespace(std)::iterator_traits<Outer>::difference_type,
          typename _namespace(std)::iterator_traits<Inner>::difference_type>,
      typename _namespace(std)::iterator_traits<Inner>::pointer,
      typename _namespace(std)::iterator_traits<Inner>::reference>;

  struct enable_tag {};

 public:
  using outer_iterator_type = Outer;
  using inner_iterator_type = Inner;

  using difference_type = typename __iterator_spec::difference_type;
  using pointer = typename __iterator_spec::pointer;
  using reference = typename __iterator_spec::reference;

  nested_iterator() = default;
  nested_iterator(const nested_iterator&) = default;
  nested_iterator& operator=(const nested_iterator&) = default;

  explicit nested_iterator(outer_iterator_type);
  nested_iterator(outer_iterator_type, inner_iterator_type);

  template<typename T, typename U> nested_iterator(
      const nested_iterator<T, U>&,
      _namespace(std)::enable_if_t<
          (_namespace(std)::is_convertible<T, Outer>::value &&
           _namespace(std)::is_convertible<U, Inner>::value),
          enable_tag> = enable_tag());

  nested_iterator& operator++();
  nested_iterator& operator--();
  nested_iterator operator++(int);
  nested_iterator operator--(int);

  nested_iterator operator+(difference_type) const;
  nested_iterator operator-(difference_type) const;
  nested_iterator& operator+=(difference_type);
  nested_iterator& operator-=(difference_type);

  difference_type operator-(const nested_iterator&) const;
  reference operator[](difference_type) const;

  reference operator*() const;
  pointer operator->() const;

  template<typename T, typename U>
  bool operator==(const nested_iterator<T, U>&) const;
  template<typename T, typename U>
  bool operator!=(const nested_iterator<T, U>&) const;

 private:
  void fixup_() const;

  mutable outer_iterator_type outer_;
  mutable inner_iterator_type inner_;
  mutable bool inner_lazy_ = true;  // inner_ = outer_.begin()
};


_namespace_end(ilias)

#include <ilias/nested_iterator-inl.h>

#endif /* _ILIAS_NESTED_ITERATOR_H_ */
