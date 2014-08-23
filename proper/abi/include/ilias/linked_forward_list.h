#ifndef _ILIAS_LINKED_FORWARD_LIST_H_
#define _ILIAS_LINKED_FORWARD_LIST_H_

#include <cdecl.h>
#include <iterator>

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

 protected:
  class ptr_iterator;

 public:
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
  element* unlink(iterator) noexcept;
  element* unlink(element*) noexcept;

  void splice_front(basic_linked_forward_list&) noexcept;
  void splice_front(basic_linked_forward_list&,
                    iterator, iterator) noexcept;
  void splice_after(iterator, basic_linked_forward_list&) noexcept;
  void splice_after(iterator, basic_linked_forward_list&,
                    iterator, iterator) noexcept;
  static void splice(ptr_iterator, ptr_iterator, ptr_iterator) noexcept;

  iterator begin() const noexcept;
  iterator end() const noexcept;

 public:
  bool empty() const noexcept;
  void swap(basic_linked_forward_list&) noexcept;

 private:
  element*& find_succ_for_(element*) noexcept;

  element* head_ = nullptr;
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
  pointer unlink(const_iterator) noexcept;
  pointer unlink(const_pointer) noexcept;

  reference front() noexcept;
  const_reference front() const noexcept;

  using basic_linked_forward_list::empty;

  void splice_front(linked_forward_list&&) noexcept;
  void splice_front(linked_forward_list&&, const_iterator, const_iterator)
      noexcept;
  void splice_after(const_iterator, linked_forward_list&&) noexcept;
  void splice_after(const_iterator,
                    linked_forward_list&&, const_iterator, const_iterator)
      noexcept;

  iterator begin() noexcept;
  iterator end() noexcept;
  const_iterator begin() const noexcept;
  const_iterator end() const noexcept;
  const_iterator cbegin() const noexcept;
  const_iterator cend() const noexcept;

  void swap(linked_forward_list&) noexcept;

  static iterator nonconst_iterator(const_iterator) noexcept;

 private:
  static element* down_cast_(pointer) noexcept;
  static element* down_cast_(const_pointer) noexcept;
  static pointer up_cast_(element*) noexcept;
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

 private:
  basic_linked_forward_list::iterator impl_;
};

template<typename T, class Tag>
void swap(linked_forward_list<T, Tag>&, linked_forward_list<T, Tag>&) noexcept;


_namespace_end(ilias)

#include <ilias/linked_forward_list-inl.h>

#endif /* _ILIAS_LINKED_FORWARD_LIST_H_ */
