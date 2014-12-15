#ifndef _ILIAS_CYPTR_EDGE_H_
#define _ILIAS_CYPTR_EDGE_H_

#include <ilias/cyptr/impl/fwd.h>
#include <ilias/cyptr/impl/edge.h>
#include <ilias/refcnt.h>
#include <type_traits>

namespace ilias {
namespace cyptr {


template<typename T>
class edge {
  template<typename> friend class edge;
  template<typename> friend class cyptr;

 private:
  static void implicit_convertible_(T*);  // No implementation.
  template<typename U> using is_implicit_convertible =
      decltype(edge::implicit_convertible_(std::declval<U*>()));

 public:
  using element_type = T;
  using pointer = element_type*;

  edge() = delete;
  edge(const edge&) = delete;
  edge(edge&&) = delete;

  explicit edge(const node&) noexcept;
  template<typename U, typename = is_implicit_convertible<U>>
      edge(const node&, const cyptr<U>&) noexcept;
  edge& operator=(const edge&) noexcept;
  template<typename U, typename = is_implicit_convertible<U>>
      edge& operator=(const edge<U>&) noexcept;
  template<typename U, typename = is_implicit_convertible<U>>
      edge& operator=(const cyptr<U>&) noexcept;
  ~edge() noexcept;

  explicit operator bool() const noexcept;
  pointer get() const noexcept;
  pointer operator->() const noexcept;
  auto operator*() const noexcept ->
      typename std::conditional_t<std::is_void<T>::value,
                                  std::identity<T>,
                                  std::add_lvalue_reference<T>>::type;

  void reset() noexcept;
  void reset(std::nullptr_t) noexcept;
  void reset(const edge&) noexcept;
  void reset(const cyptr<T>&) noexcept;

 private:
  T* ptr_ = nullptr;
  impl::edge edge_;
};


template<typename T>
class cyptr {
  template<typename> friend class edge;
  template<typename> friend class cyptr;

 private:
  static void implicit_convertible_(T*);  // No implementation.
  template<typename U> using is_implicit_convertible =
      decltype(edge::implicit_convertible_(std::declval<U*>()));

 public:
  using element_type = T;
  using pointer = element_type*;

  cyptr() noexcept;
  cyptr(const cyptr&) noexcept;
  cyptr(cyptr&&) noexcept;
  template<typename U, typename = is_implicit_convertible<U>,
           typename = std::enable_if_t<
                          std::is_base_of<node, std::remove_cv_t<U>>::value,
                          void>>
      explicit cyptr(U*) noexcept;
  template<typename U, typename = is_implicit_convertible<U>>
      cyptr(const cyptr<U>&) noexcept;
  template<typename U, typename = is_implicit_convertible<U>>
      cyptr(cyptr<U>&&) noexcept;
  template<typename U, typename = is_implicit_convertible<U>>
      cyptr(const edge<U>&) noexcept;
  template<typename U> cyptr(cyptr<U>, pointer) noexcept;
  cyptr& operator=(const cyptr&) noexcept;
  cyptr& operator=(cyptr&&) noexcept;
  template<typename U, typename = is_implicit_convertible<U>>
      cyptr& operator=(const cyptr<U>&) noexcept;
  template<typename U, typename = is_implicit_convertible<U>>
      cyptr& operator=(cyptr<U>&&) noexcept;
  template<typename U, typename = is_implicit_convertible<U>>
      cyptr& operator=(const edge<U>&) noexcept;

  explicit operator bool() const noexcept;
  pointer get() const noexcept;
  pointer operator->() const noexcept;
  auto operator*() const noexcept ->
      typename conditional_t<is_void<T>::value,
                             identity<T>,
                             add_lvalue_reference<T>>::type;

  void reset() noexcept;
  void reset(std::nullptr_t) noexcept;
  void reset(const cyptr&) noexcept;
  void reset(cyptr&&) noexcept;
  template<typename U, typename = is_implicit_convertible<U>>
      void reset(const cyptr<U>&) noexcept;
  template<typename U, typename = is_implicit_convertible<U>>
      void reset(cyptr<U>&&) noexcept;
  template<typename U, typename = is_implicit_convertible<U>>
      void reset(const edge<U>&) noexcept;

 private:
  T* ptr_ = nullptr;
  refpointer<impl::basic_obj> objptr_;
};


template<typename T, typename... Args>
cyptr<T> make_cyptr(Args&&...);


template<typename T>
void swap(edge<T>&, edge<T>&) noexcept;

template<typename T>
void swap(edge<T>&, cyptr<T>&) noexcept;

template<typename T>
void swap(cyptr<T>&, edge<T>&) noexcept;

template<typename T>
void swap(cyptr<T>&, cyptr<T>&) noexcept;


}} /* namespace ilias::cyptr */

#include <ilias/cyptr/edge-inl.h>

#endif /* _ILIAS_CYPTR_EDGE_H_ */
