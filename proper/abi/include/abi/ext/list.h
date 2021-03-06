#ifndef _ABI_EXT_LIST_H_
#define _ABI_EXT_LIST_H_

#include <cdecl.h>
#include <abi/abi.h>
#include <iterator>

namespace __cxxabiv1 {
namespace ext {


template<typename T, typename Tag> class list;
template<bool Order, typename Tag> class iterator_order;
template<typename Tag> class iterator_order<true, Tag>;
template<typename Tag> class iterator_order<false, Tag>;


template<typename Tag> class list_elem {
  template<typename T, typename TTag> friend class list;
  friend class iterator_order<true, Tag>;
  friend class iterator_order<false, Tag>;

 public:
  constexpr list_elem();
  constexpr list_elem(const list_elem&);
  list_elem& operator=(const list_elem&) noexcept;

 protected:
  ~list_elem() = default;

 private:
  using list_elem_ptr = list_elem*;

  constexpr list_elem(list_elem*);

  mutable list_elem_ptr succ_ = nullptr;
  mutable list_elem_ptr pred_ = nullptr;
};


template<typename T, typename Tag> class list {
 private:
  template<typename IT, typename IE, bool Order> class iterator_tmpl;

 public:
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using value_type = T;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;

  using iterator = iterator_tmpl<value_type, list_elem<Tag>, true>;
  using const_iterator = iterator_tmpl<const value_type,
                                       const list_elem<Tag>, true>;
  using reverse_iterator = iterator_tmpl<value_type, list_elem<Tag>, false>;
  using const_reverse_iterator = iterator_tmpl<const value_type,
                                               const list_elem<Tag>, false>;

  list() = default;
  list(const list&) = delete;
  list(list&&) noexcept;
  list& operator=(const list&) = delete;
  list& operator=(list&&) noexcept;
  ~list() noexcept;

  void swap(list&) noexcept;
  bool empty() const noexcept;
  void clear() noexcept;

  static bool is_linked(const_pointer) noexcept;
  bool link_front(const_pointer) noexcept;
  bool link_back(const_pointer) noexcept;
  bool unlink(const_pointer) noexcept;
  bool link_before(const_pointer, const_iterator) noexcept;
  bool link_after(const_pointer, const_iterator) noexcept;
  bool unlink(const_iterator) noexcept;

  iterator begin() noexcept;
  iterator end() noexcept;
  const_iterator begin() const noexcept;
  const_iterator end() const noexcept;
  reverse_iterator rbegin() noexcept;
  reverse_iterator rend() noexcept;
  const_reverse_iterator rbegin() const noexcept;
  const_reverse_iterator rend() const noexcept;
  const_iterator cbegin() const noexcept;
  const_iterator cend() const noexcept;
  const_reverse_iterator crbegin() const noexcept;
  const_reverse_iterator crend() const noexcept;

  iterator iterator_to(pointer) noexcept;
  const_iterator iterator_to(const_pointer) const noexcept;

 private:
  static bool link_after_(const_pointer, const list_elem<Tag>*) noexcept;
  static bool link_before_(const_pointer, const list_elem<Tag>*) noexcept;

  static list_elem<Tag>* pred(list_elem<Tag>*) noexcept;
  static const list_elem<Tag>* pred(const list_elem<Tag>*) noexcept;
  static list_elem<Tag>* succ(list_elem<Tag>*) noexcept;
  static const list_elem<Tag>* succ(const list_elem<Tag>*) noexcept;
  static T* down_cast(list_elem<Tag>*) noexcept;
  static const T* down_cast(const list_elem<Tag>*) noexcept;

  list_elem<Tag> head_{ &head_ };
};


template<typename Tag>
class iterator_order<true, Tag> {
 public:
  using iterator_category = _namespace(std)::bidirectional_iterator_tag;

 private:
  using elem_t = list_elem<Tag>;
  using const_elem_t = const list_elem<Tag>;

 protected:
  ~iterator_order() = default;

  static elem_t* pred(elem_t*) noexcept;
  static const_elem_t* pred(const_elem_t*) noexcept;
  static elem_t* succ(elem_t*) noexcept;
  static const_elem_t* succ(const_elem_t*) noexcept;
};

template<typename Tag>
class iterator_order<false, Tag> {
 public:
  using iterator_category = _namespace(std)::bidirectional_iterator_tag;

 private:
  using elem_t = list_elem<Tag>;
  using const_elem_t = const list_elem<Tag>;

 protected:
  ~iterator_order() = default;

  static elem_t* pred(elem_t*) noexcept;
  static const_elem_t* pred(const_elem_t*) noexcept;
  static elem_t* succ(elem_t*) noexcept;
  static const_elem_t* succ(const_elem_t*) noexcept;
};


template<typename T, typename Tag>
template<typename IT, typename IE, bool Order>
class list<T, Tag>::iterator_tmpl
: public iterator_order<Order, Tag> {
  friend list<T, Tag>;

 public:
  using value_type = IT;
  using reference = value_type&;
  using pointer = value_type*;
  using size_type = list<T, Tag>::size_type;
  using difference_type = list<T, Tag>::difference_type;

  iterator_tmpl() = default;
  iterator_tmpl(const iterator_tmpl&) = default;
  iterator_tmpl& operator=(const iterator_tmpl&) = default;

  template<typename O_IT, typename O_IE, bool O_Order>
  iterator_tmpl(const iterator_tmpl<O_IT, O_IE, O_Order>& o) noexcept;

  template<typename O_IT, typename O_IE, bool O_Order>
  iterator_tmpl& operator=(const iterator_tmpl<O_IT, O_IE, O_Order>& o)
      noexcept;

  reference operator*() const noexcept;
  pointer operator->() const noexcept;
  iterator_tmpl& operator++() noexcept;
  iterator_tmpl& operator--() noexcept;
  iterator_tmpl operator++(int) noexcept;
  iterator_tmpl operator--(int) noexcept;
  bool operator==(const iterator_tmpl&) const noexcept;
  bool operator!=(const iterator_tmpl&) const noexcept;

  template<typename O_IT, typename O_IE, bool O_Order>
  bool operator==(const iterator_tmpl<O_IT, O_IE, O_Order>& o)
      const noexcept;
  template<typename O_IT, typename O_IE, bool O_Order>
  bool operator!=(const iterator_tmpl<O_IT, O_IE, O_Order>& o)
      const noexcept;

 private:
  explicit constexpr iterator_tmpl(IE* elem);

  IE* elem_ = nullptr;
};


template<typename T, typename Tag>
void swap(list<T, Tag>&, list<T, Tag>&) noexcept;

template<typename Tag, typename T>
bool unlink(T*) noexcept;


}} /* namespace __cxxabiv1::ext */

#include <abi/ext/list-inl.h>

#endif /* _ABI_EXT_LIST_H_ */
