#ifndef _ILIAS_LINKED_FORWARD_LIST_INL_H_
#define _ILIAS_LINKED_FORWARD_LIST_INL_H_

#include <ilias/linked_forward_list.h>
#include <cassert>
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
    basic_linked_forward_list&& rhs) noexcept
: head_(_namespace(std)::exchange(rhs.head_, nullptr))
{}

inline auto basic_linked_forward_list::link_front(element* e) noexcept ->
    void {
  using _namespace(std)::exchange;

  assert(e->succ_ == nullptr);
  e->succ_ = exchange(head_, e);
}

inline auto basic_linked_forward_list::link_after(iterator i, element* e)
    noexcept -> iterator {
  using _namespace(std)::exchange;

  e->succ_ = exchange(i->succ_, e);
  return iterator{ e };
}

inline auto basic_linked_forward_list::unlink_front() noexcept -> element* {
  using _namespace(std)::exchange;

  element* e = head_;
  if (e) head_ = exchange(e->succ_, nullptr);
  return e;
}

inline auto basic_linked_forward_list::unlink(iterator i) noexcept ->
    element* {
  return unlink(i.elem_);
}

inline auto basic_linked_forward_list::begin() const noexcept -> iterator {
  return iterator{ head_ };
}

inline auto basic_linked_forward_list::end() const noexcept -> iterator {
  return iterator();
}

inline auto basic_linked_forward_list::empty() const noexcept -> bool {
  return head_ == nullptr;
}

inline auto basic_linked_forward_list::swap(basic_linked_forward_list& o)
    noexcept -> void {
  using _namespace(std)::swap;

  swap(head_, o.head_);
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
: ptr_iterator(list.head_)
{}

inline basic_linked_forward_list::ptr_iterator::ptr_iterator(
    basic_linked_forward_list& list,
    basic_linked_forward_list::iterator i) noexcept
: ptr_iterator(list.find_succ_for_(i.elem_))
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
auto linked_forward_list<T, Tag>::splice_front(linked_forward_list&& o)
    noexcept -> void {
  this->basic_linked_forward_list::splice_front(o);
}

template<typename T, class Tag>
auto linked_forward_list<T, Tag>::splice_front(linked_forward_list&& o,
                                               const_iterator o_b,
                                               const_iterator o_e)
    noexcept -> void {
  this->basic_linked_forward_list::splice_front(o, o_b.impl_, o_e.impl_);
}

template<typename T, class Tag>
auto linked_forward_list<T, Tag>::splice_after(const_iterator i,
                                               linked_forward_list&& o)
    noexcept -> void {
  this->basic_linked_forward_list::splice_after(i.impl_, o);
}

template<typename T, class Tag>
auto linked_forward_list<T, Tag>::splice_after(const_iterator i,
                                               linked_forward_list&& o,
                                               const_iterator o_b,
                                               const_iterator o_e)
    noexcept -> void {
  this->basic_linked_forward_list::splice_after(i.impl_,
                                                o, o_b.impl_, o_e.impl_);
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
auto linked_forward_list<T, Tag>::begin() const noexcept -> const_iterator {
  return const_iterator(this->basic_linked_forward_list::begin());
}

template<typename T, class Tag>
auto linked_forward_list<T, Tag>::end() const noexcept -> const_iterator {
  return const_iterator(this->basic_linked_forward_list::end());
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
void swap(linked_forward_list<T, Tag>& a, linked_forward_list<T, Tag>& b)
    noexcept {
  a.swap(b);
}


_namespace_end(ilias)

#endif /* _ILIAS_LINKED_FORWARD_LIST_INL_H_ */
