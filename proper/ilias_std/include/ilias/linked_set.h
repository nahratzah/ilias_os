#ifndef _ILIAS_LINKED_SET_H_
#define _ILIAS_LINKED_SET_H_

#include <cdecl.h>
#include <array>
#include <type_traits>
#include <functional>

_namespace_begin(ilias)


template<typename>
struct set_augmentation_is_recursive : _namespace(std)::true_type {};

template<typename A>
struct set_augmentation_is_recursive<_namespace(std)::reference_wrapper<A>>
: set_augmentation_is_recursive<A> {};


namespace impl {

template<typename...> struct set_augmentations_are_recursive_;
template<typename A, typename... Tail>
struct set_augmentations_are_recursive_<A, Tail...>
: _namespace(std)::conditional_t<set_augmentation_is_recursive<A>::value,
                                 _namespace(std)::true_type,
                                 typename set_augmentations_are_recursive_<
                                     Tail...>::type>
{};

template<>
struct set_augmentations_are_recursive_<>
: _namespace(std)::false_type
{};

} /* namespace ilias::impl */


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
  static void apply_augmentations_(element*, bool) noexcept {}
  template<typename Augment0, typename... Augment>
  static void apply_augmentations_(element*, bool, Augment0, Augment&&...)
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

void swap(basic_linked_set&, basic_linked_set&) noexcept;

class basic_linked_set::iterator {
  friend basic_linked_set;

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


_namespace_end(ilias)

#include <ilias/linked_set-inl.h>

#endif /* _ILIAS_LINKED_SET_H_ */
