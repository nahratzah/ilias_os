#ifndef _ILIAS_PTRDEREF_ITERATOR_H_
#define _ILIAS_PTRDEREF_ITERATOR_H_

#include <cdecl.h>
#include <iterator>
#include <memory>

_namespace_begin(ilias)


template<typename Iter>
class ptrderef_iterator
: public _namespace(std)::iterator<
      typename _namespace(std)::iterator_traits<Iter>::iterator_category,
      typename _namespace(std)::pointer_traits<
          typename _namespace(std)::iterator_traits<Iter>::value_type
         >::element_type,
      typename _namespace(std)::iterator_traits<Iter>::difference_type>
{
  template<typename> friend class ptrderef_iterator;

 private:
  using __iterator_spec = _namespace(std)::iterator<
      typename _namespace(std)::iterator_traits<Iter>::iterator_category,
      typename _namespace(std)::pointer_traits<
          typename _namespace(std)::iterator_traits<Iter>::value_type
         >::element_type,
      typename _namespace(std)::iterator_traits<Iter>::difference_type>;

  struct enable_tag {};

 public:
  using difference_type = typename __iterator_spec::difference_type;
  using pointer = typename __iterator_spec::pointer;
  using reference = typename __iterator_spec::reference;

  ptrderef_iterator() = default;
  ptrderef_iterator(const ptrderef_iterator&) = default;
  ptrderef_iterator& operator=(const ptrderef_iterator&) = default;

  explicit ptrderef_iterator(const Iter& impl) : impl_(impl) {}

  template<typename OIter>
  ptrderef_iterator(
      const ptrderef_iterator<OIter>&,
      _namespace(std)::enable_if_t<
          _namespace(std)::is_convertible<OIter, Iter>::value,
          enable_tag> = enable_tag());

  ptrderef_iterator& operator++();
  ptrderef_iterator operator++(int);
  ptrderef_iterator& operator--();
  ptrderef_iterator operator--(int);

  ptrderef_iterator operator+(difference_type) const;
  ptrderef_iterator operator-(difference_type) const;
  ptrderef_iterator& operator+=(difference_type);
  ptrderef_iterator& operator-=(difference_type);

  difference_type operator-(const ptrderef_iterator&) const;
  reference operator[](difference_type) const;

  reference operator*() const { return **impl_; }
  pointer operator->() const { return &*(*this); }

  template<typename OIter>
  bool operator==(const ptrderef_iterator<OIter>&) const;
  template<typename OIter>
  bool operator!=(const ptrderef_iterator<OIter>&) const;

 private:
  Iter impl_;
};


_namespace_end(ilias)

#include <ilias/ptrderef_iterator-inl.h>

#endif /* _ILIAS_PTRDEREF_ITERATOR_H_ */
