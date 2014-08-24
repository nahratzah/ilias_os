#ifndef _ILIAS_LINKED_FORWARD_LIST_INL_H_
#define _ILIAS_LINKED_FORWARD_LIST_INL_H_

#include <ilias/linked_forward_list.h>
#include <cassert>
#include <algorithm>
#include <functional>
#include <utility>

_namespace_begin(ilias)


constexpr basic_linked_forward_list::element::element(const element&) noexcept
: element()
{}

inline auto basic_linked_forward_list::element::operator=(const element&)
    noexcept -> element& {
  return *this;
}


inline basic_linked_forward_list::basic_linked_forward_list(
    basic_linked_forward_list&& rhs) noexcept {
  before_.succ_ = _namespace(std)::exchange(rhs.before_.succ_, nullptr);
}

inline auto basic_linked_forward_list::link_front(element* e) noexcept ->
    void {
  using _namespace(std)::exchange;

  assert(e->succ_ == nullptr);
  e->succ_ = exchange(before_.succ_, e);
}

inline auto basic_linked_forward_list::link_after(iterator i, element* e)
    noexcept -> iterator {
  using _namespace(std)::exchange;

  e->succ_ = exchange(i->succ_, e);
  return iterator{ e };
}

inline auto basic_linked_forward_list::unlink_front() noexcept -> element* {
  using _namespace(std)::exchange;

  element* e = before_.succ_;
  if (e) before_.succ_ = exchange(e->succ_, nullptr);
  return e;
}

inline auto basic_linked_forward_list::unlink_after(iterator i) noexcept ->
    element* {
  return unlink_after(i.elem_);
}

inline auto basic_linked_forward_list::unlink_after(element* pred) noexcept ->
    element* {
  using _namespace(std)::exchange;

  element* e = pred->succ_;
  assert(e != nullptr);
  pred->succ_ = exchange(e->succ_, nullptr);
  return e;
}

inline auto basic_linked_forward_list::unlink(iterator i) noexcept ->
    element* {
  return unlink(i.elem_);
}

template<typename Compare>
auto basic_linked_forward_list::merge(ptr_iterator b1, ptr_iterator e1,
                                      ptr_iterator b2, ptr_iterator e2,
                                      Compare compare) ->
    _namespace(std)::tuple<ptr_iterator, ptr_iterator> {
  using _namespace(std)::ref;
  using _namespace(std)::bind;
  using _namespace(std)::placeholders::_1;
  using _namespace(std)::find_if;
  using _namespace(std)::find_if_not;
  using _namespace(std)::make_tuple;

  if (b2 == e2) return make_tuple(b1, e1);
  const ptr_iterator saved_b1 = b1;

  /* Find b1, such that b2 < b1. */
  b1 = find_if(b1, e1,
               bind(ref(compare), ref(*b2), _1));

  /*
   * Loop invariant:
   * -  b2 < b1
   * -  b2 != e2  (i.e. not empty)
   * -  b1 != e1  (i.e. not empty)
   */
  while (b1 != e1) {
    /* Find b2_e, such that [b2..b2_e) < b1 && !(b2_e < b1). */
    ptr_iterator b2_e = find_if_not(next(b2), e2,
                                    bind(ref(compare), _1, ref(*b1)));

    /* If b2_e == e2, e2 will be invalidated during the splice operation.
     * b2_e will be invalidated unconditionally. */
    const bool splice_everything = (b2_e == e2);

    /* Splice [b2..b2_e) into list, before b1. */
    tie(b1, b2) = splice(b1, b2, b2_e);
    if (splice_everything)
      return make_tuple(saved_b1, e1);  // Last merge completed.

    /*
     * Note: no need to update b2, since it points at its predecessor;
     * b2 now points at b2_e.
     */

    /* Find b1, such that b2 < b1. */
    b1 = find_if(next(b1), e1,
                 bind(ref(compare), ref(*b2), _1));
  }

  /*
   * b1 == e1  -- because loop guard
   * b2 != e2  -- because if b2 == e2, the code above forces an early return
   */
  tie(b1, b2) = splice(b1, b2, e2);
  return make_tuple(saved_b1, b1);
}

template<typename Compare>
auto basic_linked_forward_list::merge(basic_linked_forward_list& other,
                                      Compare compare) -> void {
  using _namespace(std)::ref;

  assert(this != &other);

  merge(ptr_iterator(*this), ptr_iterator(*this, end()),
        ptr_iterator(other), ptr_iterator(other, end()),
        ref(compare));
}

template<typename Compare>
auto basic_linked_forward_list::sort(ptr_iterator b, ptr_iterator e,
                                     Compare compare, size_t dist) ->
    _namespace(std)::tuple<ptr_iterator, ptr_iterator> {
  if (dist < 2) {
    assert(_namespace(std)::distance(b, e) - dist == 0);
    return make_tuple(b, e);
  }

  size_t half_dist = dist / 2;
  ptr_iterator halfway_point = next(b, half_dist);

  tie(b, halfway_point) = sort(b, halfway_point, ref(compare),
                               half_dist);
  tie(halfway_point, e) = sort(halfway_point, e, ref(compare),
                               dist - half_dist);
  return merge(b, halfway_point, halfway_point, e, ref(compare));
}

template<typename Compare>
auto basic_linked_forward_list::sort(ptr_iterator b, ptr_iterator e,
                                     Compare compare) ->
    _namespace(std)::tuple<ptr_iterator, ptr_iterator> {
  return sort(b, e, ref(compare), _namespace(std)::distance(b, e));
}

template<typename Compare>
auto basic_linked_forward_list::sort(Compare compare, size_t dist) -> void {
  return sort(ptr_iterator(*this), ptr_iterator(*this, end()),
              ref(compare), dist);
}

template<typename Compare>
auto basic_linked_forward_list::sort(Compare compare) -> void {
  sort(ptr_iterator(*this), ptr_iterator(*this, end()), ref(compare));
}

inline auto basic_linked_forward_list::before_begin() const noexcept ->
    iterator {
  return iterator{ const_cast<element*>(&before_) };
}

inline auto basic_linked_forward_list::begin() const noexcept -> iterator {
  return iterator{ before_.succ_ };
}

inline auto basic_linked_forward_list::end() const noexcept -> iterator {
  return iterator();
}

inline auto basic_linked_forward_list::empty() const noexcept -> bool {
  return before_.succ_ == nullptr;
}

inline auto basic_linked_forward_list::swap(basic_linked_forward_list& o)
    noexcept -> void {
  using _namespace(std)::swap;

  swap(before_.succ_, o.before_.succ_);
}


inline bool operator==(const basic_linked_forward_list::iterator& a,
                       const basic_linked_forward_list::iterator& b) noexcept {
  return a.elem_ == b.elem_;
}
inline bool operator!=(const basic_linked_forward_list::iterator& a,
                       const basic_linked_forward_list::iterator& b) noexcept {
  return !(a == b);
}

inline auto swap(basic_linked_forward_list& a, basic_linked_forward_list& b)
    noexcept -> void {
  a.swap(b);
}


inline basic_linked_forward_list::iterator::iterator(element* e) noexcept
: elem_(e)
{}

inline auto basic_linked_forward_list::iterator::operator++() noexcept ->
    iterator& {
  elem_ = elem_->succ_;
  return *this;
}

inline auto basic_linked_forward_list::iterator::operator++(int) noexcept ->
    iterator {
  iterator tmp = *this;
  ++*this;
  return tmp;
}

inline auto basic_linked_forward_list::iterator::operator*() const noexcept ->
    element& {
  return *elem_;
}

inline auto basic_linked_forward_list::iterator::operator->() const noexcept ->
    element* {
  return elem_;
}

inline auto basic_linked_forward_list::iterator::operator==(
    const ptr_iterator& i) const noexcept -> bool {
  return i == *this;
}

inline auto basic_linked_forward_list::iterator::operator!=(
    const ptr_iterator& i) const noexcept -> bool {
  return i != *this;
}


inline basic_linked_forward_list::ptr_iterator::ptr_iterator(
    element*& e) noexcept
: ptr_(&e)
{}

inline basic_linked_forward_list::ptr_iterator::ptr_iterator(
    basic_linked_forward_list& list) noexcept
: ptr_iterator(list.before_.succ_)
{}

inline basic_linked_forward_list::ptr_iterator::ptr_iterator(
    basic_linked_forward_list& list,
    basic_linked_forward_list::iterator i) noexcept
: ptr_iterator(list.find_succ_for_(i.elem_))
{}

inline basic_linked_forward_list::ptr_iterator::ptr_iterator(
    basic_linked_forward_list::iterator i, from_before_iter_t)
: ptr_iterator(i->succ_)
{}

inline basic_linked_forward_list::ptr_iterator::
    operator basic_linked_forward_list::iterator()
    const noexcept {
  if (!ptr_) return basic_linked_forward_list::iterator();
  return basic_linked_forward_list::iterator(*ptr_);
}

inline auto basic_linked_forward_list::ptr_iterator::operator++() noexcept ->
    ptr_iterator& {
  ptr_ = &(*ptr_)->succ_;
  return *this;
}

inline auto basic_linked_forward_list::ptr_iterator::operator++(int)
    noexcept -> ptr_iterator {
  ptr_iterator tmp = *this;
  ++*this;
  return tmp;
}

inline auto basic_linked_forward_list::ptr_iterator::operator*()
    const noexcept -> element& {
  return **ptr_;
}

inline auto basic_linked_forward_list::ptr_iterator::operator->()
    const noexcept -> element* {
  return *ptr_;
}

inline auto basic_linked_forward_list::ptr_iterator::get_ptr()
    const noexcept -> element*& {
  assert(ptr_);
  return *ptr_;
}

inline auto basic_linked_forward_list::ptr_iterator::operator==(
    const ptr_iterator& i) const noexcept -> bool {
  return ptr_ == i.ptr_;
}

inline auto basic_linked_forward_list::ptr_iterator::operator!=(
    const ptr_iterator& i) const noexcept -> bool {
  return !(*this == i);
}

inline auto basic_linked_forward_list::ptr_iterator::operator==(
    const basic_linked_forward_list::iterator& i) const noexcept -> bool {
  return basic_linked_forward_list::iterator(*this) == i;
}

inline auto basic_linked_forward_list::ptr_iterator::operator!=(
    const basic_linked_forward_list::iterator& i) const noexcept -> bool {
  return !(*this == i);
}


template<typename T, class Tag>
auto linked_forward_list<T, Tag>::link_front(pointer p) noexcept -> void {
  this->basic_linked_forward_list::link_front(down_cast_(p));
}

template<typename T, class Tag>
auto linked_forward_list<T, Tag>::link_after(const_iterator i, pointer p)
    noexcept -> iterator {
  return iterator(this->basic_linked_forward_list::link_after(i.impl_,
                                                              down_cast_(p)));
}

template<typename T, class Tag>
auto linked_forward_list<T, Tag>::unlink_front() noexcept -> pointer {
  return up_cast_(this->basic_linked_forward_list::unlink_front());
}

template<typename T, class Tag>
auto linked_forward_list<T, Tag>::unlink_after(const_iterator i) noexcept ->
    pointer {
  return up_cast_(this->basic_linked_forward_list::unlink_after(i.impl_));
}

template<typename T, class Tag>
auto linked_forward_list<T, Tag>::unlink_after(const_pointer p) noexcept ->
    pointer {
  return up_cast_(this->basic_linked_forward_list::unlink_after(
                      down_cast_(p)));
}

template<typename T, class Tag>
auto linked_forward_list<T, Tag>::unlink(const_iterator i) noexcept ->
    pointer {
  return up_cast_(this->basic_linked_forward_list::unlink(i.impl_));
}

template<typename T, class Tag>
auto linked_forward_list<T, Tag>::unlink(const_pointer p) noexcept ->
    pointer {
  return up_cast_(this->basic_linked_forward_list::unlink(down_cast_(p)));
}

template<typename T, class Tag>
auto linked_forward_list<T, Tag>::front() noexcept -> reference {
  return *begin();
}

template<typename T, class Tag>
auto linked_forward_list<T, Tag>::front() const noexcept -> const_reference {
  return *begin();
}

template<typename T, class Tag>
auto linked_forward_list<T, Tag>::splice_after(const_iterator i,
                                               linked_forward_list&& o)
    noexcept -> void {
  this->basic_linked_forward_list::splice_after(i.impl_, o);
}

template<typename T, class Tag>
auto linked_forward_list<T, Tag>::splice_after(const_iterator i,
                                               const_iterator o_b,
                                               const_iterator o_e)
    noexcept -> void {
  basic_linked_forward_list::splice_after(i.impl_, o_b.impl_, o_e.impl_);
}

template<typename T, class Tag>
template<typename Compare>
auto linked_forward_list<T, Tag>::merge(linked_forward_list& other,
                                        Compare compare) -> void {
  using _namespace(std)::ref;
  using _namespace(std)::bind;
  using _namespace(std)::placeholders::_1;
  using _namespace(std)::placeholders::_2;

  this->basic_linked_forward_list::merge(other,
                                         bind(ref(compare),
                                              bind(&up_cast_cref_, _1),
                                              bind(&up_cast_cref_, _2)));
}

template<typename T, class Tag>
auto linked_forward_list<T, Tag>::merge(linked_forward_list& other) ->
    void {
  this->merge(other, _namespace(std)::less<value_type>());
}

template<typename T, class Tag>
template<typename Compare>
auto linked_forward_list<T, Tag>::sort(Compare compare) -> void {
  using _namespace(std)::ref;
  using _namespace(std)::bind;
  using _namespace(std)::placeholders::_1;
  using _namespace(std)::placeholders::_2;

  this->basic_linked_forward_list::sort(
      bind(ref(compare), bind(&up_cast_cref_, _1), bind(&up_cast_cref_, _2)));
}

template<typename T, class Tag>
auto linked_forward_list<T, Tag>::sort() -> void {
  this->sort(_namespace(std)::less<value_type>());
}

template<typename T, class Tag>
auto linked_forward_list<T, Tag>::before_begin() noexcept -> iterator {
  return iterator(this->basic_linked_forward_list::before_begin());
}

template<typename T, class Tag>
auto linked_forward_list<T, Tag>::begin() noexcept -> iterator {
  return iterator(this->basic_linked_forward_list::begin());
}

template<typename T, class Tag>
auto linked_forward_list<T, Tag>::end() noexcept -> iterator {
  return iterator(this->basic_linked_forward_list::end());
}

template<typename T, class Tag>
auto linked_forward_list<T, Tag>::before_begin() const noexcept ->
    const_iterator {
  return const_iterator(this->basic_linked_forward_list::before_begin());
}

template<typename T, class Tag>
auto linked_forward_list<T, Tag>::begin() const noexcept -> const_iterator {
  return const_iterator(this->basic_linked_forward_list::begin());
}

template<typename T, class Tag>
auto linked_forward_list<T, Tag>::end() const noexcept -> const_iterator {
  return const_iterator(this->basic_linked_forward_list::end());
}

template<typename T, class Tag>
auto linked_forward_list<T, Tag>::cbefore_begin() const noexcept ->
    const_iterator {
  return before_begin();
}

template<typename T, class Tag>
auto linked_forward_list<T, Tag>::cbegin() const noexcept -> const_iterator {
  return begin();
}

template<typename T, class Tag>
auto linked_forward_list<T, Tag>::cend() const noexcept -> const_iterator {
  return end();
}

template<typename T, class Tag>
auto linked_forward_list<T, Tag>::swap(linked_forward_list& o) noexcept ->
    void {
  this->basic_linked_forward_list::swap(o);
}

template<typename T, class Tag>
auto linked_forward_list<T, Tag>::nonconst_iterator(const_iterator i)
    noexcept -> iterator {
  return iterator(i.impl_);
}

template<typename T, class Tag>
auto linked_forward_list<T, Tag>::down_cast_(pointer p) noexcept -> element* {
  if (!p) return nullptr;
  return static_cast<element*>(
      static_cast<linked_forward_list_element<Tag>*>(p));
}

template<typename T, class Tag>
auto linked_forward_list<T, Tag>::down_cast_(const_pointer p) noexcept ->
    element* {
  if (!p) return nullptr;
  return const_cast<element*>(static_cast<const element*>(
      static_cast<const linked_forward_list_element<Tag>*>(p)));
}

template<typename T, class Tag>
auto linked_forward_list<T, Tag>::up_cast_(element* e) noexcept -> pointer {
  if (!e) return nullptr;
  return static_cast<pointer>(
      static_cast<linked_forward_list_element<Tag>*>(e));
}

template<typename T, class Tag>
auto linked_forward_list<T, Tag>::up_cast_cref_(const element& e) noexcept ->
    const_reference {
  return static_cast<const_reference>(
      static_cast<const linked_forward_list_element<Tag>&>(e));
}


template<typename T, class Tag>
linked_forward_list<T, Tag>::iterator::iterator(
    const basic_linked_forward_list::iterator& i) noexcept
: impl_(i)
{}

template<typename T, class Tag>
auto linked_forward_list<T, Tag>::iterator::operator*() const noexcept -> T& {
  return *up_cast_(&*impl_);
}

template<typename T, class Tag>
auto linked_forward_list<T, Tag>::iterator::operator->() const noexcept -> T* {
  return up_cast_(&*impl_);
}

template<typename T, class Tag>
auto linked_forward_list<T, Tag>::iterator::operator++() noexcept ->
    iterator& {
  ++impl_;
  return *this;
}

template<typename T, class Tag>
auto linked_forward_list<T, Tag>::iterator::operator++(int) noexcept ->
    iterator {
  iterator tmp = *this;
  ++*this;
  return tmp;
}

template<typename T, class Tag>
auto linked_forward_list<T, Tag>::iterator::operator==(const iterator& other)
    const noexcept -> bool {
  return impl_ == other.impl_;
}

template<typename T, class Tag>
auto linked_forward_list<T, Tag>::iterator::operator!=(const iterator& other)
    const noexcept -> bool {
  return !(*this == other);
}

template<typename T, class Tag>
auto linked_forward_list<T, Tag>::iterator::operator==(
    const const_iterator& other) const noexcept -> bool {
  return other == *this;
}

template<typename T, class Tag>
auto linked_forward_list<T, Tag>::iterator::operator!=(
    const const_iterator& other) const noexcept -> bool {
  return !(*this == other);
}

template<typename T, class Tag>
auto linked_forward_list<T, Tag>::iterator::get_unsafe_basic_iter()
    const noexcept -> basic_linked_forward_list::iterator {
  return impl_;
}

template<typename T, class Tag>
auto linked_forward_list<T, Tag>::iterator::from_unsafe_basic_iter(
    const basic_linked_forward_list::iterator& unsafe) noexcept ->
    iterator {
  return iterator(unsafe);
}


template<typename T, class Tag>
linked_forward_list<T, Tag>::const_iterator::const_iterator(
    const basic_linked_forward_list::iterator& i) noexcept
: impl_(i)
{}

template<typename T, class Tag>
linked_forward_list<T, Tag>::const_iterator::const_iterator(
    const iterator& other) noexcept
: impl_(other.impl_)
{}

template<typename T, class Tag>
auto linked_forward_list<T, Tag>::const_iterator::operator*()
    const noexcept -> const T& {
  return *up_cast_(&*impl_);
}

template<typename T, class Tag>
auto linked_forward_list<T, Tag>::const_iterator::operator->()
    const noexcept -> const T* {
  return up_cast_(&*impl_);
}

template<typename T, class Tag>
auto linked_forward_list<T, Tag>::const_iterator::operator++() noexcept ->
    const_iterator& {
  ++impl_;
  return *this;
}

template<typename T, class Tag>
auto linked_forward_list<T, Tag>::const_iterator::operator++(int) noexcept ->
    const_iterator {
  const_iterator tmp = *this;
  ++*this;
  return tmp;
}

template<typename T, class Tag>
auto linked_forward_list<T, Tag>::const_iterator::operator==(
    const iterator& other) const noexcept -> bool {
  return impl_ == other.impl_;
}

template<typename T, class Tag>
auto linked_forward_list<T, Tag>::const_iterator::operator!=(
    const iterator& other) const noexcept -> bool {
  return !(*this == other);
}

template<typename T, class Tag>
auto linked_forward_list<T, Tag>::const_iterator::operator==(
    const const_iterator& other) const noexcept -> bool {
  return impl_ == other.impl_;
}

template<typename T, class Tag>
auto linked_forward_list<T, Tag>::const_iterator::operator!=(
    const const_iterator& other) const noexcept -> bool {
  return !(*this == other);
}

template<typename T, class Tag>
auto linked_forward_list<T, Tag>::const_iterator::get_unsafe_basic_iter()
    const noexcept -> basic_linked_forward_list::iterator {
  return impl_;
}

template<typename T, class Tag>
auto linked_forward_list<T, Tag>::const_iterator::from_unsafe_basic_iter(
    const basic_linked_forward_list::iterator& unsafe) noexcept ->
    const_iterator {
  return const_iterator(unsafe);
}


template<typename T, class Tag>
void swap(linked_forward_list<T, Tag>& a, linked_forward_list<T, Tag>& b)
    noexcept {
  a.swap(b);
}


_namespace_end(ilias)

#endif /* _ILIAS_LINKED_FORWARD_LIST_INL_H_ */
