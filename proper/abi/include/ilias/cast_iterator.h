#ifndef _ILIAS_CAST_ITERATOR_H_
#define _ILIAS_CAST_ITERATOR_H_

#include <cdecl.h>
#include <iterator>

_namespace_begin(ilias)


enum cast_iterator_style {
  static_cast_style,
  dynamic_cast_style,
  const_cast_style,
  implicit_cast_style,
  reinterpret_cast_style
};


namespace impl {


template<typename T, cast_iterator_style>
struct cast_iterator_transform;

template<typename T>
struct cast_iterator_transform<T, static_cast_style> {
  template<typename U> T operator()(U& u) { return static_cast<T>(u); }
};

template<typename T>
struct cast_iterator_transform<T, dynamic_cast_style> {
  template<typename U> T operator()(U& u) { return dynamic_cast<T>(u); }
};

template<typename T>
struct cast_iterator_transform<T, const_cast_style> {
  template<typename U> T operator()(U& u) { return const_cast<T>(u); }
};

template<typename T>
struct cast_iterator_transform<T, implicit_cast_style> {
  template<typename U> T operator()(U& u) { return u; }
};

template<typename T>
struct cast_iterator_transform<T, reinterpret_cast_style> {
  template<typename U> T operator()(U& u) { return reinterpret_cast<T>(u); }
};


} /* namespace ilias::impl */


template<typename Iter, typename T, cast_iterator_style Style,
         typename TPtr = T*, typename TRef = T&>
class cast_iterator
: public _namespace(std)::iterator<
      typename _namespace(std)::iterator_traits<Iter>::iterator_category,
      T,
      typename _namespace(std)::iterator_traits<Iter>::difference_type,
      TPtr,
      TRef>
{
  template<typename, typename, cast_iterator_style, typename, typename>
      friend class cast_iterator;

 private:
  using __iterator_spec = _namespace(std)::iterator<
      typename _namespace(std)::iterator_traits<Iter>::iterator_category,
      T,
      typename _namespace(std)::iterator_traits<Iter>::difference_type,
      TPtr,
      TRef>;

  struct enable_tag {};

 public:
  using difference_type = typename __iterator_spec::difference_type;
  using pointer = typename __iterator_spec::pointer;
  using reference = typename __iterator_spec::reference;

  cast_iterator() = default;
  cast_iterator(const cast_iterator&) = default;
  cast_iterator& operator=(const cast_iterator&) = default;

  explicit cast_iterator(const Iter& impl) : impl_(impl) {}

  template<typename OIter, typename OT, cast_iterator_style OStyle,
           typename OTPtr, typename OTRef>
  cast_iterator(
      const cast_iterator<OIter, OT, OStyle, OTPtr, OTRef>&,
      _namespace(std)::enable_if_t<
          (_namespace(std)::is_convertible<OIter, Iter>::value &&
           _namespace(std)::is_convertible<OT, T>::value),
          enable_tag> = enable_tag());

  cast_iterator& operator++();
  cast_iterator operator++(int);
  cast_iterator& operator--();
  cast_iterator operator--(int);

  cast_iterator operator+(difference_type) const;
  cast_iterator operator-(difference_type) const;
  cast_iterator& operator+=(difference_type);
  cast_iterator& operator-=(difference_type);

  difference_type operator-(const cast_iterator&) const;
  reference operator[](difference_type) const;

  reference operator*() const;
  pointer operator->() const { return &*(*this); }

  template<typename OIter, typename OT, cast_iterator_style OStyle,
           typename OTPtr, typename OTRef>
  bool operator==(const cast_iterator<OIter, OT, OStyle, OTPtr, OTRef>&) const;
  template<typename OIter, typename OT, cast_iterator_style OStyle,
           typename OTPtr, typename OTRef>
  bool operator!=(const cast_iterator<OIter, OT, OStyle, OTPtr, OTRef>&) const;

 private:
  Iter impl_;
};


_namespace_end(ilias)

#include <ilias/cast_iterator-inl.h>

#endif /* _ILIAS_CAST_ITERATOR_H_ */
