#ifndef _ILIAS_LINKED_LIST_INL_H_
#define _ILIAS_LINKED_LIST_INL_H_

#include <cassert>
#include <functional>
#include <utility>

_namespace_begin(ilias)


inline basic_linked_list::element::element(link_to_self) noexcept
: succ_(this),
  pred_(this)
{}

constexpr basic_linked_list::element::element(const element&) noexcept
: element()
{}

inline auto basic_linked_list::element::operator=(const element&) noexcept ->
    element& {
  return *this;
}


inline basic_linked_list::basic_linked_list(basic_linked_list&& other)
    noexcept {
  using _namespace(std)::exchange;

  if (!other.empty()) {
    root_.succ_ = exchange(other.root_.succ_, &other.root_);
    root_.pred_ = exchange(other.root_.pred_, &other.root_);
  }
}

inline auto basic_linked_list::link_front(element* e) noexcept -> void {
  using _namespace(std)::exchange;

  assert(e->succ_ == nullptr && e->pred_ == nullptr);
  e->pred_ = &root_;
  e->succ_ = _namespace(std)::exchange(root_.succ_, e);
}

inline auto basic_linked_list::link_back(element* e) noexcept -> void {
  using _namespace(std)::exchange;

  assert(e->succ_ == nullptr && e->pred_ == nullptr);
  e->succ_ = &root_;
  e->pred_ = exchange(root_.pred_, e);
}

inline auto basic_linked_list::unlink_front() noexcept -> element* {
  element* rv = root_.succ_;
  if (rv == &root_) {
    rv = nullptr;
    return rv;
  }

  root_.succ_ = rv->succ_;
  rv->succ_ = nullptr;
  rv->pred_ = nullptr;
  return rv;
}

inline auto basic_linked_list::unlink_back() noexcept -> element* {
  element* rv = root_.pred_;
  if (rv == &root_) {
    rv = nullptr;
    return rv;
  }

  root_.pred_ = rv->pred_;
  rv->succ_ = nullptr;
  rv->pred_ = nullptr;
  return rv;
}

inline auto basic_linked_list::link(iterator i, element* e) noexcept ->
    iterator {
  using _namespace(std)::exchange;

  assert(e->succ_ == nullptr && e->pred_ == nullptr);
  e->succ_ = &*i;
  e->pred_ = exchange(i->pred_, e);
  e->pred_->succ_ = e;
  return iterator{ e };
}

inline auto basic_linked_list::unlink(iterator i) noexcept -> element* {
  i->pred_->succ_ = i->succ_;
  i->succ_->pred_ = i->pred_;
  i->pred_ = nullptr;
  i->succ_ = nullptr;
  return &*i;
}

inline auto basic_linked_list::unlink(element* e) noexcept -> element* {
  if (e->succ_ == nullptr) {
    assert(e->pred_ == nullptr);
    return nullptr;
  }
  assert(e->pred_ != nullptr);

  e->pred_->succ_ = e->succ_;
  e->succ_->pred_ = e->pred_;
  e->pred_ = nullptr;
  e->succ_ = nullptr;
  return e;
}

inline auto basic_linked_list::splice(iterator i, basic_linked_list& o)
    noexcept -> void {
  if (o.empty()) return;

  element* s = &*i;  // May be root_.
  element* p = i->pred_;  // May be root_.
  element* first = _namespace(std)::exchange(o.root_.succ_, &o.root_);
  element* last = _namespace(std)::exchange(o.root_.pred_, &o.root_);

  last->succ_ = s;
  s->pred_ = last;
  first->pred_ = p;
  p->succ_ = first;
}

inline auto basic_linked_list::splice(iterator i, iterator o_b, iterator o_e)
    noexcept -> void {
  if (o_b == o_e) return;

  element* s = &*i;  // May be root_.
  element* p = i->pred_;  // May be root_.
  element* first = &*o_b;
  element* last = o_e->pred_;

  last->succ_->pred_ = first->pred_;
  first->pred_->succ_ = last->succ_;

  last->succ_ = s;
  s->pred_ = last;
  first->pred_ = p;
  p->succ_ = first;
}

inline auto basic_linked_list::begin() const noexcept -> iterator {
  return iterator{ root_.succ_ };
}

inline auto basic_linked_list::end() const noexcept -> iterator {
  return iterator{ const_cast<element*>(&root_) };
}

inline auto basic_linked_list::empty() const noexcept -> bool {
  return root_.succ_ == &root_;
}

inline auto basic_linked_list::swap(basic_linked_list& other) noexcept ->
    void {
  using _namespace(std)::swap;
  using _namespace(std)::exchange;

  if (!empty() && !other.empty()) {

    swap(root_.succ_, other.root_.succ_);
    swap(root_.pred_, other.root_.pred_);
  } else if (!empty()) {
    other.root_.succ_ = exchange(root_.succ_, &root_);
    other.root_.pred_ = exchange(root_.pred_, &root_);
  } else if (!other.empty()) {
    root_.succ_ = exchange(other.root_.succ_, &root_);
    root_.pred_ = exchange(other.root_.pred_, &root_);
  }
}


inline basic_linked_list::iterator::iterator(element* e) noexcept
: elem_(e)
{}

inline auto basic_linked_list::iterator::operator++() noexcept -> iterator& {
  elem_ = elem_->succ_;
  return *this;
}

inline auto basic_linked_list::iterator::operator++(int) noexcept -> iterator {
  iterator tmp = *this;
  ++*this;
  return tmp;
}

inline auto basic_linked_list::iterator::operator--() noexcept -> iterator& {
  elem_ = elem_->pred_;
  return *this;
}

inline auto basic_linked_list::iterator::operator--(int) noexcept -> iterator {
  iterator tmp = *this;
  --*this;
  return tmp;
}

inline auto basic_linked_list::iterator::operator*() const noexcept ->
    element& {
  return *elem_;
}

inline auto basic_linked_list::iterator::operator->() const noexcept ->
    element* {
  return elem_;
}


template<typename T, class Tag>
auto linked_list<T, Tag>::link_front(pointer p) noexcept -> void {
  this->basic_linked_list::link_front(down_cast_(p));
}

template<typename T, class Tag>
auto linked_list<T, Tag>::link_back(pointer p) noexcept -> void {
  this->basic_linked_list::link_front(down_cast_(p));
}

template<typename T, class Tag>
auto linked_list<T, Tag>::unlink_front() noexcept -> pointer {
  return up_cast_(this->basic_linked_list::unlink_front());
}

template<typename T, class Tag>
auto linked_list<T, Tag>::unlink_back() noexcept -> pointer {
  return up_cast_(this->basic_linked_list::unlink_back());
}

template<typename T, class Tag>
auto linked_list<T, Tag>::link(const_iterator i, pointer p) noexcept ->
    iterator {
  return iterator(this->basic_linked_list::link(i.impl_, down_cast_(p)));
}

template<typename T, class Tag>
auto linked_list<T, Tag>::unlink(const_iterator i) noexcept -> pointer {
  return up_cast_(this->basic_linked_list::unlink(i.impl_));
}

template<typename T, class Tag>
auto linked_list<T, Tag>::unlink(const_pointer p) noexcept -> pointer {
  return up_cast_(this->basic_linked_list::unlink(down_cast_(p)));
}

template<typename T, class Tag>
auto linked_list<T, Tag>::front() noexcept -> reference {
  return *begin();
}

template<typename T, class Tag>
auto linked_list<T, Tag>::front() const noexcept -> const_reference {
  return *begin();
}

template<typename T, class Tag>
auto linked_list<T, Tag>::back() noexcept -> reference {
  return *--end();
}

template<typename T, class Tag>
auto linked_list<T, Tag>::back() const noexcept -> const_reference {
  return *--end();
}

template<typename T, class Tag>
auto linked_list<T, Tag>::splice(const_iterator i, linked_list& o)
    noexcept -> void {
  basic_linked_list::splice(i.impl_, o);
}

template<typename T, class Tag>
auto linked_list<T, Tag>::splice(const_iterator i, linked_list&& o)
    noexcept -> void {
  splice(i, o);
}

template<typename T, class Tag>
auto linked_list<T, Tag>::splice(const_iterator i,
                                 const_iterator o_b, const_iterator o_e)
    noexcept -> void {
  basic_linked_list::splice(i.impl_, o_b.impl_, o_e.impl_);
}

template<typename T, class Tag>
template<typename Compare>
auto linked_list<T, Tag>::merge(const_iterator b1, const_iterator e1,
                                const_iterator b2, const_iterator e2,
                                Compare compare) -> void {
  using _namespace(std)::ref;
  using _namespace(std)::placeholders::_1;

  /* Find the first element after other.begin(). */
  b1 = find_if(b1, e1,
               bind(ref(compare), ref(*b2), _1));
  while (b1 != e1 && b2 != e2) {
    /* Collect everything that should be before the found element. */
    const_iterator b2_e = find_if_not(b2, e2,
                                      bind(ref(compare), _1, ref(*b1)));
    /* Splice that into the list. */
    splice(b1, b2, b2_e);
    b2 = b2_e;  // Update b2, everything before b2_e now lives before b1.
    /*
     * Find the first element after other.begin()
     * Note that other.begin() points to a different element now,
     * due to the splice operation.
     */
    b1 = find_if(next(b1), e1,
                 bind(ref(compare), ref(*b2), _1));
  }

  /* Splice the remainder of the other list at the end. */
  if (b2 != e2) splice(b1, b2, e2);
}

template<typename T, class Tag>
template<typename Compare>
auto linked_list<T, Tag>::sort(const_iterator b, const_iterator e,
                               Compare compare, size_t dist) -> void {
  using _namespace(std)::ref;

  if (dist < 2) {
    assert(dist == size_t(distance(b, e)));
    return;
  }

  size_t half_dist = dist / 2;
  const_iterator halfway_point = next(b, half_dist);

  sort(b, halfway_point, ref(compare), half_dist);
  sort(halfway_point, e, ref(compare), dist - half_dist);
  merge(b, halfway_point, halfway_point, e, ref(compare));
}

template<typename T, class Tag>
template<typename Compare>
auto linked_list<T, Tag>::sort(const_iterator b, const_iterator e,
                               Compare compare) -> void {
  sort(b, e, ref(compare), distance(b, e));
}

template<typename T, class Tag>
auto linked_list<T, Tag>::begin() noexcept -> iterator {
  return iterator(this->basic_linked_list::begin());
}

template<typename T, class Tag>
auto linked_list<T, Tag>::end() noexcept -> iterator {
  return iterator(this->basic_linked_list::end());
}

template<typename T, class Tag>
auto linked_list<T, Tag>::rbegin() noexcept -> reverse_iterator {
  return reverse_iterator(end());
}

template<typename T, class Tag>
auto linked_list<T, Tag>::rend() noexcept -> reverse_iterator {
  return reverse_iterator(begin());
}

template<typename T, class Tag>
auto linked_list<T, Tag>::begin() const noexcept -> const_iterator {
  return const_iterator(this->basic_linked_list::begin());
}

template<typename T, class Tag>
auto linked_list<T, Tag>::end() const noexcept -> const_iterator {
  return const_iterator(this->basic_linked_list::end());
}

template<typename T, class Tag>
auto linked_list<T, Tag>::rbegin() const noexcept -> const_reverse_iterator {
  return const_reverse_iterator(end());
}

template<typename T, class Tag>
auto linked_list<T, Tag>::rend() const noexcept -> const_reverse_iterator {
  return const_reverse_iterator(begin());
}

template<typename T, class Tag>
auto linked_list<T, Tag>::cbegin() const noexcept -> const_iterator {
  return begin();
}

template<typename T, class Tag>
auto linked_list<T, Tag>::cend() const noexcept -> const_iterator {
  return end();
}

template<typename T, class Tag>
auto linked_list<T, Tag>::crbegin() const noexcept -> const_reverse_iterator {
  return rbegin();
}

template<typename T, class Tag>
auto linked_list<T, Tag>::crend() const noexcept -> const_reverse_iterator {
  return rend();
}

template<typename T, class Tag>
auto linked_list<T, Tag>::swap(linked_list& o) noexcept -> void {
  this->basic_linked_list::swap(o);
}

template<typename T, class Tag>
auto linked_list<T, Tag>::nonconst_iterator(const_iterator i) noexcept ->
    iterator {
  return iterator(i.impl_);
}

template<typename T, class Tag>
auto linked_list<T, Tag>::down_cast_(pointer p) noexcept -> element* {
  if (!p) return nullptr;
  return static_cast<element*>(
      static_cast<linked_list_element<Tag>*>(p));
}

template<typename T, class Tag>
auto linked_list<T, Tag>::down_cast_(const_pointer p) noexcept -> element* {
  if (!p) return nullptr;
  return const_cast<element*>(static_cast<const element*>(
      static_cast<const linked_list_element<Tag>*>(p)));
}

template<typename T, class Tag>
auto linked_list<T, Tag>::up_cast_(element* e) noexcept -> pointer {
  if (!e) return nullptr;
  return static_cast<pointer>(static_cast<linked_list_element<Tag>*>(e));
}


template<typename T, class Tag>
linked_list<T, Tag>::iterator::iterator(
    const basic_linked_list::iterator& i) noexcept
: impl_(i)
{}

template<typename T, class Tag>
auto linked_list<T, Tag>::iterator::operator*() const noexcept -> T& {
  return *up_cast_(&*impl_);
}

template<typename T, class Tag>
auto linked_list<T, Tag>::iterator::operator->() const noexcept -> T* {
  return up_cast_(&*impl_);
}

template<typename T, class Tag>
auto linked_list<T, Tag>::iterator::operator++() noexcept -> iterator& {
  ++impl_;
  return *this;
}

template<typename T, class Tag>
auto linked_list<T, Tag>::iterator::operator++(int) noexcept -> iterator {
  iterator tmp = *this;
  ++*this;
  return tmp;
}

template<typename T, class Tag>
auto linked_list<T, Tag>::iterator::operator--() noexcept -> iterator& {
  --impl_;
  return *this;
}

template<typename T, class Tag>
auto linked_list<T, Tag>::iterator::operator--(int) noexcept -> iterator {
  iterator tmp = *this;
  --*this;
  return tmp;
}

template<typename T, class Tag>
auto linked_list<T, Tag>::iterator::operator==(const iterator& other)
    const noexcept -> bool {
  return impl_ == other.impl_;
}

template<typename T, class Tag>
auto linked_list<T, Tag>::iterator::operator!=(const iterator& other)
    const noexcept -> bool {
  return !(*this == other);
}

template<typename T, class Tag>
auto linked_list<T, Tag>::iterator::operator==(const const_iterator& other)
    const noexcept -> bool {
  return other == *this;
}

template<typename T, class Tag>
auto linked_list<T, Tag>::iterator::operator!=(const const_iterator& other)
    const noexcept -> bool {
  return !(*this == other);
}


template<typename T, class Tag>
linked_list<T, Tag>::const_iterator::const_iterator(
    const basic_linked_list::iterator& i) noexcept
: impl_(i)
{}

template<typename T, class Tag>
linked_list<T, Tag>::const_iterator::const_iterator(
    const iterator& other) noexcept
: impl_(other.impl_)
{}

template<typename T, class Tag>
auto linked_list<T, Tag>::const_iterator::operator*() const noexcept ->
    const T& {
  return *up_cast_(&*impl_);
}

template<typename T, class Tag>
auto linked_list<T, Tag>::const_iterator::operator->() const noexcept ->
    const T* {
  return up_cast_(&*impl_);
}

template<typename T, class Tag>
auto linked_list<T, Tag>::const_iterator::operator++() noexcept ->
    const_iterator& {
  ++impl_;
  return *this;
}

template<typename T, class Tag>
auto linked_list<T, Tag>::const_iterator::operator++(int) noexcept ->
    const_iterator {
  const_iterator tmp = *this;
  ++*this;
  return tmp;
}

template<typename T, class Tag>
auto linked_list<T, Tag>::const_iterator::operator--() noexcept ->
    const_iterator& {
  --impl_;
  return *this;
}

template<typename T, class Tag>
auto linked_list<T, Tag>::const_iterator::operator--(int) noexcept ->
    const_iterator {
  const_iterator tmp = *this;
  --*this;
  return tmp;
}

template<typename T, class Tag>
auto linked_list<T, Tag>::const_iterator::operator==(const iterator& other)
    const noexcept -> bool {
  return impl_ == other.impl_;
}

template<typename T, class Tag>
auto linked_list<T, Tag>::const_iterator::operator!=(const iterator& other)
    const noexcept -> bool {
  return !(*this == other);
}

template<typename T, class Tag>
auto linked_list<T, Tag>::const_iterator::operator==(
    const const_iterator& other) const noexcept -> bool {
  return impl_ == other.impl_;
}

template<typename T, class Tag>
auto linked_list<T, Tag>::const_iterator::operator!=(
    const const_iterator& other) const noexcept -> bool {
  return !(*this == other);
}


inline bool operator==(const basic_linked_list::iterator& a,
                const basic_linked_list::iterator& b) noexcept {
  return a.elem_ == b.elem_;
}
inline bool operator!=(const basic_linked_list::iterator& a,
                const basic_linked_list::iterator& b) noexcept {
  return !(a == b);
}
inline void swap(basic_linked_list& a, basic_linked_list& b) noexcept {
  a.swap(b);
}
template<typename T, class Tag>
void swap(linked_list<T, Tag>& a, linked_list<T, Tag>& b) noexcept {
  a.swap(b);
}


_namespace_end(ilias)

#endif /* _ILIAS_LINKED_LIST_INL_H_ */
