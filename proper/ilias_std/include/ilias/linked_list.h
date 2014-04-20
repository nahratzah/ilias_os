#ifndef _ILIAS_LINKED_LIST_H_
#define _ILIAS_LINKED_LIST_H_

#include <cdecl.h>
#include <iterator>

_namespace_begin(ilias)


/*
 * The basic algorithm of linked lists.
 *
 * Note that this implementation only supports mutable elements.
 *
 * Instead of using this class, which is more like an implementation
 * detail than anything else, consider using linked_list<T>.
 */
class basic_linked_list {
 private:
  struct link_to_self {};

 public:
  class iterator;

  class element {
    friend basic_linked_list;
    friend basic_linked_list::iterator;

   private:
    explicit element(link_to_self) noexcept;

   protected:
    element() noexcept = default;
    constexpr element(const element&) noexcept;
    element& operator=(const element&) noexcept;

   private:
    element* succ_ = nullptr;
    element* pred_ = nullptr;
  };

 protected:
  basic_linked_list() noexcept {}
  basic_linked_list(basic_linked_list&&) noexcept;
  basic_linked_list(const basic_linked_list&) = delete;
  basic_linked_list& operator=(const basic_linked_list&) = delete;

  void link_front(element*) noexcept;
  void link_back(element*) noexcept;
  element* unlink_front() noexcept;
  element* unlink_back() noexcept;
  iterator link(iterator, element*) noexcept;
  element* unlink(iterator) noexcept;

  static void splice(iterator, basic_linked_list&) noexcept;
  static void splice(iterator, iterator, iterator)
      noexcept;
  void reverse() noexcept;

  iterator begin() const noexcept;
  iterator end() const noexcept;

 public:
  bool empty() const noexcept;
  void swap(basic_linked_list&) noexcept;

 private:
  element root_{ link_to_self() };
};

bool operator==(const basic_linked_list::iterator&,
                const basic_linked_list::iterator&) noexcept;
bool operator!=(const basic_linked_list::iterator&,
                const basic_linked_list::iterator&) noexcept;
void swap(basic_linked_list&, basic_linked_list&) noexcept;

class basic_linked_list::iterator
: public _namespace(std)::iterator<_namespace(std)::bidirectional_iterator_tag,
                                   element>
{
  friend basic_linked_list;
  friend bool operator==(const basic_linked_list::iterator&,
                         const basic_linked_list::iterator&) noexcept;

 private:
  explicit iterator(element*) noexcept;

 public:
  iterator& operator++() noexcept;
  iterator operator++(int) noexcept;
  iterator& operator--() noexcept;
  iterator operator--(int) noexcept;

  element& operator*() const noexcept;
  element* operator->() const noexcept;

 private:
  element* elem_ = nullptr;
};


template<typename, typename = void> class linked_list;

template<typename Tag = void>
class linked_list_element
: private basic_linked_list::element
{
  template<typename, typename> friend class linked_list;

 protected:
  linked_list_element() = default;
  linked_list_element(const linked_list_element&) = default;
  linked_list_element& operator=(const linked_list_element&) = default;
  ~linked_list_element() noexcept = default;
};

template<typename T, typename Tag>
class linked_list
: private basic_linked_list
{
 private:
  using tag_type = Tag;

 public:
  using value_type = T;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;

  class iterator;
  class const_iterator;
  using reverse_iterator = _namespace(std)::reverse_iterator<iterator>;
  using const_reverse_iterator =
      _namespace(std)::reverse_iterator<const_iterator>;

  void link_front(pointer) noexcept;
  void link_back(pointer) noexcept;
  pointer unlink_front() noexcept;
  pointer unlink_back() noexcept;
  iterator link(const_iterator, pointer) noexcept;
  pointer unlink(const_iterator) noexcept;

  reference front() noexcept;
  const_reference front() const noexcept;
  reference back() noexcept;
  const_reference back() const noexcept;

  using basic_linked_list::empty;

  static void splice(const_iterator, linked_list&) noexcept;
  static void splice(const_iterator, linked_list&&) noexcept;
  static void splice(const_iterator, const_iterator, const_iterator) noexcept;
  using basic_linked_list::reverse;

  template<typename Compare>
  static void merge(const_iterator, const_iterator,
                    const_iterator, const_iterator, Compare);
  template<typename Compare>
  static void sort(const_iterator, const_iterator, Compare, size_t);
  template<typename Compare>
  static void sort(const_iterator, const_iterator, Compare);

  iterator begin() noexcept;
  iterator end() noexcept;
  reverse_iterator rbegin() noexcept;
  reverse_iterator rend() noexcept;
  const_iterator begin() const noexcept;
  const_iterator end() const noexcept;
  const_reverse_iterator rbegin() const noexcept;
  const_reverse_iterator rend() const noexcept;
  const_iterator cbegin() const noexcept;
  const_iterator cend() const noexcept;
  const_reverse_iterator crbegin() const noexcept;
  const_reverse_iterator crend() const noexcept;

  void swap(linked_list&) noexcept;

  static iterator nonconst_iterator(const_iterator) noexcept;

 private:
  static element* down_cast_(pointer) noexcept;
  static element* down_cast_(const_pointer) noexcept;
  static pointer up_cast_(element*) noexcept;
};

template<typename T, typename Tag>
class linked_list<T, Tag>::iterator
: public _namespace(std)::iterator<
    basic_linked_list::iterator::iterator_category, T>
{
  friend const_iterator;
  friend linked_list;

 public:
  explicit iterator(const basic_linked_list::iterator&) noexcept;

  T& operator*() const noexcept;
  T* operator->() const noexcept;

  iterator& operator++() noexcept;
  iterator operator++(int) noexcept;
  iterator& operator--() noexcept;
  iterator operator--(int) noexcept;

  bool operator==(const iterator&) const noexcept;
  bool operator!=(const iterator&) const noexcept;
  bool operator==(const const_iterator&) const noexcept;
  bool operator!=(const const_iterator&) const noexcept;

 private:
  basic_linked_list::iterator impl_;
};

template<typename T, typename Tag>
class linked_list<T, Tag>::const_iterator
: public _namespace(std)::iterator<
    basic_linked_list::iterator::iterator_category, const T>
{
  friend linked_list;

 public:
  explicit const_iterator(const basic_linked_list::iterator&) noexcept;
  const_iterator(const iterator&) noexcept;

  const T& operator*() const noexcept;
  const T* operator->() const noexcept;

  const_iterator& operator++() noexcept;
  const_iterator operator++(int) noexcept;
  const_iterator& operator--() noexcept;
  const_iterator operator--(int) noexcept;

  bool operator==(const iterator&) const noexcept;
  bool operator!=(const iterator&) const noexcept;
  bool operator==(const const_iterator&) const noexcept;
  bool operator!=(const const_iterator&) const noexcept;

 private:
  basic_linked_list::iterator impl_;
};

template<typename T, typename Tag>
void swap(linked_list<T, Tag>&, linked_list<T, Tag>&) noexcept;


_namespace_end(ilias)

#include <ilias/linked_list-inl.h>

#endif /* _ILIAS_LINKED_LIST_H_ */
