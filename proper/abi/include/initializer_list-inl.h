#ifndef _INITIALIZER_LIST_INL_H_
#define _INITIALIZER_LIST_INL_H_

#include <cdecl.h>
#include <initializer_list>

_namespace_begin(std)


template<typename E>
constexpr initializer_list<E>::initializer_list(
    const initializer_list<E>::value_type* p,
    initializer_list<E>::size_type s)
: begin_(p),
  sz_(s)
{}

template<typename E>
inline typename initializer_list<E>::const_iterator initializer_list<E>::begin()
    const noexcept {
  return begin_;
}

template<typename E>
inline typename initializer_list<E>::const_iterator initializer_list<E>::end()
    const noexcept {
  return begin_ + sz_;
}

template<typename E>
inline typename initializer_list<E>::iterator initializer_list<E>::begin()
    noexcept {
  return begin_;
}

template<typename E>
inline typename initializer_list<E>::iterator initializer_list<E>::end()
    noexcept {
  return begin_ + sz_;
}

template<typename E>
inline typename initializer_list<E>::size_type initializer_list<E>::size()
    const noexcept {
  return sz_;
}

template<typename E>
inline bool initializer_list<E>::empty() const noexcept {
  return sz_ == 0;
}


template<typename E>
inline typename initializer_list<E>::iterator begin(initializer_list<E>& l)
    noexcept {
  return l.begin();
}
template<typename E>
inline typename initializer_list<E>::iterator end(initializer_list<E>& l)
    noexcept {
  return l.end();
}

template<typename E>
inline typename initializer_list<E>::const_iterator begin(
    const initializer_list<E>& l) noexcept {
  return l.begin();
}
template<typename E>
inline typename initializer_list<E>::const_iterator end(
    const initializer_list<E>& l) noexcept {
  return l.end();
}


_namespace_end(std)

#endif /* _INITIALIZER_LIST_INL_H_ */
