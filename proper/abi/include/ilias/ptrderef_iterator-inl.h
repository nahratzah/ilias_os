#ifndef _ILIAS_PTRDEREF_ITERATOR_INL_H_
#define _ILIAS_PTRDEREF_ITERATOR_INL_H_

#include <ilias/ptrderef_iterator.h>

_namespace_begin(ilias)


template<typename Iter>
template<typename OIter>
ptrderef_iterator<Iter>::ptrderef_iterator(
    const ptrderef_iterator<OIter>& o,
    _namespace(std)::enable_if_t<
        _namespace(std)::is_convertible<OIter, Iter>::value,
        enable_tag>)
: impl_(o.impl_)
{}

template<typename Iter>
auto ptrderef_iterator<Iter>::operator++() -> ptrderef_iterator& {
  ++impl_;
  return *this;
}

template<typename Iter>
auto ptrderef_iterator<Iter>::operator++(int) -> ptrderef_iterator {
  return ptrderef_iterator(impl_++);
}

template<typename Iter>
auto ptrderef_iterator<Iter>::operator--() -> ptrderef_iterator& {
  --impl_;
  return *this;
}

template<typename Iter>
auto ptrderef_iterator<Iter>::operator--(int) -> ptrderef_iterator {
  return ptrderef_iterator(impl_--);
}

template<typename Iter>
auto ptrderef_iterator<Iter>::operator+(difference_type d) const ->
    ptrderef_iterator {
  return ptrderef_iterator(impl_ + d);
}

template<typename Iter>
auto ptrderef_iterator<Iter>::operator-(difference_type d) const ->
    ptrderef_iterator {
  return ptrderef_iterator(impl_ - d);
}

template<typename Iter>
auto ptrderef_iterator<Iter>::operator+=(difference_type d) ->
    ptrderef_iterator& {
  impl_ += d;
  return *this;
}

template<typename Iter>
auto ptrderef_iterator<Iter>::operator-=(difference_type d) ->
    ptrderef_iterator& {
  impl_ -= d;
  return *this;
}

template<typename Iter>
auto ptrderef_iterator<Iter>::operator-(const ptrderef_iterator& o) const ->
    difference_type {
  return impl_ - o.impl_;
}

template<typename Iter>
auto ptrderef_iterator<Iter>::operator[](difference_type d) const ->
    reference {
  return *(*this + d);
}


template<typename Iter>
template<typename OIter>
auto ptrderef_iterator<Iter>::operator==(const ptrderef_iterator<OIter>& o)
    const -> bool {
  return impl_ == o.impl_;
}

template<typename Iter>
template<typename OIter>
auto ptrderef_iterator<Iter>::operator!=(const ptrderef_iterator<OIter>& o)
    const -> bool {
  return !(*this == o);
}


_namespace_end(ilias)

#endif /* _ILIAS_PTRDEREF_ITERATOR_INL_H_ */
