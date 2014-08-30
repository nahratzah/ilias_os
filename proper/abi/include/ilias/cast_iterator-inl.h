#ifndef _ILIAS_CAST_ITERATOR_INL_H_
#define _ILIAS_CAST_ITERATOR_INL_H_

#include <ilias/cast_iterator.h>

_namespace_begin(ilias)


template<typename Iter, typename T, cast_iterator_style Style,
         typename TPtr, typename TRef>
template<typename OIter, typename OT, cast_iterator_style OStyle,
         typename OTPtr, typename OTRef>
cast_iterator<Iter, T, Style, TPtr, TRef>::cast_iterator(
    const cast_iterator<OIter, OT, OStyle, OTPtr, OTRef>& o,
    _namespace(std)::enable_if_t<
        (_namespace(std)::is_convertible<OIter, Iter>::value &&
         _namespace(std)::is_convertible<OT, T>::value),
        enable_tag>)
: impl_(o.impl_)
{}

template<typename Iter, typename T, cast_iterator_style Style,
         typename TPtr, typename TRef>
auto cast_iterator<Iter, T, Style, TPtr, TRef>::operator++() ->
    cast_iterator& {
  ++impl_;
  return *this;
}

template<typename Iter, typename T, cast_iterator_style Style,
         typename TPtr, typename TRef>
auto cast_iterator<Iter, T, Style, TPtr, TRef>::operator++(int) ->
    cast_iterator {
  return cast_iterator(impl_++);
}

template<typename Iter, typename T, cast_iterator_style Style,
         typename TPtr, typename TRef>
auto cast_iterator<Iter, T, Style, TPtr, TRef>::operator--() ->
    cast_iterator& {
  --impl_;
  return *this;
}

template<typename Iter, typename T, cast_iterator_style Style,
         typename TPtr, typename TRef>
auto cast_iterator<Iter, T, Style, TPtr, TRef>::operator--(int) ->
    cast_iterator {
  return cast_iterator(impl_--);
}

template<typename Iter, typename T, cast_iterator_style Style,
         typename TPtr, typename TRef>
auto cast_iterator<Iter, T, Style, TPtr, TRef>::operator+(difference_type d)
    const -> cast_iterator {
  return cast_iterator(impl_ + d);
}

template<typename Iter, typename T, cast_iterator_style Style,
         typename TPtr, typename TRef>
auto cast_iterator<Iter, T, Style, TPtr, TRef>::operator-(difference_type d)
    const -> cast_iterator {
  return cast_iterator(impl_ - d);
}

template<typename Iter, typename T, cast_iterator_style Style,
         typename TPtr, typename TRef>
auto cast_iterator<Iter, T, Style, TPtr, TRef>::operator+=(
    difference_type d) -> cast_iterator& {
  impl_ += d;
  return *this;
}

template<typename Iter, typename T, cast_iterator_style Style,
         typename TPtr, typename TRef>
auto cast_iterator<Iter, T, Style, TPtr, TRef>::operator-=(
    difference_type d) -> cast_iterator& {
  impl_ -= d;
  return *this;
}

template<typename Iter, typename T, cast_iterator_style Style,
         typename TPtr, typename TRef>
auto cast_iterator<Iter, T, Style, TPtr, TRef>::operator-(
    const cast_iterator& o) const -> difference_type {
  return impl_ - o.impl_;
}

template<typename Iter, typename T, cast_iterator_style Style,
         typename TPtr, typename TRef>
auto cast_iterator<Iter, T, Style, TPtr, TRef>::operator[](difference_type d)
    const -> reference {
  return *(*this + d);
}

template<typename Iter, typename T, cast_iterator_style Style,
         typename TPtr, typename TRef>
auto cast_iterator<Iter, T, Style, TPtr, TRef>::operator*() const ->
    reference {
  impl::cast_iterator_transform<TRef, Style> op;
  return op(*impl_);
}


template<typename Iter, typename T, cast_iterator_style Style,
         typename TPtr, typename TRef>
template<typename OIter, typename OT, cast_iterator_style OStyle,
         typename OTPtr, typename OTRef>
auto cast_iterator<Iter, T, Style, TPtr, TRef>::operator==(
    const cast_iterator<OIter, OT, OStyle, OTPtr, OTRef>& o) const -> bool {
  return impl_ == o.impl_;
}

template<typename Iter, typename T, cast_iterator_style Style,
         typename TPtr, typename TRef>
template<typename OIter, typename OT, cast_iterator_style OStyle,
         typename OTPtr, typename OTRef>
auto cast_iterator<Iter, T, Style, TPtr, TRef>::operator!=(
    const cast_iterator<OIter, OT, OStyle, OTPtr, OTRef>& o) const -> bool {
  return !(*this == o);
}


_namespace_end(ilias)

#endif /* _ILIAS_CAST_ITERATOR_INL_H_ */
