#ifndef _ILIAS_LINKED_SET_INL_H_
#define _ILIAS_LINKED_SET_INL_H_

#include <ilias/linked_set.h>
#include <algorithm>
#include <functional>
#include <type_traits>
#include <utility>

_namespace_begin(ilias)


inline bool operator==(const basic_linked_set::iterator& x,
                       const basic_linked_set::iterator& y) noexcept {
  return x.elem_ == y.elem_;
}

inline bool operator!=(const basic_linked_set::iterator& x,
                       const basic_linked_set::iterator& y) noexcept {
  return !(x == y);
}

inline void swap(basic_linked_set& x, basic_linked_set& y) noexcept {
  x.swap(y);
}


inline basic_linked_set::basic_linked_set(basic_linked_set&& o) noexcept
: root_(o.root_)
{
  o.root_ = nullptr;
}

inline auto basic_linked_set::operator=(basic_linked_set&& o) noexcept ->
    basic_linked_set& {
  basic_linked_set(_namespace(std)::move(o)).swap(*this);
  return *this;
}

inline auto basic_linked_set::swap(basic_linked_set& o) noexcept -> void {
  using _namespace(std)::swap;

  swap(root_, o.root_);
}

template<typename Visitor>
auto basic_linked_set::unlink_all(Visitor visitor) noexcept -> void {
  /*
   * Abuse linked-list semantics to visit each element in O(n) time,
   * as opposed to O(n log n) time that iterator based visiting would take.
   *
   * This algorithm reuses a child pointer in each node to get the complexity
   * reduction.  This only works because the child pointers are going to be
   * zeroed anyway.
   */
  using _namespace(std)::exchange;

  element* i = root_;
  if (i == nullptr) return;
  root_ = nullptr;
  element* last = i;
  while (last->children_[1]) last = last->children_[1];

  do {
    last->children_[1] = exchange(i->children_[0], nullptr);
    while (last->children_[1]) last = last->children_[1];

    element* succ = exchange(i->children_[1], nullptr);
    i->parent_ = 0;
    visitor(i);
    i = succ;
  } while (i);
}

inline auto basic_linked_set::clear() noexcept -> void {
  root_ = nullptr;
}

template<typename Comparator, typename... Augments>
auto basic_linked_set::link(element* elem, Comparator cmp, bool dup_ok,
                            Augments&&... augments) ->
    _namespace(std)::pair<iterator, bool> {
  auto rv = link_(elem, cmp, dup_ok);
  if (rv.second)
    link_fixup_(elem, _namespace(std)::forward<Augments>(augments)...);
  return rv;
}

template<typename... Augments>
auto basic_linked_set::unlink(element* elem, Augments&&... augments)
    noexcept -> element* {
  using _namespace(std)::none_of;
  using _namespace(std)::ref;

  assert_msg((elem->parent() == nullptr) == (root_ == elem),
             "Attempt to unlink node that is not linked.");

  unlink_to_leaf_(elem);
  unlink_fixup_(elem, ref(augments)...);
  assert(elem->parent_ == 0 &&
         none_of(elem->children_.begin(), elem->children_.end(),
                 [](element* p) { return p != nullptr; }));

  return elem;
}

template<typename... Augments>
auto basic_linked_set::unlink(iterator iter, Augments&&... augments)
    noexcept -> element* {
  assert(iter.set_ == this);
  return unlink(iter.elem_, _namespace(std)::forward<Augments>(augments)...);
}

inline auto basic_linked_set::root() const noexcept -> iterator {
  return iterator(this, root_);
}

inline auto basic_linked_set::end() const noexcept -> iterator {
  element* last = nullptr;
  return iterator(this, last);
}

template<typename T, typename Comparator>
auto basic_linked_set::find(const T& v, Comparator cmp) -> iterator {
  element* i = root_;
  while (i != nullptr) {
    if (cmp(*i, v))
      i = i->right();
    else if (cmp(v, *i))
      i = i->left();
    else
      break;  // GUARD
  }
  return iterator(this, i);
}

template<typename T, typename Comparator>
auto basic_linked_set::lower_bound(const T& v, Comparator cmp) -> iterator {
  element* i = root_;
  element* match = nullptr;  // iterator(this, nullptr) == end()
  while (i != nullptr) {
    if (cmp(*i, v)) {
      i = i->right();
    } else {
      match = i;
      i = i->left();
    }
  }
  return iterator(this, match);
}

template<typename T, typename Comparator>
auto basic_linked_set::upper_bound(const T& v, Comparator cmp) -> iterator {
  using _namespace(std)::bind;
  using _namespace(std)::ref;
  using _namespace(std)::placeholders::_1;
  using _namespace(std)::placeholders::_2;

  /* Bind statement transforms cmp in <= operation. */
  return lower_bound(v, bind(ref(cmp), _2, _1));
}

template<typename T, typename Comparator>
auto basic_linked_set::equal_range(const T& v, Comparator cmp) ->
    _namespace(std)::pair<iterator, iterator> {
  return _namespace(std)::make_pair(lower_bound(v, ref(cmp)),
                                    upper_bound(v, ref(cmp)));
}

inline auto basic_linked_set::set_color(element* e, uintptr_t c) noexcept ->
    void {
  assert(c == BLACK || c == RED);
  if (c == 0)
    e->parent_ &= PTR_MASK;
  else
    e->parent_ |= RB_MASK;
}

inline auto basic_linked_set::toggle_color(element* e) noexcept -> void {
  e->parent_ ^= RB_MASK;
}

template<typename Comparator>
auto basic_linked_set::link_(element* elem, Comparator cmp, bool dup_ok) ->
    _namespace(std)::pair<iterator, bool> {
  using _namespace(std)::make_pair;

  /* Prevent linking elem twice. */
  assert(elem->parent_ == 0);
  assert(_namespace(std)::all_of(elem->children_.begin(),
                                 elem->children_.end(),
                                 [](element* e) { return (e == nullptr); }));

  /* Find a spot where to insert the new node. */
  element* p = root_;
  element** pp = &root_;
  while (p) {
    if (cmp(*elem, *p)) {
      pp = &p->children_[0];
      p = p->children_[0];
    } else if (dup_ok || cmp(*p, *elem)) {
      pp = &p->children_[1];
      p = p->children_[1];
    } else
      return make_pair(iterator(this, p), false);
  }
  *pp = elem;
  elem->parent_ = (reinterpret_cast<uintptr_t>(p) | RED);

  link_fixup_(elem);

  return make_pair(iterator(this, elem), true);
}

template<typename... Augments>
auto basic_linked_set::link_fixup_(element* elem, Augments&&... augments)
    noexcept -> void {
  using _namespace(std)::ref;

  assert(elem->red());
  assert((elem->parent() == nullptr) == (root_ == elem));

  while (elem->parent() != nullptr) {
    element* p = elem->parent();
    if (p->black()) {
      apply_augmentations_(elem, ref(augments)...);
      do {
        apply_augmentations_(p, ref(augments)...);
        p = p->parent();
      } while (p != nullptr);
      return;
    }

    element*const ps = p->sibling();
    element*const pp = p->parent();
    if (ps && ps->red()) {
      /* Parent and its sibling are both red:
       * color their parent red instead. */
      toggle_color(pp);
      toggle_color(p);
      toggle_color(ps);

      apply_augmentations_(elem, ref(augments)...);
      apply_augmentations_(p, ref(augments)...);

      elem = pp;
    } else {
      /* Rotation trickery. */
      const auto ppidx = p->pidx_();

      /* Ensure both red nodes are on the same side of their parents. */
      if (elem->pidx_() != ppidx) {
        element* subtree_root = rotate_(p, ppidx);
        assert(subtree_root == elem);
        elem = p;
        p = subtree_root;

        assert(p == elem->parent() && pp == p->parent());
      }

      /* Subtree rooted in elem won't change in the next operations. */
      apply_augmentations_(elem, ref(augments)...);

      auto subtree_root = rotate_(pp, 1 - ppidx);
      assert(subtree_root == p);
      apply_augmentations_(pp, ref(augments)...);

      do {
        apply_augmentations_(p, ref(augments)...);
        p = p->parent();
      } while (p != nullptr);
      return;
    }
  }

  /* elem is the root node. */
  assert(root_ == elem);
  if (!elem->black()) toggle_color(elem);
  apply_augmentations_(elem, ref(augments)...);
  return;
}

inline auto basic_linked_set::node_swap_(element* x, element* y) noexcept ->
    void {
  using _namespace(std)::swap;

  swap(x->parent_, y->parent_);  // Color swap is required here.
  swap(x->children_, y->children_);
}

template<typename... Augments>
auto basic_linked_set::unlink_fixup_(element* elem, Augments&&... augments)
    noexcept -> void {
  using _namespace(std)::none_of;
  using _namespace(std)::all_of;
  using _namespace(std)::ref;

  assert(none_of(elem->children_.begin(),
                 elem->children_.end(),
                 [](element* p) { return p != nullptr; }));
  element* n = elem;
  if (n->red()) return;
  element* p = elem->parent();
  if (p == nullptr) {
    assert(root_ == n);
    root_ = nullptr;
    n->parent_ = 0;
    return;
  } else if (p->red()) {
    toggle_color(p);
    p->children_[n->pidx_()] = nullptr;
    n->parent_ = 0;
    do {
      apply_augmentations_(p, ref(augments)...);
      p = p->parent();
    } while (p != nullptr);
    return;
  }
  p->children_[n->pidx_()] = nullptr;
  n->parent_ = 0;
  n = p;

  for (;;) {
    apply_augmentations_(n, ref(augments)...);
    p = n->parent();

    if (p != nullptr) {
      element* s = n->sibling();
      assert(s != nullptr);
      if (s->red()) {
        assert(s == n->sibling());
        element* pp = rotate_(p, n->pidx_());
        assert(s == pp);
        toggle_color(p);
        toggle_color(pp);
        s = n->sibling();
      }
      if (p->black() && s->black() &&
          all_of(s->children_.begin(), s->children_.end(),
                 [](element* p) { return p == nullptr || p->black(); })) {
        assert(s == n->sibling());
        toggle_color(s);
        apply_augmentations_(n, ref(augments)...);
        n = p;
        continue;  // RECURSE
      } else if (p->red() &&
                 all_of(s->children_.begin(), s->children_.end(),
                        [](element* p) {
                          return p == nullptr || p->black();
                        })) {
        assert(s == n->sibling());
        toggle_color(s);
        toggle_color(p);
      } else {
        if (s->children_[n->pidx_()]->red()) {
          assert(s == n->sibling());
          toggle_color(s->children_[n->pidx_()]);
          toggle_color(s);
          rotate_(s, 1 - n->pidx_());
          apply_augmentations_(s, ref(augments)...);
          apply_augmentations_(s->parent(), ref(augments)...);
          s = n->sibling();
        }

        assert(s == n->sibling());
        set_color(s, p->rb_flag_());
        set_color(p, BLACK);
        set_color(s->children_[1 - n->pidx_()], BLACK);
        rotate_(p, n->pidx_());
      }
    }

    break;  // GUARD
  }

  p = n;
  while ((p = p->parent()) != nullptr)
    apply_augmentations_(p, ref(augments)...);
}

template<typename Augment0, typename... Augment>
auto basic_linked_set::apply_augmentations_(element* e, Augment0 augment0,
                                            Augment&&... augments)
    noexcept -> void {
  augment0(e);
  apply_augmentations_(e, _namespace(std)::forward<Augment>(augments)...);
}


inline basic_linked_set::element::element(const element&) noexcept
: element()
{}

inline auto basic_linked_set::element::operator=(const element&) noexcept ->
    element& {
  return *this;
}

inline auto basic_linked_set::element::red() const noexcept -> bool {
  return rb_flag_() == RED;
}

inline auto basic_linked_set::element::black() const noexcept -> bool {
  return rb_flag_() == BLACK;
}

inline auto basic_linked_set::element::rb_flag_() const noexcept ->
    uintptr_t {
  return parent_ & RB_MASK;
}

inline auto basic_linked_set::element::parent() const noexcept ->
    element* {
  return reinterpret_cast<element*>(parent_ & PTR_MASK);
}

inline auto basic_linked_set::element::left() const noexcept -> element* {
  return child(0);
}

inline auto basic_linked_set::element::right() const noexcept -> element* {
  return child(1);
}

inline auto basic_linked_set::element::child(unsigned int i) const noexcept ->
    element* {
  assert(i < children_.size());
  return children_[i];
}

inline auto basic_linked_set::element::succ() const noexcept -> element* {
  return sp_(1);
}

inline auto basic_linked_set::element::pred() const noexcept -> element* {
  return sp_(0);
}

inline auto basic_linked_set::element::sibling() const noexcept -> element* {
  element* p = parent();
  if (p) return p->child(1 - pidx_());
  return nullptr;
}

inline auto basic_linked_set::element::pidx_() const noexcept -> unsigned int {
  assert(parent());
  return (parent()->left() == this ? 0 : 1);
}


inline basic_linked_set::iterator::iterator(const basic_linked_set* set,
                                            element* elem) noexcept
: set_(set),
  elem_(elem)
{}

inline auto basic_linked_set::iterator::operator++() noexcept -> iterator& {
  if (_predict_true(elem_ != nullptr)) {
    elem_ = elem_->succ();
  } else if (set_ != nullptr) {
    elem_ = set_->root_;
    while (elem_) elem_ = elem_->left();
  }
  return *this;
}

inline auto basic_linked_set::iterator::operator++(int) noexcept -> iterator {
  iterator clone = *this;
  ++*this;
  return clone;
}

inline auto basic_linked_set::iterator::operator--() noexcept -> iterator& {
  if (_predict_true(elem_ != nullptr)) {
    elem_ = elem_->pred();
  } else if (set_ != nullptr) {
    elem_ = set_->root_;
    while (elem_) elem_ = elem_->right();
  }
  return *this;
}

inline auto basic_linked_set::iterator::operator--(int) noexcept -> iterator {
  iterator clone = *this;
  --*this;
  return clone;
}

inline auto basic_linked_set::iterator::operator*() const noexcept ->
    element& {
  return *elem_;
}

inline auto basic_linked_set::iterator::operator->() const noexcept ->
    element* {
  return elem_;
}


template<typename Derived, class Tag>
auto linked_set_element<Derived, Tag>::parent() const noexcept ->
    const Derived* {
  return static_cast<Derived>(static_cast<linked_set_element>(
      basic_linked_set::element::parent()));
}

template<typename Derived, class Tag>
auto linked_set_element<Derived, Tag>::left() const noexcept ->
    const Derived* {
  return static_cast<Derived>(static_cast<linked_set_element>(
      basic_linked_set::element::left()));
}

template<typename Derived, class Tag>
auto linked_set_element<Derived, Tag>::right() const noexcept ->
    const Derived* {
  return static_cast<Derived>(static_cast<linked_set_element>(
      basic_linked_set::element::right()));
}


template<typename T, class Tag, typename Cmp, typename... Augments>
linked_set<T, Tag, Cmp, Augments...>::linked_set(const key_compare& cmp)
: cmp_(cmp)
{}

template<typename T, class Tag, typename Cmp, typename... Augments>
linked_set<T, Tag, Cmp, Augments...>::linked_set(const key_compare& cmp,
                                                 const Augments&... augments)
: augments_(augments...),
  cmp_(cmp)
{}

template<typename T, class Tag, typename Cmp, typename... Augments>
linked_set<T, Tag, Cmp, Augments...>::linked_set(linked_set&& o)
: basic_linked_set(move(o)),
  augments_(move(o.augments_)),
  cmp_(move(o.cmp_))
{}

template<typename T, class Tag, typename Cmp, typename... Augments>
auto linked_set<T, Tag, Cmp, Augments...>::operator=(linked_set&& o) ->
    linked_set& {
  linked_set(move(o)).swap(*this);
  return *this;
}

template<typename T, class Tag, typename Cmp, typename... Augments>
auto linked_set<T, Tag, Cmp, Augments...>::link(pointer p, bool dup_ok)
    noexcept -> _namespace(std)::pair<iterator, bool> {
  return link_(p, dup_ok, _namespace(std)::index_sequence_for<Augments...>());
}

template<typename T, class Tag, typename Cmp, typename... Augments>
auto linked_set<T, Tag, Cmp, Augments...>::unlink(const_iterator i)
    noexcept -> pointer {
  return unlink_(i, _namespace(std)::index_sequence_for<Augments...>());
}

template<typename T, class Tag, typename Cmp, typename... Augments>
auto linked_set<T, Tag, Cmp, Augments...>::unlink(const_pointer p)
    noexcept -> pointer {
  return unlink_(p, _namespace(std)::index_sequence_for<Augments...>());
}

template<typename T, class Tag, typename Cmp, typename... Augments>
auto linked_set<T, Tag, Cmp, Augments...>::unlink_all() noexcept -> void {
  basic_linked_set::unlink_all();
}

template<typename T, class Tag, typename Cmp, typename... Augments>
template<typename Visitor>
auto linked_set<T, Tag, Cmp, Augments...>::unlink_all(Visitor v) -> void {
  basic_linked_set::unlink_all([&v](element* e) {
                                 v(*up_cast_(e));
                               });
}

template<typename T, class Tag, typename Cmp, typename... Augments>
auto linked_set<T, Tag, Cmp, Augments...>::begin() noexcept ->
    iterator {
  return iterator(basic_linked_set::begin());
}

template<typename T, class Tag, typename Cmp, typename... Augments>
auto linked_set<T, Tag, Cmp, Augments...>::end() noexcept ->
    iterator {
  return iterator(basic_linked_set::end());
}

template<typename T, class Tag, typename Cmp, typename... Augments>
auto linked_set<T, Tag, Cmp, Augments...>::rbegin() noexcept ->
    reverse_iterator {
  return reverse_iterator(end());
}

template<typename T, class Tag, typename Cmp, typename... Augments>
auto linked_set<T, Tag, Cmp, Augments...>::rend() noexcept ->
    reverse_iterator {
  return reverse_iterator(begin());
}

template<typename T, class Tag, typename Cmp, typename... Augments>
auto linked_set<T, Tag, Cmp, Augments...>::begin() const noexcept ->
    const_iterator {
  return const_iterator(basic_linked_set::begin());
}

template<typename T, class Tag, typename Cmp, typename... Augments>
auto linked_set<T, Tag, Cmp, Augments...>::end() const noexcept ->
    const_iterator {
  return const_iterator(basic_linked_set::begin());
}

template<typename T, class Tag, typename Cmp, typename... Augments>
auto linked_set<T, Tag, Cmp, Augments...>::rbegin() const noexcept ->
    const_reverse_iterator {
  return const_reverse_iterator(end());
}

template<typename T, class Tag, typename Cmp, typename... Augments>
auto linked_set<T, Tag, Cmp, Augments...>::rend() const noexcept ->
    const_reverse_iterator {
  return const_reverse_iterator(begin());
}

template<typename T, class Tag, typename Cmp, typename... Augments>
auto linked_set<T, Tag, Cmp, Augments...>::cbegin() const noexcept ->
    const_iterator {
  return begin();
}

template<typename T, class Tag, typename Cmp, typename... Augments>
auto linked_set<T, Tag, Cmp, Augments...>::cend() const noexcept ->
    const_iterator {
  return end();
}

template<typename T, class Tag, typename Cmp, typename... Augments>
auto linked_set<T, Tag, Cmp, Augments...>::crbegin() const noexcept ->
    const_reverse_iterator {
  return rbegin();
}

template<typename T, class Tag, typename Cmp, typename... Augments>
auto linked_set<T, Tag, Cmp, Augments...>::crend() const noexcept ->
    const_reverse_iterator {
  return rend();
}

template<typename T, class Tag, typename Cmp, typename... Augments>
template<typename K>
auto linked_set<T, Tag, Cmp, Augments...>::find(const K& v) ->
    iterator {
  using _namespace(std)::cref;
  return iterator(basic_linked_set::find(v, cref(cmp_)));
}

template<typename T, class Tag, typename Cmp, typename... Augments>
template<typename K>
auto linked_set<T, Tag, Cmp, Augments...>::find(const K& v) const ->
    const_iterator {
  using _namespace(std)::cref;
  return const_iterator(basic_linked_set::find(v, cref(cmp_)));
}

template<typename T, class Tag, typename Cmp, typename... Augments>
template<typename K>
auto linked_set<T, Tag, Cmp, Augments...>::lower_bound(const K& v) ->
    iterator {
  using _namespace(std)::cref;
  return iterator(basic_linked_set::lower_bound(v, cref(cmp_)));
}

template<typename T, class Tag, typename Cmp, typename... Augments>
template<typename K>
auto linked_set<T, Tag, Cmp, Augments...>::lower_bound(const K& v) const ->
    const_iterator {
  using _namespace(std)::cref;
  return const_iterator(basic_linked_set::lower_bound(v, cref(cmp_)));
}

template<typename T, class Tag, typename Cmp, typename... Augments>
template<typename K>
auto linked_set<T, Tag, Cmp, Augments...>::upper_bound(const K& v) ->
    iterator {
  using _namespace(std)::cref;
  return iterator(basic_linked_set::upper_bound(v, cref(cmp_)));
}

template<typename T, class Tag, typename Cmp, typename... Augments>
template<typename K>
auto linked_set<T, Tag, Cmp, Augments...>::upper_bound(const K& v) const ->
    const_iterator {
  using _namespace(std)::cref;
  return const_iterator(basic_linked_set::upper_bound(v, cref(cmp_)));
}

template<typename T, class Tag, typename Cmp, typename... Augments>
template<typename K>
auto linked_set<T, Tag, Cmp, Augments...>::equal_range(const K& v) ->
    _namespace(std)::pair<iterator, iterator> {
  using _namespace(std)::cref;
  using _namespace(std)::make_pair;

  auto range = basic_linked_set::equal_range(v, cref(cmp_));
  return make_pair(iterator(range.first), iterator(range.second));
}

template<typename T, class Tag, typename Cmp, typename... Augments>
template<typename K>
auto linked_set<T, Tag, Cmp, Augments...>::equal_range(const K& v) const ->
    _namespace(std)::pair<const_iterator, const_iterator> {
  using _namespace(std)::cref;
  using _namespace(std)::make_pair;

  auto range = basic_linked_set::equal_range(v, cref(cmp_));
  return make_pair(const_iterator(range.first), const_iterator(range.second));
}

template<typename T, class Tag, typename Cmp, typename... Augments>
auto linked_set<T, Tag, Cmp, Augments...>::swap(linked_set& o) noexcept ->
    void {
  using _namespace(std)::swap;

  basic_linked_set::swap(o);
  swap(cmp_, o.cmp_);
  swap(augments_, o.augments_);
}

template<typename T, class Tag, typename Cmp, typename... Augments>
auto linked_set<T, Tag, Cmp, Augments...>::nonconst_iterator(const_iterator i)
    noexcept -> iterator {
  return iterator(i.impl_);
}

template<typename T, class Tag, typename Cmp, typename... Augments>
template<typename A>
auto linked_set<T, Tag, Cmp, Augments...>::invoke_augment_(const A& a,
                                                           element* e)
    noexcept -> void {
  a(up_cast_(e));
}

template<typename T, class Tag, typename Cmp, typename... Augments>
template<size_t... Idx>
auto linked_set<T, Tag, Cmp, Augments...>::link_(
    pointer p, bool dup_ok, _namespace(std)::index_sequence<Idx...>)
    noexcept -> _namespace(std)::pair<iterator, bool> {
  using _namespace(std)::make_pair;
  using _namespace(std)::bind;
  using _namespace(std)::placeholders::_1;
  using _namespace(std)::cref;
  using _namespace(std)::get;

  auto link_result = basic_linked_set::link(
      down_cast_(p), cref(cmp_), dup_ok,
      bind(&invoke_augment_<Augments>, cref(get<Idx>(augments_)), _1)...);
  return make_pair(iterator(link_result.first), link_result.second);
}

template<typename T, class Tag, typename Cmp, typename... Augments>
template<size_t... Idx>
auto linked_set<T, Tag, Cmp, Augments...>::unlink_(
    const_iterator i, _namespace(std)::index_sequence<Idx...>)
    noexcept -> pointer {
  using _namespace(std)::bind;
  using _namespace(std)::placeholders::_1;
  using _namespace(std)::cref;
  using _namespace(std)::get;

  return up_cast_(basic_linked_set::unlink(
      i.impl_,
      bind(&invoke_augment_<Augments>, cref(get<Idx>(augments_)), _1)...));
}

template<typename T, class Tag, typename Cmp, typename... Augments>
template<size_t... Idx>
auto linked_set<T, Tag, Cmp, Augments...>::unlink_(
    const_pointer p, _namespace(std)::index_sequence<Idx...>)
    noexcept -> pointer {
  using _namespace(std)::bind;
  using _namespace(std)::placeholders::_1;
  using _namespace(std)::cref;
  using _namespace(std)::get;

  return up_cast_(basic_linked_set::unlink(
      down_cast_(p),
      bind(&invoke_augment_<Augments>, cref(get<Idx>(augments_)), _1)...));
}

template<typename T, class Tag, typename Cmp, typename... Augments>
auto linked_set<T, Tag, Cmp, Augments...>::down_cast_(pointer p)
    noexcept -> element* {
  if (!p) return nullptr;
  return static_cast<element*>(static_cast<linked_set_element<T, Tag>*>(p));
}

template<typename T, class Tag, typename Cmp, typename... Augments>
auto linked_set<T, Tag, Cmp, Augments...>::down_cast_(const_pointer p)
    noexcept -> element* {
  if (!p) return nullptr;
  return static_cast<element*>(const_cast<linked_set_element<T, Tag>>(
      static_cast<const linked_set_element<T, Tag>*>(p)));
}

template<typename T, class Tag, typename Cmp, typename... Augments>
auto linked_set<T, Tag, Cmp, Augments...>::up_cast_(element* e)
    noexcept -> pointer {
  if (!e) return nullptr;
  return static_cast<pointer>(static_cast<linked_set_element<T, Tag>*>(e));
}


template<typename T, class Tag>
linked_set_iterator_<T, Tag>::linked_set_iterator_(
    const basic_linked_set::iterator& impl) noexcept
: impl_(impl)
{}

template<typename T, class Tag>
auto linked_set_iterator_<T, Tag>::operator*() const noexcept -> T& {
  return static_cast<T&>(static_cast<linked_set_element<T, Tag>&>(*impl_));
}

template<typename T, class Tag>
auto linked_set_iterator_<T, Tag>::operator->() const noexcept -> T* {
  return static_cast<T*>(static_cast<linked_set_element<T, Tag>*>(*impl_));
}

template<typename T, class Tag>
auto linked_set_iterator_<T, Tag>::operator++() noexcept ->
    linked_set_iterator_& {
  ++impl_;
  return *this;
}

template<typename T, class Tag>
auto linked_set_iterator_<T, Tag>::operator++(int) noexcept ->
    linked_set_iterator_ {
  return linked_set_iterator_(impl_++);
}

template<typename T, class Tag>
auto linked_set_iterator_<T, Tag>::operator--() noexcept ->
    linked_set_iterator_& {
  --impl_;
  return *this;
}

template<typename T, class Tag>
auto linked_set_iterator_<T, Tag>::operator--(int) noexcept ->
    linked_set_iterator_ {
  return linked_set_iterator_(impl_--);
}

template<typename T, class Tag>
auto linked_set_iterator_<T, Tag>::operator==(const linked_set_iterator_& o)
    const noexcept -> bool {
  return impl_ == o.impl_;
}

template<typename T, class Tag>
auto linked_set_iterator_<T, Tag>::operator!=(const linked_set_iterator_& o)
    const noexcept -> bool {
  return impl_ != o.impl_;
}

template<typename T, class Tag>
auto linked_set_iterator_<T, Tag>::operator==(
    const const_linked_set_iterator_<T, Tag>& o) const noexcept -> bool {
  return impl_ == o.impl_;
}

template<typename T, class Tag>
auto linked_set_iterator_<T, Tag>::operator!=(
    const const_linked_set_iterator_<T, Tag>& o) const noexcept -> bool {
  return impl_ != o.impl_;
}


template<typename T, class Tag>
const_linked_set_iterator_<T, Tag>::const_linked_set_iterator_(
    const basic_linked_set::iterator& impl) noexcept
: impl_(impl)
{}

template<typename T, class Tag>
auto const_linked_set_iterator_<T, Tag>::operator*() const noexcept ->
    const T& {
  return static_cast<const T&>(
      static_cast<const linked_set_element<T, Tag>&>(*impl_));
}

template<typename T, class Tag>
auto const_linked_set_iterator_<T, Tag>::operator->() const noexcept ->
    const T* {
  return static_cast<const T*>(
      static_cast<const linked_set_element<T, Tag>*>(*impl_));
}

template<typename T, class Tag>
auto const_linked_set_iterator_<T, Tag>::operator++() noexcept ->
    const_linked_set_iterator_& {
  ++impl_;
  return *this;
}

template<typename T, class Tag>
auto const_linked_set_iterator_<T, Tag>::operator++(int) noexcept ->
    const_linked_set_iterator_ {
  return const_linked_set_iterator_(impl_++);
}

template<typename T, class Tag>
auto const_linked_set_iterator_<T, Tag>::operator--() noexcept ->
    const_linked_set_iterator_& {
  --impl_;
  return *this;
}

template<typename T, class Tag>
auto const_linked_set_iterator_<T, Tag>::operator--(int) noexcept ->
    const_linked_set_iterator_ {
  return const_linked_set_iterator_(impl_--);
}

template<typename T, class Tag>
auto const_linked_set_iterator_<T, Tag>::operator==(
    const linked_set_iterator_<T, Tag>& o) const noexcept -> bool {
  return impl_ == o.impl_;
}

template<typename T, class Tag>
auto const_linked_set_iterator_<T, Tag>::operator!=(
    const linked_set_iterator_<T, Tag>& o) const noexcept -> bool {
  return impl_ != o.impl_;
}

template<typename T, class Tag>
auto const_linked_set_iterator_<T, Tag>::operator==(
    const const_linked_set_iterator_& o) const noexcept -> bool {
  return impl_ == o.impl_;
}

template<typename T, class Tag>
auto const_linked_set_iterator_<T, Tag>::operator!=(
    const const_linked_set_iterator_& o) const noexcept -> bool {
  return impl_ != o.impl_;
}


template<typename T, class Tag, typename Cmp, typename... Augments>
void swap(linked_set<T, Tag, Cmp, Augments...>& x,
          linked_set<T, Tag, Cmp, Augments...>& y) noexcept {
  x.swap(y);
}


_namespace_end(ilias)

#endif /* _ILIAS_LINKED_SET_INL_H_ */
