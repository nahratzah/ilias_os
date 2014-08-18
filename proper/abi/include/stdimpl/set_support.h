#ifndef _STDIMPL_SET_SUPPORT_H_
#define _STDIMPL_SET_SUPPORT_H_

#include <cdecl.h>
#include <iterator>
#include <ilias/linked_set.h>

_namespace_begin(std)
namespace impl {

template<typename T, class Tag>
struct set_elem
: _namespace(ilias)::linked_set_element<set_elem<T, Tag>, Tag>
{
  using value_type = T;

  set_elem() = default;
  set_elem(const set_elem&) = default;
  set_elem& operator=(const set_elem&) = default;
  template<typename... Args> set_elem(Args&&...);

  value_type value;
};

template<typename T, typename Iter>
class set_elem_iter
: public iterator<typename iterator_traits<Iter>::iterator_category,
                  T,
                  typename iterator_traits<Iter>::difference_type> {
 public:
  using difference_type = typename iterator_traits<Iter>::difference_type;

  set_elem_iter() = default;
  set_elem_iter(const set_elem_iter&) = default;
  set_elem_iter& operator=(const set_elem_iter&) = default;
  set_elem_iter(Iter impl) : impl_(impl) {}

  template<typename OT, typename OIter>
  set_elem_iter(const set_elem_iter<OT, OIter>& o)
  : impl_(o.impl())
  {}

  template<typename OT, typename OIter>
  bool operator==(const set_elem_iter<OT, OIter>& o) const {
    return impl() == o.impl();
  }

  template<typename OT, typename OIter>
  bool operator!=(const set_elem_iter<OT, OIter>& o) const {
    return !(*this == o);
  }

  const Iter& impl() const noexcept { return impl_; }

  set_elem_iter& operator++() noexcept {
    ++impl_;
    return *this;
  }

  set_elem_iter operator++(int) noexcept {
    return set_elem_iter(impl_++);
  }

  set_elem_iter& operator--() noexcept {
    --impl_;
    return *this;
  }

  set_elem_iter operator--(int) noexcept {
    return set_elem_iter(impl_--);
  }

  set_elem_iter operator+(difference_type n) {
    return set_elem_iter(next(impl_, n));
  }

  set_elem_iter operator-(difference_type n) {
    return set_elem_iter(prev(impl_, n));
  }

  set_elem_iter& operator+=(difference_type n) {
    advance(impl_, n);
    return *this;
  }

  set_elem_iter& operator-=(difference_type n) {
    advance(impl_, n);
    return *this;
  }

  T& operator*() const {
    return impl_->value;
  }

  T* operator->() const {
    return &impl_->value;
  }

 private:
  Iter impl_;
};

} /* namespace impl */
_namespace_end(std)

#include <stdimpl/set_support-inl.h>

#endif /* _STDIMPL_SET_SUPPORT_H_ */
