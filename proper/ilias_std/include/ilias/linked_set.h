#ifndef _ILIAS_LINKED_SET_H_
#define _ILIAS_LINKED_SET_H_

#include <cdecl.h>
#include <array>
#include <type_traits>
#include <functional>
#include <tuple>

_namespace_begin(ilias)


class basic_linked_set {
 public:
  class iterator;

  class element {
    friend basic_linked_set;
    friend basic_linked_set::iterator;

   protected:
    element() noexcept = default;
    element(const element&) noexcept;
    element& operator=(const element&) noexcept;

    element* parent() const noexcept;
    element* left() const noexcept;
    element* right() const noexcept;

   private:
    bool red() const noexcept;
    bool black() const noexcept;
    uintptr_t rb_flag_() const noexcept;
    element* child(unsigned int) const noexcept;
    element* succ() const noexcept;
    element* pred() const noexcept;
    element* sp_(unsigned int) const noexcept;
    element* sibling() const noexcept;
    unsigned int pidx_() const noexcept;

    uintptr_t parent_ = 0;  // Tagged pointer to element.
    _namespace(std)::array<element*, 2> children_ = {{ nullptr, nullptr }};
  };

  basic_linked_set() noexcept : root_(nullptr) {}
  basic_linked_set(const basic_linked_set&) = delete;
  basic_linked_set(basic_linked_set&&) noexcept;
  basic_linked_set& operator=(const basic_linked_set&) = delete;
  basic_linked_set& operator=(basic_linked_set&&) noexcept;

  void swap(basic_linked_set&) noexcept;

  bool empty() const noexcept { return root_ == nullptr; }
  template<typename Visitor> void unlink_all(Visitor) noexcept;
  void unlink_all() noexcept;
  void clear() noexcept;

  template<typename Comparator, typename... Augments>
  _namespace(std)::pair<iterator, bool> link(element*, Comparator, bool,
                                             Augments&&...);
  template<typename... Augments>
  element* unlink(element*, Augments&&...) noexcept;
  template<typename... Augments>
  element* unlink(iterator, Augments&&...) noexcept;

  iterator root() const noexcept;
  iterator begin() const noexcept;
  iterator end() const noexcept;

  template<typename T, typename Comparator>
  iterator find(const T&, Comparator);
  template<typename T, typename Comparator>
  iterator lower_bound(const T&, Comparator);
  template<typename T, typename Comparator>
  iterator upper_bound(const T&, Comparator);
  template<typename T, typename Comparator>
  _namespace(std)::pair<iterator, iterator> equal_range(const T&, Comparator);

 private:
  /* Color modification. */
  static void set_color(element*, uintptr_t) noexcept;
  static void toggle_color(element*) noexcept;

  /* Link/unlink support functions. */
  template<typename Comparator>
  _namespace(std)::pair<iterator, bool> link_(element*, Comparator, bool);
  template<typename... Augments>
  void link_fixup_(element*, Augments&&...) noexcept;
  void unlink_to_leaf_(element*) noexcept;
  void node_swap_(element*, element*) noexcept;
  template<typename... Augments>
  void unlink_fixup_(element*, Augments&&...) noexcept;

  /* Rotation operation. */
  element* rotate_(element*, unsigned int) noexcept;

  /* Augmentation operation. */
  static void apply_augmentations_(element*) noexcept {}
  template<typename Augment0, typename... Augment>
  static void apply_augmentations_(element*, Augment0, Augment&&...)
      noexcept;

  /* Element parent-pointer tagging constants. */
  static constexpr uintptr_t BLACK = 0x0;
  static constexpr uintptr_t RED = 0x1;
  static constexpr uintptr_t RB_MASK = 0x1;
  static constexpr uintptr_t PTR_MASK = ~RB_MASK;

  /* Root node of the tree. */
  element* root_;

  static_assert((alignof(element) & ~PTR_MASK) == 0,
                "Tagged pointer won't work.");
};

bool operator==(const basic_linked_set::iterator&,
                const basic_linked_set::iterator&) noexcept;
bool operator!=(const basic_linked_set::iterator&,
                const basic_linked_set::iterator&) noexcept;
void swap(basic_linked_set&, basic_linked_set&) noexcept;

class basic_linked_set::iterator
: public _namespace(std)::iterator<_namespace(std)::bidirectional_iterator_tag,
                                   element>
{
  friend basic_linked_set;
  friend bool operator==(const basic_linked_set::iterator&,
                         const basic_linked_set::iterator&) noexcept;

 private:
  explicit iterator(const basic_linked_set*, element*) noexcept;

 public:
  iterator& operator++() noexcept;
  iterator operator++(int) noexcept;
  iterator& operator--() noexcept;
  iterator operator--(int) noexcept;

  element& operator*() const noexcept;
  element* operator->() const noexcept;

 private:
  const basic_linked_set* set_ = nullptr;
  element* elem_ = nullptr;
};


template<typename T, class = void, typename = _namespace(std)::less<T>,
         typename...>
class linked_set;

template<typename T, class Tag>
class linked_set_iterator_;
template<typename T, class Tag>
class const_linked_set_iterator_;


template<typename Derived, class Tag = void>
class linked_set_element
: private basic_linked_set::element
{
  template<typename, class, typename, typename...> friend class linked_set;
  template<typename, class> friend class linked_set_iterator_;
  template<typename, class> friend class const_linked_set_iterator_;

 protected:
  linked_set_element() = default;
  linked_set_element(const linked_set_element&) = default;
  linked_set_element& operator=(const linked_set_element&) = default;
  ~linked_set_element() = default;

  const Derived* parent() const noexcept;
  const Derived* left() const noexcept;
  const Derived* right() const noexcept;
};

template<typename T, class Tag, typename Cmp, typename... Augments>
class linked_set
: private basic_linked_set
{
 public:
  using value_type = T;
  using key_type = value_type;
  using key_compare = Cmp;
  using value_compare = Cmp;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;

  using iterator = linked_set_iterator_<T, Tag>;
  using const_iterator = const_linked_set_iterator_<T, Tag>;
  using reverse_iterator = _namespace(std)::reverse_iterator<iterator>;
  using const_reverse_iterator =
      _namespace(std)::reverse_iterator<const_iterator>;

  explicit linked_set(const key_compare& = value_compare());
  explicit linked_set(const key_compare&, const Augments&...);
  linked_set(const linked_set&) = delete;
  linked_set& operator=(const linked_set&) = delete;
  linked_set(linked_set&&);
  linked_set& operator=(linked_set&&);

  _namespace(std)::pair<iterator, bool> link(pointer, bool) noexcept;
  pointer unlink(const_iterator) noexcept;
  pointer unlink(const_pointer) noexcept;
  void unlink_all() noexcept;
  template<typename Visitor> void unlink_all(Visitor);

  using basic_linked_set::empty;

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

  void swap(linked_set&) noexcept;

  static iterator nonconst_iterator(const_iterator) noexcept;

 private:
  template<typename A>
  static void invoke_augment_(const A&, element*) noexcept;
  template<size_t... Idx>
  _namespace(std)::pair<iterator, bool> link_(
      pointer, bool, _namespace(std)::index_sequence<Idx...>) noexcept;
  template<size_t... Idx>
  pointer unlink_(
      const_iterator, _namespace(std)::index_sequence<Idx...>) noexcept;
  template<size_t... Idx>
  pointer unlink_(
      const_pointer, _namespace(std)::index_sequence<Idx...>) noexcept;

  static element* down_cast_(pointer) noexcept;
  static element* down_cast_(const_pointer) noexcept;
  static pointer up_cast_(element*) noexcept;

  _namespace(std)::tuple<Augments...> augments_;
  key_compare cmp_;
};

template<typename T, class Tag>
class linked_set_iterator_
: public _namespace(std)::iterator<
    basic_linked_set::iterator::iterator_category, T>
{
  template<typename, class, typename, typename...> friend class linked_set;
  template<typename, class> friend class const_linked_set_iterator_;

 public:
  explicit linked_set_iterator_(const basic_linked_set::iterator&) noexcept;

  T& operator*() const noexcept;
  T* operator->() const noexcept;

  linked_set_iterator_& operator++() noexcept;
  linked_set_iterator_ operator++(int) noexcept;
  linked_set_iterator_& operator--() noexcept;
  linked_set_iterator_ operator--(int) noexcept;

  bool operator==(const linked_set_iterator_&) const noexcept;
  bool operator!=(const linked_set_iterator_&) const noexcept;
  bool operator==(const const_linked_set_iterator_<T, Tag>&) const noexcept;
  bool operator!=(const const_linked_set_iterator_<T, Tag>&) const noexcept;

 private:
  basic_linked_set::iterator impl_;
};

template<typename T, class Tag>
class const_linked_set_iterator_
: public _namespace(std)::iterator<
    basic_linked_set::iterator::iterator_category, const T>
{
  template<typename, class, typename, typename...> friend class linked_set;
  template<typename, class> friend class linked_set_iterator_;

 public:
  explicit const_linked_set_iterator_(const basic_linked_set::iterator&)
      noexcept;
  const_linked_set_iterator_(const linked_set_iterator_<T, Tag>&) noexcept;

  const T& operator*() const noexcept;
  const T* operator->() const noexcept;

  const_linked_set_iterator_& operator++() noexcept;
  const_linked_set_iterator_ operator++(int) noexcept;
  const_linked_set_iterator_& operator--() noexcept;
  const_linked_set_iterator_ operator--(int) noexcept;

  bool operator==(const linked_set_iterator_<T, Tag>&) const noexcept;
  bool operator!=(const linked_set_iterator_<T, Tag>&) const noexcept;
  bool operator==(const const_linked_set_iterator_&) const noexcept;
  bool operator!=(const const_linked_set_iterator_&) const noexcept;

 private:
  basic_linked_set::iterator impl_;
};

template<typename T, class Tag, typename Cmp, typename... Augments>
void swap(linked_set<T, Tag, Cmp, Augments...>&,
          linked_set<T, Tag, Cmp, Augments...>&) noexcept;


_namespace_end(ilias)

#include <ilias/linked_set-inl.h>

#endif /* _ILIAS_LINKED_SET_H_ */
