#ifndef _ILIAS_LINKED_FORWARD_LIST_H_
#define _ILIAS_LINKED_FORWARD_LIST_H_

#include <cdecl.h>
#include <iterator>
#include <tuple>

_namespace_begin(ilias)


/*
 * Basic algorithm of a singly linked list.
 *
 * Note that this implementation only supports mutable elements.
 *
 * Instead of using this class, which is more like an implementation
 * detail than anything else, consider using linked_forward_list<T>.
 */
class basic_linked_forward_list {
 public:
  class iterator;

  enum from_before_iter_t { from_before_iter };
  class ptr_iterator;

  class element {
    friend basic_linked_forward_list;
    friend basic_linked_forward_list::iterator;
    friend basic_linked_forward_list::ptr_iterator;

   protected:
    constexpr element() noexcept = default;
    constexpr element(const element&) noexcept;
    element& operator=(const element&) noexcept;

   private:
    element* succ_ = nullptr;
  };

 protected:
  constexpr basic_linked_forward_list() noexcept {}
  basic_linked_forward_list(basic_linked_forward_list&&) noexcept;
  basic_linked_forward_list(const basic_linked_forward_list&) = delete;
  basic_linked_forward_list& operator=(const basic_linked_forward_list&) =
      delete;

  void link_front(element*) noexcept;
  iterator link_after(iterator, element*) noexcept;
  element* unlink_front() noexcept;
  element* unlink_after(iterator) noexcept;
  element* unlink_after(element*) noexcept;
  element* unlink(iterator) noexcept;
  element* unlink(element*) noexcept;

 public:
  void splice_after(iterator, basic_linked_forward_list&) noexcept;
  static void splice_after(iterator,
                           iterator, iterator) noexcept;
  static _namespace(std)::tuple<ptr_iterator, ptr_iterator>
      splice(ptr_iterator, ptr_iterator, ptr_iterator) noexcept;
  template<typename Compare>
  static _namespace(std)::tuple<ptr_iterator, ptr_iterator>
      merge(ptr_iterator, ptr_iterator,
            ptr_iterator, ptr_iterator,
            Compare);

  template<typename Compare>
  void merge(basic_linked_forward_list&, Compare);

  static iterator iterator_to(element*) noexcept;

 private:
  template<typename Compare>
  static _namespace(std)::tuple<ptr_iterator, ptr_iterator>
      sort(ptr_iterator, ptr_iterator, Compare, size_t);
  template<typename Compare>
  static _namespace(std)::tuple<ptr_iterator, ptr_iterator>
      sort(ptr_iterator, ptr_iterator, Compare);
  template<typename Compare> void sort(Compare, size_t);

 public:
  template<typename Compare> void sort(Compare);

  iterator before_begin() const noexcept;
  iterator begin() const noexcept;
  iterator end() const noexcept;

  bool empty() const noexcept;
  void swap(basic_linked_forward_list&) noexcept;
  void reverse() noexcept;

 private:
  element*& find_succ_for_(element*) noexcept;

  element before_;
};

bool operator==(const basic_linked_forward_list::iterator&,
                const basic_linked_forward_list::iterator&) noexcept;
bool operator!=(const basic_linked_forward_list::iterator&,
                const basic_linked_forward_list::iterator&) noexcept;
void swap(basic_linked_forward_list&, basic_linked_forward_list&) noexcept;

class basic_linked_forward_list::iterator
: public _namespace(std)::iterator<_namespace(std)::forward_iterator_tag,
                                   element>
{
  friend basic_linked_forward_list;
  friend bool operator==(const basic_linked_forward_list::iterator&,
                         const basic_linked_forward_list::iterator&) noexcept;

 public:
  iterator() = default;
  iterator(const iterator&) = default;
  iterator& operator=(const iterator&) = default;

 private:
  explicit iterator(element*) noexcept;

 public:
  iterator& operator++() noexcept;
  iterator operator++(int) noexcept;

  element& operator*() const noexcept;
  element* operator->() const noexcept;

  bool operator==(const ptr_iterator&) const noexcept;
  bool operator!=(const ptr_iterator&) const noexcept;

 private:
  element* elem_ = nullptr;
};

class basic_linked_forward_list::ptr_iterator
: public _namespace(std)::iterator<_namespace(std)::forward_iterator_tag,
                                   element>
{
  friend basic_linked_forward_list;

 public:
  ptr_iterator() = default;
  ptr_iterator(const ptr_iterator&) = default;
  ptr_iterator& operator=(const ptr_iterator&) = default;

 private:
  explicit ptr_iterator(element*&) noexcept;

 public:
  ptr_iterator(basic_linked_forward_list&) noexcept;
  ptr_iterator(basic_linked_forward_list&,
               basic_linked_forward_list::iterator) noexcept;
  ptr_iterator(basic_linked_forward_list::iterator, from_before_iter_t);

  operator basic_linked_forward_list::iterator() const noexcept;

  ptr_iterator& operator++() noexcept;
  ptr_iterator operator++(int) noexcept;

  element& operator*() const noexcept;
  element* operator->() const noexcept;

  element*& get_ptr() const noexcept;

  bool operator==(const ptr_iterator&) const noexcept;
  bool operator!=(const ptr_iterator&) const noexcept;
  bool operator==(const basic_linked_forward_list::iterator&) const noexcept;
  bool operator!=(const basic_linked_forward_list::iterator&) const noexcept;

 private:
  element** ptr_ = nullptr;
};


template<typename, class = void> class linked_forward_list;

template<class Tag = void>
class linked_forward_list_element
: private basic_linked_forward_list::element
{
  template<typename, class> friend class linked_forward_list;

 protected:
  linked_forward_list_element() = default;
  linked_forward_list_element(const linked_forward_list_element&) = default;
  linked_forward_list_element& operator=(const linked_forward_list_element&) =
      default;
  ~linked_forward_list_element() noexcept = default;
};

template<typename T, class Tag>
class linked_forward_list
: private basic_linked_forward_list
{
 public:
  using value_type = T;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;

  class iterator;
  class const_iterator;

  void link_front(pointer) noexcept;
  iterator link_after(const_iterator, pointer) noexcept;
  pointer unlink_front() noexcept;
  pointer unlink_after(const_iterator) noexcept;
  pointer unlink_after(const_pointer) noexcept;
  pointer unlink(const_iterator) noexcept;
  pointer unlink(const_pointer) noexcept;

  reference front() noexcept;
  const_reference front() const noexcept;

  using basic_linked_forward_list::empty;

  void splice_after(const_iterator, linked_forward_list&&) noexcept;
  static void splice_after(const_iterator, const_iterator, const_iterator)
      noexcept;

  template<typename Compare> void merge(linked_forward_list&, Compare);
  void merge(linked_forward_list&);
  template<typename Compare> void sort(Compare);
  void sort();
  using basic_linked_forward_list::reverse;

  iterator before_begin() noexcept;
  iterator begin() noexcept;
  iterator end() noexcept;
  const_iterator before_begin() const noexcept;
  const_iterator begin() const noexcept;
  const_iterator end() const noexcept;
  const_iterator cbefore_begin() const noexcept;
  const_iterator cbegin() const noexcept;
  const_iterator cend() const noexcept;

  void swap(linked_forward_list&) noexcept;

  static iterator nonconst_iterator(const_iterator) noexcept;
  static iterator iterator_to(reference) noexcept;
  static const_iterator iterator_to(const_reference) noexcept;

 protected:
  static element* down_cast_(pointer) noexcept;
  static element* down_cast_(const_pointer) noexcept;
  static pointer up_cast_(element*) noexcept;
  static const_reference up_cast_cref_(const element&) noexcept;
};

template<typename T, class Tag>
class linked_forward_list<T, Tag>::iterator
: public _namespace(std)::iterator<
      basic_linked_forward_list::iterator::iterator_category, T>
{
  friend const_iterator;
  friend linked_forward_list;

 public:
  iterator() = default;
  iterator(const iterator&) = default;
  iterator& operator=(const iterator&) = default;
  iterator(iterator&&) = default;
  iterator& operator=(iterator&&) = default;
  explicit iterator(const basic_linked_forward_list::iterator&) noexcept;

  T& operator*() const noexcept;
  T* operator->() const noexcept;

  iterator& operator++() noexcept;
  iterator operator++(int) noexcept;

  bool operator==(const iterator&) const noexcept;
  bool operator!=(const iterator&) const noexcept;
  bool operator==(const const_iterator&) const noexcept;
  bool operator!=(const const_iterator&) const noexcept;

  basic_linked_forward_list::iterator get_unsafe_basic_iter() const noexcept;
  static iterator from_unsafe_basic_iter(
      const basic_linked_forward_list::iterator&) noexcept;

 private:
  basic_linked_forward_list::iterator impl_;
};

template<typename T, class Tag>
class linked_forward_list<T, Tag>::const_iterator
: public _namespace(std)::iterator<
      basic_linked_forward_list::iterator::iterator_category, const T>
{
  friend linked_forward_list;

 public:
  const_iterator() = default;
  const_iterator(const const_iterator&) = default;
  const_iterator& operator=(const const_iterator&) = default;
  const_iterator(const_iterator&&) = default;
  const_iterator& operator=(const_iterator&&) = default;
  explicit const_iterator(const basic_linked_forward_list::iterator&) noexcept;
  const_iterator(const iterator&) noexcept;

  const T& operator*() const noexcept;
  const T* operator->() const noexcept;

  const_iterator& operator++() noexcept;
  const_iterator operator++(int) noexcept;

  bool operator==(const iterator&) const noexcept;
  bool operator!=(const iterator&) const noexcept;
  bool operator==(const const_iterator&) const noexcept;
  bool operator!=(const const_iterator&) const noexcept;

  basic_linked_forward_list::iterator get_unsafe_basic_iter() const noexcept;
  static const_iterator from_unsafe_basic_iter(
      const basic_linked_forward_list::iterator&) noexcept;

 private:
  basic_linked_forward_list::iterator impl_;
};

template<typename T, class Tag>
void swap(linked_forward_list<T, Tag>&, linked_forward_list<T, Tag>&) noexcept;


_namespace_end(ilias)

#include <ilias/linked_forward_list-inl.h>

#endif /* _ILIAS_LINKED_FORWARD_LIST_H_ */
