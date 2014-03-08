#ifndef _STDIMPL_ALLOC_TRAITS_H_
#define _STDIMPL_ALLOC_TRAITS_H_

#include <cdecl.h>
#include <type_traits>
#include <stdimpl/member_check.h>

_namespace_begin(std)

template<typename T> class pointer_traits;  // See <memory>

namespace impl {


using alloc_traits_construct_yes = true_type;
using alloc_traits_construct_no = false_type;

template<typename Alloc, typename T, typename... Args>
struct alloc_traits_construct_choice {
 private:
  template<typename A, typename RV =
      decltype(declval<A>().construct(declval<T*>(), declval<Args>()...),
               true_type())>
  static RV fn(A = declval<Alloc>());
  static false_type fn(...);

  using _type = decltype(fn());

 public:
  using value_type = typename _type::value_type;
  using type = typename _type::type;
  static constexpr value_type value = _type::value;
};

template<bool Choice> struct alloc_traits_construct;

template<>
struct alloc_traits_construct<true> {
  template<typename Alloc, typename T, typename... Args>
  static void construct(Alloc& a, T* p, Args&&... args)
      noexcept(noexcept(a.construct(p, forward<Args>(args)...))) {
    a.construct(p, forward<Args>(args)...);
  }
};

template<>
struct alloc_traits_construct<false> {
  template<typename Alloc, typename T, typename... Args>
  static void construct(Alloc& a, T* p, Args&&... args)
      noexcept(noexcept(::new(p) T(forward<Args>(args)...))) {
    ::new(static_cast<void*>(p)) T(forward<Args>(args)...);
  }
};


_MEMBER_TYPE_CHECK(reference);
_MEMBER_TYPE_CHECK(const_reference);
_MEMBER_TYPE_CHECK(pointer);
_MEMBER_TYPE_CHECK(const_pointer);


template<typename T>
struct alloc_traits_fallback {
  using value_type = T;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;
};


template<typename Alloc> struct alloc_traits {
 private:
  /* Use alloc_traits_construct above. */
  template<typename T, typename... Args> using construct_impl =
      alloc_traits_construct<
        alloc_traits_construct_choice<Alloc, T, Args...>::value>;

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
  using fallback = alloc_traits_fallback<typename Alloc::value_type>;

  using reference =
      typename conditional_t<member_type_check_reference<Alloc>::value,
                             Alloc,
                             fallback>::reference;
  using const_reference =
      typename conditional_t<member_type_check_const_reference<Alloc>::value,
                             Alloc,
                             fallback>::const_reference;
  using pointer =
      typename conditional_t<member_type_check_pointer<Alloc>::value,
                             Alloc,
                             fallback>::pointer;
  using const_pointer =
      typename conditional_t<member_type_check_const_pointer<Alloc>::value,
                             Alloc,
                             fallback>::const_pointer;

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
                                 declval<Alloc>()) ->
          AllocImpl<T, Args...>;

 public:
  template<typename T> using rebind_alloc = decltype(select_rebind_<T>());

 public:
  /* Select allocate method. */
  template<typename A, typename RV = decltype(
      declval<A&>().allocate(declval<size_type>(),
                             declval<const_void_pointer>()))>
  static auto allocate(A& a, size_type n, const_void_pointer hint) ->
      RV {
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
      noexcept(noexcept(construct_impl<T, Args...>::construct(
        a, p, forward<Args>(args)...))) ->
      void {
    return construct_impl<T, Args...>::construct(
        a, p, forward<Args>(args)...);
  }

  /* Select destroy method. */
  template<typename A, typename T, typename F>
  static auto destroy(A& a, T* p,
      F = declval<decltype(a.destroy(p))>())
      noexcept(noexcept(a.destroy(p))) ->
      decltype(a.destroy(p)) {
    return a.destroy(p);
  }

  template<typename T>
  static void destroy(Alloc& a, T* p, ...)
      noexcept(noexcept(p->~T())) {
    p->~T();
  }

  /* Select max_size method. */
  template<typename F>
  static auto max_size(const Alloc& a,
      F = declval<decltype(a.max_size())>())
      noexcept(noexcept(a.max_size())) ->
      decltype(a.max_size()) {
    return a.max_size();
  }

  static constexpr auto max_size(const Alloc&, ...) noexcept -> size_type {
    return numeric_limits<size_type>::max();
  }
};


} /* namespace std::impl */
_namespace_end(std)

#endif /* _STDIMPL_ALLOC_TRAITS_H_ */
