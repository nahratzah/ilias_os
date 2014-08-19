#ifndef _ILIAS_LINKED_UNORDERED_SET_H_
#define _ILIAS_LINKED_UNORDERED_SET_H_

#include <cdecl.h>
#include <functional>
#include <iterator>
#include <memory>
#include <type_traits>
#include <vector>
#include <ilias/linked_list.h>

_namespace_begin(ilias)
namespace impl {


template<class T> struct linked_unordered_set_tag {};


} /* namespace ilias::impl */


template<class Tag = void> using linked_unordered_set_element =
    linked_list_element<impl::linked_unordered_set_tag<Tag>>;

template<typename T, class Tag = void,
         typename Hash = _namespace(std)::hash<T>,
         typename Pred = _namespace(std)::equal_to<T>,
         typename Allocator = _namespace(std)::allocator<void>>
class linked_unordered_set {
 private:
  // XXX: change to singly linked list (once that is implemented)
  using list_type = linked_list<T, impl::linked_unordered_set_tag<T>>;

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

  using difference_type = typename iterator_traits<iterator>::difference_type;
  using size_type = make_unsigned_t<difference_type>;
  using allocator_type = Allocator;
  using hasher = Hash;
  using key_equal = Pred;

 private:
  using bucket_set = _namespace(std)::vector<
      iterator,
      typename allocator_traits<allocator_type>::
          template rebind_alloc<iterator>>;

 public:
  using bucket_size_type =
      conditional_t<sizeof(size_t) < sizeof(typename bucket_set::size_type),
                    size_t,
                    typename bucket_set::size_type>;

  linked_unordered_set();

  _namespace(std)::pair<iterator, bool> link(pointer, bool);
  pointer unlink(const_iterator) noexcept;
  pointer unlink(const_pointer) noexcept;

  bucket_size_type bucket_count() const noexcept;
  bucket_size_type max_bucket_count() const noexcept;
  bool empty() const noexcept;
  size_type size() const noexcept;
  size_type max_size() const noexcept;

  float load_factor() const noexcept;
  float max_load_factor() const noexcept;
  void max_load_factor(float);
  void rehash(bucket_size_type);
  void reserve(bucket_size_type);

  template<typename K> bucket_size_type bucket(const K&) const;
  hasher hash_function() const;
  key_equal key_eq() const;
  allocator_type get_allocator();

  iterator begin() noxcept;
  const_iterator begin() const noxcept;
  const_iterator cbegin() const noxcept;
  iterator end() noxcept;
  const_iterator end() const noxcept;
  const_iterator cend() const noxcept;

  iterator begin(bucket_size_type) noxcept;
  const_iterator begin(bucket_size_type) const noxcept;
  const_iterator cbegin(bucket_size_type) const noxcept;
  iterator end(bucket_size_type) noxcept;
  const_iterator end(bucket_size_type) const noxcept;
  const_iterator cend(bucket_size_type) const noxcept;

 private:
  list_type list_;
  tuple<hasher, key_equal, bucket_set, float> params_;
  size_type size_ = 0;
};


_namespace_end(ilias)

#include <ilias/linked_unordered_set-inl.h>

#endif /* _ILIAS_LINKED_UNORDERED_SET_H_ */
