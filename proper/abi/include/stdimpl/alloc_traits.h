#ifndef _STDIMPL_ALLOC_TRAITS_H_
#define _STDIMPL_ALLOC_TRAITS_H_

#include <cdecl.h>
#include <type_traits>

_namespace_begin(std)

template<typename T> class pointer_traits;  // See <memory>

namespace impl {


template<typename Alloc> struct alloc_traits {
 private:
  /* Reference type selector. */
  template<typename A> static auto select_reference_(A = declval<Alloc>()) ->
      typename A::reference;
  static auto select_reference_(...) -> typename Alloc::value_type&;

  /* Const reference type selector. */
  template<typename A>
      static auto select_const_reference_(A = declval<Alloc>()) ->
      typename A::const_reference;
  static auto select_const_reference_(...) ->
      const typename Alloc::value_type&;

  /* Pointer type selector. */
  template<typename A>
      static auto select_pointer_(A = declval<Alloc>()) ->
      typename A::pointer;
  static auto select_pointer_(...) -> typename Alloc::value_type*;

  /* Const pointer type selector. */
  template<typename A>
      static auto select_const_pointer_(A = declval<Alloc>()) ->
      typename A::const_pointer;
  static auto select_const_pointer_(...) ->
      const typename Alloc::value_type*;

  /* POCCA selector. */
  template<typename A> static auto select_pocca_(A = declval<Alloc>()) ->
       typename A::propagate_on_container_copy_assignment;
  static auto select_pocca_(...) ->
      false_type;

  /* POCMA selector. */
  template<typename A> static auto select_pocma_(A = declval<Alloc>()) ->
      typename A::propagate_on_container_move_assignment;
  static auto select_pocma_(...) ->
      false_type;

  /* POCS selector. */
  template<typename A> static auto select_pocs_(A = declval<Alloc>()) ->
      typename A::propagate_on_container_swap;
  static auto select_pocs_(...) ->
      false_type;

 public:
  using reference = decltype(select_reference_());
  using const_reference = decltype(select_const_reference_());
  using pointer = decltype(select_pointer_());
  using const_pointer = decltype(select_const_pointer_());

 private:
  /* Void pointer selector. */
  template<typename A> static auto select_vpointer_(A = declval<Alloc>()) ->
      typename A::void_pointer;
  static auto select_vpointer_(...) ->
      const typename pointer_traits<pointer>::template rebind<void>;

  /* Const void pointer selector. */
  template<typename A> static auto select_cvpointer_(A = declval<Alloc>()) ->
      typename A::const_void_pointer;
  static auto select_cvpointer_(...) ->
      const typename pointer_traits<pointer>::template rebind<const void>;

  /* Difference type selector. */
  template<typename A> static auto select_difftype_(A = declval<Alloc>()) ->
      typename A::difference_type;
  static auto select_difftype_(...) ->
      typename pointer_traits<pointer>::difference_type;

 public:
  using void_pointer = decltype(select_vpointer_());
  using const_void_pointer = decltype(select_cvpointer_());
  using difference_type = decltype(select_difftype_());

 private:
  /* Size type selector. */
  template<typename A> static auto select_sizetype_(A = declval<Alloc>()) ->
      typename A::size_type;
  static auto select_sizetype_(...) ->
      typename make_unsigned<difference_type>::type;

 public:
  using size_type = decltype(select_sizetype_());

  using propagate_on_container_copy_assignment = decltype(select_pocca_());
  using propagate_on_container_move_assignment = decltype(select_pocma_());
  using propagate_on_container_swap = decltype(select_pocs_());

 private:
  /* Rebind selector. */
  template<typename T, typename A>
      static auto select_rebind_(A = declval<Alloc>()) ->
      typename A::template rebind<T>::other;
  template<typename T,
           template<typename, typename...> class AllocImpl,
           typename U, typename... Args>
      static auto select_rebind_(const AllocImpl<U, Args...>& =
                                 std::declval<Alloc>()) ->
          AllocImpl<T, Args...>;

 public:
  template<typename T> using rebind_alloc = decltype(select_rebind_<T>());

 public:
  /* Select allocate method. */
  template<typename A>
  static auto allocate(Alloc& a, size_type n, const_void_pointer hint,
                       A& = declval<Alloc>()) ->
      decltype(declval<A>().allocate(n, hint)) {
    return a.allocate(n, hint);
  }

  template<typename A>
  static auto allocate(Alloc& a, size_type n, const_void_pointer,
                       A& = declval<Alloc>(), ...) ->
      pointer {
    return a.allocate(n);
  }

  /* Select construct method. */
  template<typename T, typename... Args>
  static auto construct(Alloc& a, T* p, Args&&... args)
      noexcept(noexcept(a.construct(p, std::forward<Args>(args)...))) ->
      decltype(a.construct(p, std::forward<Args>(args)...)) {
    return a.construct(p, std::forward<Args>(args)...);
  }

  template<typename T, typename... Args>
  static void construct(Alloc& a, T* p, Args&&... args)
      noexcept(noexcept(::new (static_cast<void*>(p))
                              T(std::forward<Args>(args)...))) {
    ::new (static_cast<void*>(p)) T(std::forward<Args>(args)...);
  }

  /* Select destroy method. */
  template<typename T>
  static auto destroy(Alloc& a, T* p)
      noexcept(noexcept(a.destroy(p))) ->
      decltype(a.destroy(p)) {
    return a.destroy(p);
  }

  template<typename T>
  static void destroy(Alloc& a, T* p)
      noexcept(noexcept(p->~T())) {
    p->~T();
  }
};


} /* namespace std::impl */
_namespace_end(std)

#endif /* _STDIMPL_ALLOC_TRAITS_H_ */
