#ifndef _ILIAS_NESTED_ITERATOR_INL_H_
#define _ILIAS_NESTED_ITERATOR_INL_H_

#include <ilias/nested_iterator.h>

_namespace_begin(ilias)


template<typename O, typename I>
nested_iterator<O, I>::nested_iterator(outer_iterator_type outer)
: outer_(outer)
{}

template<typename O, typename I>
nested_iterator<O, I>::nested_iterator(outer_iterator_type outer,
                                       inner_iterator_type inner)
: outer_(outer),
  inner_(inner),
  inner_lazy_(false)
{}

template<typename O, typename I>
template<typename T, typename U>
nested_iterator<O, I>::nested_iterator(
    const nested_iterator<T, U>& o,
    _namespace(std)::enable_if_t<
        (_namespace(std)::is_convertible<T, O>::value &&
         _namespace(std)::is_convertible<U, I>::value),
        enable_tag>)
: outer_(o.outer_),
  inner_(o.inner_),
  inner_lazy_(o.inner_lazy_)
{}

template<typename O, typename I>
auto nested_iterator<O, I>::operator++() -> nested_iterator& {
  fixup_();
  if (inner_ == outer_->end()) {
    ++outer_;
    inner_lazy_ = true;
  } else {
    ++inner_;
  }
  return *this;
}

template<typename O, typename I>
auto nested_iterator<O, I>::operator++(int) -> nested_iterator {
  nested_iterator tmp = *this;
  ++*this;
  return tmp;
}

template<typename O, typename I>
auto nested_iterator<O, I>::operator--() -> nested_iterator& {
  using _namespace(std)::begin;
  using _namespace(std)::end;

  while (inner_lazy_ || inner_ == begin(*outer_)) {
    --outer_;
    inner_ = end(*outer_);
    inner_lazy_ = false;
  }
  --inner_;
  return *this;
}

template<typename O, typename I>
auto nested_iterator<O, I>::operator--(int) -> nested_iterator {
  nested_iterator tmp = *this;
  --*this;
  return tmp;
}

template<typename O, typename I>
auto nested_iterator<O, I>::operator+(difference_type d) const ->
    nested_iterator {
  nested_iterator tmp = *this;
  tmp += d;
  return tmp;
}

template<typename O, typename I>
auto nested_iterator<O, I>::operator-(difference_type d) const ->
    nested_iterator {
  nested_iterator tmp = *this;
  tmp -= d;
  return tmp;
}

template<typename O, typename I>
auto nested_iterator<O, I>::operator+=(difference_type d) ->
    nested_iterator& {
  using _namespace(std)::distance;
  using _namespace(std)::advance;
  using _namespace(std)::next;
  using _namespace(std)::begin;

  constexpr difference_type max_delta =
      impl::nested_collection_size<O, I>::value;
  static_assert(max_delta > 0,
                "Random-access-iterator functionality "
                "requires known inner collection size.");
  if (d == 0) return *this;

  /*
   * Skip elements in inner_, so we end up on an outer_ boundary.
   *
   * It looks a bit complicated, but this way, we skip an update of the outer
   * iterator.  This is especially important if multiple nested_iterator are
   * nested.
   */
  {
    const difference_type inner_delta = (inner_lazy_ ?
                                         0 :
                                         distance(begin(*outer_), inner_));
    if (d > 0) {
      if (d < max_delta - inner_delta) {
        if (inner_lazy_) inner_ = begin(*outer_);
        advance(inner_, d);
        return *this;
      }

      advance(outer_, 1);
      inner_lazy_ = true;
      d -= (max_delta - inner_delta);
    } else {  // d < 0
      if (d > -inner_delta) {
        // Note: inner_delta > 0, therefore inner_lazy_ must be false
        advance(inner_, d);
        return *this;
      }

      if (inner_delta != 0) inner_lazy_ = true;
      d += inner_delta;
    }
  }

  /*
   * Calculate outer and inner delta required.
   * This calculation ensures inner_delta >= 0.
   */
  difference_type inner_delta = d % max_delta;
  difference_type outer_delta = d / max_delta;
  if (inner_delta < 0) {
    inner_delta += max_delta;
    outer_delta -= 1;
  }

  /* Advance outer_, inner_ according to calculation above. */
  advance(outer_, outer_delta);
  if (inner_delta != 0) {
    inner_ = next(begin(*outer_), inner_delta);
    inner_lazy_ = false;
  } else if (outer_delta != 0) {
    inner_lazy_ = true;
  }

  return *this;
}

template<typename O, typename I>
auto nested_iterator<O, I>::operator-=(difference_type d) ->
    nested_iterator& {
  return (*this += -d);
}

template<typename O, typename I>
auto nested_iterator<O, I>::operator-(const nested_iterator& o) const ->
    difference_type {
  using _namespace(std)::distance;
  using _namespace(std)::begin;

  constexpr difference_type max_delta =
      impl::nested_collection_size<O, I>::value;
  static_assert(max_delta > 0, "Require known inner collection size.");

  const auto outer_delta = distance(o.outer_, outer_) * max_delta;
  const auto inner_my_delta = (inner_lazy_ ?
                               difference_type(0) :
                               distance(begin(*outer_), inner_));
  const auto inner_o_delta = (o.inner_lazy_ ?
                              difference_type(0) :
                              distance(begin(*o.outer_), o.inner_));
  return outer_delta + (inner_my_delta - inner_o_delta);
}

template<typename O, typename I>
auto nested_iterator<O, I>::operator[](difference_type d) const -> reference {
  return *(*this + d);
}

template<typename O, typename I>
auto nested_iterator<O, I>::operator*() const -> reference {
  fixup_();
  return *inner_;
}

template<typename O, typename I>
auto nested_iterator<O, I>::operator->() const -> pointer {
  return &*(*this);
}

template<typename O, typename I>
template<typename T, typename U>
auto nested_iterator<O, I>::operator==(const nested_iterator<T, U>& o) const ->
    bool {
  using _namespace(std)::begin;

  /* Compare outer iterator first. */
  if (outer_ != o.outer_) return false;
  /* Compare inner iterator, non-lazy case. */
  if (!inner_lazy_ && !o.inner_lazy_) return inner_ == o.inner_;

  /* Compare inner iterator, lazy case. */
  return ((inner_lazy_ || (inner_ == begin(*outer_))) &&
          (o.inner_lazy_ || (o.inner_ == begin(*o.outer_))));
}

template<typename O, typename I>
template<typename T, typename U>
auto nested_iterator<O, I>::operator!=(const nested_iterator<T, U>& o) const ->
    bool {
  return !(*this == o);
}

template<typename O, typename I>
auto nested_iterator<O, I>::fixup_() const -> void {
  using _namespace(std)::begin;
  using _namespace(std)::end;

  if (inner_lazy_) {
    inner_ = begin(*outer_);
    inner_lazy_ = false;
  }

  while (inner_ == end(*outer_)) {
    ++outer_;
    inner_ = begin(*outer_);
  }
}


_namespace_end(ilias)

#endif /* _ILIAS_NESTED_ITERATOR_INL_H_ */
