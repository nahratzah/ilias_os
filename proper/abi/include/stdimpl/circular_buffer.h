#ifndef _STDIMPL_CIRCULAR_BUFFER_H_
#define _STDIMPL_CIRCULAR_BUFFER_H_

#include <cdecl.h>
#include <iterator>
#include <type_traits>
#include <memory>
#include <initializer_list>
#include <stdimpl/alloc_base.h>

_namespace_begin(std)
namespace impl {


template<typename T> class _circular_buffer_iterator;

/*
 * Resizeable circular buffer.
 */
template<typename T, typename Alloc = _namespace(std)::allocator<T>>
class circular_buffer
: public impl::alloc_base<Alloc>
{
 private:
  using alloc_base = impl::alloc_base<Alloc>;

 public:
  using value_type = T;
  using allocator_type = typename alloc_base::allocator_type;
  using pointer =
      typename allocator_traits<
          allocator_type>::pointer;
  using const_pointer =
      typename allocator_traits<
          allocator_type>::const_pointer;
  using size_type =
      typename allocator_traits<allocator_type>::size_type;
  using reference = value_type&;
  using const_reference = const value_type&;
  using iterator = _circular_buffer_iterator<value_type>;
  using const_iterator = _circular_buffer_iterator<const value_type>;
  using reverse_iterator =
      _namespace(std)::reverse_iterator<iterator>;
  using const_reverse_iterator =
      _namespace(std)::reverse_iterator<const_iterator>;

  explicit circular_buffer(const allocator_type& = allocator_type());
  explicit circular_buffer(size_type,
                           const allocator_type& = allocator_type());
  circular_buffer(size_type, const_reference,
                  const allocator_type& = allocator_type());
  template<typename InputIter> circular_buffer(
      InputIter, InputIter, const allocator_type& = allocator_type());
  circular_buffer(const circular_buffer&);
  circular_buffer(circular_buffer&&)
      noexcept(
          is_nothrow_move_constructible<alloc_base>::value);
  circular_buffer(const circular_buffer&, const allocator_type&);
  circular_buffer(circular_buffer&&, const allocator_type&);
  circular_buffer(initializer_list<value_type>,
                  const allocator_type&);
  ~circular_buffer() noexcept;
  circular_buffer& operator=(const circular_buffer&);
  circular_buffer& operator=(circular_buffer&&);
  circular_buffer& operator=(initializer_list<value_type>);

  iterator begin() noexcept;
  iterator end() noexcept;
  const_iterator begin() const noexcept;
  const_iterator end() const noexcept;
  const_iterator cbegin() const noexcept;
  const_iterator cend() const noexcept;

  reverse_iterator rbegin() noexcept;
  reverse_iterator rend() noexcept;
  const_reverse_iterator rbegin() const noexcept;
  const_reverse_iterator rend() const noexcept;
  const_reverse_iterator crbegin() const noexcept;
  const_reverse_iterator crend() const noexcept;

  size_type size() const noexcept;
  size_type max_size() const noexcept;
  void resize(size_type);
  void resize(size_type, const_reference);
  size_type capacity() const noexcept;
  bool empty() const noexcept;
  void reserve(size_type);
  void shrink_to_fit();

  reference operator[](size_type) noexcept;
  const_reference operator[](size_type) const noexcept;
  reference at(size_type);
  const_reference at(size_type) const;
  reference front() noexcept;
  const_reference front() const noexcept;
  reference back() noexcept;
  const_reference back() const noexcept;

  template<typename... Args> void emplace_front(Args&&... args);
  void push_front(const_reference);
  void push_front(value_type&&);
  void pop_front() noexcept;

  template<typename... Args> void emplace_back(Args&&... args);
  void push_back(const_reference);
  void push_back(value_type&&);
  void pop_back() noexcept;

  template<typename... Args> iterator emplace(const_iterator, Args&&...);
  iterator insert(const_iterator, const_reference);
  iterator insert(const_iterator, value_type&&);
  iterator insert(const_iterator, size_type, const_reference);
  template<typename InputIter> iterator insert(const_iterator,
                                               InputIter, InputIter);
  iterator insert(const_iterator, initializer_list<value_type>);
  iterator erase(const_iterator);
  iterator erase(const_iterator, const_iterator);
  void swap(circular_buffer& o)
      noexcept(noexcept(declval<circular_buffer>().swap_(o)));
  void clear() noexcept;

 private:
  value_type* heap_begin() noexcept;
  const value_type* heap_begin() const noexcept;
  value_type* heap_end() noexcept;
  const value_type* heap_end() const noexcept;

  pointer heap_ = nullptr;
  size_type avail_ = 0;
  size_type size_ = 0;
  size_type off_ = 0;
};

template<typename T>
class _circular_buffer_iterator
: public _namespace(std)::iterator<random_access_iterator_tag,
                                   T>
{
  template<typename, typename> friend class circular_buffer;
  template<typename> friend class _circular_buffer_iterator;

 public:
  using difference_type = typename _namespace(std)::iterator<
      random_access_iterator_tag, T>::difference_type;
  using reference = typename _namespace(std)::iterator<
      random_access_iterator_tag, T>::reference;
  using pointer = typename _namespace(std)::iterator<
      random_access_iterator_tag, T>::pointer;

 private:
  _circular_buffer_iterator(pointer, pointer,
                            make_unsigned_t<difference_type>)
      noexcept;

 public:
  _circular_buffer_iterator() = default;
  _circular_buffer_iterator(const _circular_buffer_iterator&) = default;
  _circular_buffer_iterator& operator=(const _circular_buffer_iterator&) =
      default;

  template<typename U>
  _circular_buffer_iterator(const _circular_buffer_iterator<U>&,
           enable_if_t<is_convertible<
                           typename _circular_buffer_iterator<U>::pointer,
                           pointer>::value, int> = 0) noexcept;

  reference operator*() const noexcept;
  pointer operator->() const noexcept;

  _circular_buffer_iterator& operator++() noexcept;
  _circular_buffer_iterator operator++(int) noexcept;
  _circular_buffer_iterator& operator--() noexcept;
  _circular_buffer_iterator operator--(int) noexcept;

  _circular_buffer_iterator& operator+=(difference_type) noexcept;
  _circular_buffer_iterator& operator-=(difference_type) noexcept;
  _circular_buffer_iterator operator+(difference_type) const noexcept;
  _circular_buffer_iterator operator-(difference_type) const noexcept;
  difference_type operator-(const _circular_buffer_iterator&) const noexcept;

  reference operator[](difference_type) const noexcept;

  template<typename U> bool operator==(const _circular_buffer_iterator<U>&)
      const noexcept;
  template<typename U> bool operator!=(const _circular_buffer_iterator<U>&)
      const noexcept;

 private:
  pointer heap_begin_ = nullptr;
  pointer heap_end_ = nullptr;
  pointer impl_ = nullptr;
};


template<typename T, typename Alloc>
bool operator==(const circular_buffer<T, Alloc>&,
                const circular_buffer<T, Alloc>&)
    noexcept(noexcept(
        declval<const typename circular_buffer<T, Alloc>::value_type>() ==
        declval<const typename circular_buffer<T, Alloc>::value_type>()));
template<typename T, typename Alloc>
bool operator<(const circular_buffer<T, Alloc>&,
               const circular_buffer<T, Alloc>&)
    noexcept(noexcept(
        declval<const typename circular_buffer<T, Alloc>::value_type>() <
        declval<const typename circular_buffer<T, Alloc>::value_type>()));
template<typename T, typename Alloc>
bool operator!=(const circular_buffer<T, Alloc>& a,
                const circular_buffer<T, Alloc>& b)
    noexcept(noexcept(!(a == b)));
template<typename T, typename Alloc>
bool operator>(const circular_buffer<T, Alloc>& a,
               const circular_buffer<T, Alloc>& b)
    noexcept(noexcept(b < a));
template<typename T, typename Alloc>
bool operator>=(const circular_buffer<T, Alloc>& a,
                const circular_buffer<T, Alloc>& b)
    noexcept(noexcept(!(a < b)));
template<typename T, typename Alloc>
bool operator<=(const circular_buffer<T, Alloc>& a,
                const circular_buffer<T, Alloc>& b)
    noexcept(noexcept(!(b < a)));

template<typename T, typename Alloc>
void swap(circular_buffer<T, Alloc>& a, circular_buffer<T, Alloc>& b)
    noexcept(noexcept(a.swap(b)));


extern template class circular_buffer<void*>;

extern template bool operator==(const circular_buffer<void*>&,
                                const circular_buffer<void*>&);
extern template bool operator<(const circular_buffer<void*>&,
                               const circular_buffer<void*>&);


} /* namespace std::impl */
_namespace_end(std)

#include <stdimpl/circular_buffer-inl.h>

#endif /* _STDIMPL_CIRCULAR_BUFFER_H_ */
