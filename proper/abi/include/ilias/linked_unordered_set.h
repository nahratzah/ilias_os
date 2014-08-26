#ifndef _ILIAS_LINKED_UNORDERED_SET_H_
#define _ILIAS_LINKED_UNORDERED_SET_H_

#include <cdecl.h>
#include <functional>
#include <iterator>
#include <memory>
#include <type_traits>
#include <tuple>
#include <vector>
#include <ilias/linked_forward_list.h>
#include <cstdint>
#include <stdimpl/heap_array.h>

_namespace_begin(ilias)
namespace impl {

template<class T> struct linked_unordered_set_tag {};

/*
 * Ceil function, for use by linked unordered set.
 */
uint64_t ceil_uls_(long double, uint64_t = UINT64_MAX) noexcept;


/*
 * Type agnostic algorithms for basic_linked_unordered_set.
 */
struct basic_lu_set_algorithms {
 public:
  using iterator = basic_linked_forward_list::iterator;
  using pred_array_type = _namespace(std)::impl::heap_array<iterator>;

  static iterator find_predecessor(const iterator*, const iterator*,
                                   iterator, size_t, iterator) noexcept;
  template<typename Pred>
  static iterator find_predecessor_for_link(const iterator*, const iterator*,
                                            size_t, Pred, iterator, iterator);

  static void update_on_link(iterator*, iterator*, iterator, size_t) noexcept;
  static void update_on_unlink(iterator*, iterator*, iterator, size_t)
      noexcept;

 private:
  template<typename Hash>
  static bool rehash_completely(iterator*, iterator*, Hash, iterator, iterator)
      noexcept;
  static bool rehash_shrink(iterator*, iterator*, iterator*,
                                 iterator, iterator) noexcept;

 public:
  template<typename Hash>
  static void rehash(iterator*, iterator*, iterator*, Hash, iterator, iterator)
      noexcept;

 private:
  static void rehash_splice_operation_(iterator*, iterator*,
                                       iterator, iterator,
                                       size_t, iterator, iterator,
                                       pred_array_type&, iterator) noexcept;

 public:
  static void on_move(iterator*, iterator*, iterator, iterator) noexcept;
};

} /* namespace ilias::impl */


template<typename Allocator>
class basic_linked_unordered_set {
 protected:
  using iterator = impl::basic_lu_set_algorithms::iterator;
  using bucket_set = _namespace(std)::vector<
      iterator,
      typename _namespace(std)::allocator_traits<Allocator>::
          template rebind_alloc<iterator>>;

 public:
  using bucket_size_type =
      _namespace(std)::conditional_t<(sizeof(size_t) <
                                      sizeof(typename bucket_set::size_type)),
                                     size_t,
                                     typename bucket_set::size_type>;
  using allocator_type = Allocator;

 protected:
  basic_linked_unordered_set(
      bucket_size_type, iterator, const allocator_type& = allocator_type());
  explicit basic_linked_unordered_set(
      const allocator_type& = allocator_type());

  basic_linked_unordered_set(const basic_linked_unordered_set&) = delete;
  basic_linked_unordered_set(basic_linked_unordered_set&&);
  basic_linked_unordered_set& operator=(
      const basic_linked_unordered_set&) = delete;
  basic_linked_unordered_set& operator=(
      basic_linked_unordered_set&&) = delete;
  basic_linked_unordered_set(basic_linked_unordered_set&&, iterator, iterator)
      noexcept;
  void swap(basic_linked_unordered_set&, iterator, iterator) noexcept;

  iterator begin_(bucket_size_type, iterator) const noexcept;
  iterator end_(bucket_size_type, iterator) const noexcept;

  iterator find_predecessor(iterator, bucket_size_type, iterator)
      const noexcept;
  template<typename Pred>
  iterator find_predecessor_for_link(bucket_size_type, Pred,
                                     iterator, iterator) const;

  void update_on_link(iterator, bucket_size_type) noexcept;
  void update_on_unlink(iterator, bucket_size_type) noexcept;
  void update_on_unlink_all(iterator) noexcept;

  template<typename Hash>
  void rehash(bucket_size_type, Hash, iterator, iterator);
  template<typename SizeT, typename Hash>
  void rehash_grow(SizeT, float, Hash, iterator, iterator,
                   bucket_size_type = 6, bucket_size_type = 5);
  template<typename SizeT, typename Hash>
  void rehash_shrink(SizeT, float, Hash, iterator, iterator,
                     bucket_size_type = 1, bucket_size_type = 3) noexcept;

  float load_factor_for_size(size_t) const noexcept;

 public:
  bucket_size_type bucket_count() const noexcept;
  bucket_size_type max_bucket_count() const noexcept;
  allocator_type get_allocator() const;

 private:
  bucket_set buckets_;
};


template<class Tag = void> using linked_unordered_set_element =
    linked_forward_list_element<impl::linked_unordered_set_tag<Tag>>;

template<typename T, class Tag = void,
         typename Hash = _namespace(std)::hash<T>,
         typename Pred = _namespace(std)::equal_to<T>,
         typename Allocator = _namespace(std)::allocator<void>>
class linked_unordered_set
: private linked_forward_list<T, impl::linked_unordered_set_tag<Tag>>,
  private basic_linked_unordered_set<Allocator>
{
 private:
  using list_type = linked_forward_list<T,
                                        impl::linked_unordered_set_tag<Tag>>;

 public:
  using value_type = typename list_type::value_type;
  using reference = typename list_type::reference;
  using const_reference = typename list_type::const_reference;
  using pointer = typename list_type::pointer;
  using const_pointer = typename list_type::const_pointer;

  using iterator = typename list_type::iterator;
  using const_iterator = typename list_type::const_iterator;
  using local_iterator = iterator;
  using const_local_iterator = const_iterator;

  using difference_type =
      typename _namespace(std)::iterator_traits<iterator>::difference_type;
  using size_type = _namespace(std)::make_unsigned_t<difference_type>;
  using allocator_type = Allocator;
  using hasher = Hash;
  using key_equal = Pred;

  using bucket_size_type =
      typename basic_linked_unordered_set<Allocator>::bucket_size_type;

  explicit linked_unordered_set(bucket_size_type = 0,
                                const hasher& = hasher(),
                                const key_equal& = key_equal(),
                                const allocator_type& = allocator_type());
  explicit linked_unordered_set(const allocator_type&);
  linked_unordered_set(linked_unordered_set&&) noexcept;
  linked_unordered_set& operator=(linked_unordered_set&&) noexcept;
  linked_unordered_set(const linked_unordered_set&) = delete;
  linked_unordered_set& operator=(const linked_unordered_set&) = delete;

  _namespace(std)::pair<iterator, bool> link(pointer, bool, bool = false);
  pointer unlink(const_iterator) noexcept;
  pointer unlink(const_pointer) noexcept;

  void unlink_all() noexcept;
  template<typename Visitor> void unlink_all(Visitor) noexcept;

  using basic_linked_unordered_set<Allocator>::bucket_count;
  using basic_linked_unordered_set<Allocator>::max_bucket_count;
  using list_type::empty;
  size_type size() const noexcept;
  size_type max_size() const noexcept;

  float load_factor() const noexcept;
  float max_load_factor() const noexcept;
  void max_load_factor(float);
  void rehash(bucket_size_type);
  void reserve(size_type);

  template<typename K> bucket_size_type bucket(const K&) const;
  hasher hash_function() const;
  key_equal key_eq() const;
  using basic_linked_unordered_set<Allocator>::get_allocator;

  using list_type::begin;
  using list_type::end;
  using list_type::cbegin;
  using list_type::cend;

  iterator begin(bucket_size_type) noexcept;
  const_iterator begin(bucket_size_type) const noexcept;
  const_iterator cbegin(bucket_size_type) const noexcept;
  iterator end(bucket_size_type) noexcept;
  const_iterator end(bucket_size_type) const noexcept;
  const_iterator cend(bucket_size_type) const noexcept;

  template<typename K> iterator find(const K&);
  template<typename K> const_iterator find(const K&) const;
  template<typename K> iterator lower_bound(const K&);
  template<typename K> const_iterator lower_bound(const K&) const;
  template<typename K> iterator upper_bound(const K&);
  template<typename K> const_iterator upper_bound(const K&) const;
  template<typename K> _namespace(std)::pair<iterator, iterator>
      equal_range(const K&);
  template<typename K> _namespace(std)::pair<const_iterator, const_iterator>
      equal_range(const K&) const;
  template<typename K> size_type count(const K&) const;

  void swap(linked_unordered_set&) noexcept;

  using list_type::nonconst_iterator;

 private:
  _namespace(std)::tuple<hasher, key_equal, float> params_;
  size_type size_ = 0;
};

template<typename T, class Tag, typename Hash, typename Pred, typename A>
void swap(linked_unordered_set<T, Tag, Hash, Pred, A>&,
          linked_unordered_set<T, Tag, Hash, Pred, A>&) noexcept;

template<typename T, class Tag, typename Hash, typename Pred, typename A>
bool operator==(const linked_unordered_set<T, Tag, Hash, Pred, A>& x,
                const linked_unordered_set<T, Tag, Hash, Pred, A>& y);
template<typename T, class Tag, typename Hash, typename Pred, typename A>
bool operator!=(const linked_unordered_set<T, Tag, Hash, Pred, A>& x,
                const linked_unordered_set<T, Tag, Hash, Pred, A>& y);


_namespace_end(ilias)

#include <ilias/linked_unordered_set-inl.h>

#endif /* _ILIAS_LINKED_UNORDERED_SET_H_ */
