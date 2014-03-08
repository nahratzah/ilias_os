#ifndef _STDIMPL_ALLOC_TRAITS_H_
#define _STDIMPL_ALLOC_TRAITS_H_

#include <cdecl.h>
#include <type_traits>
#include <limits>
#include <stdimpl/member_check.h>

_namespace_begin(std)

template<typename T> class pointer_traits;  // See <memory>

namespace impl {


_MEMBER_TYPE_CHECK(reference);
_MEMBER_TYPE_CHECK(const_reference);
_MEMBER_TYPE_CHECK(pointer);
_MEMBER_TYPE_CHECK(const_pointer);
_MEMBER_TYPE_CHECK(void_pointer);
_MEMBER_TYPE_CHECK(const_void_pointer);
_MEMBER_TYPE_CHECK(difference_type);
_MEMBER_TYPE_CHECK(size_type);

_MEMBER_TYPE_CHECK(propagate_on_container_move_assignment);
_MEMBER_TYPE_CHECK(propagate_on_container_copy_assignment);
_MEMBER_TYPE_CHECK(propagate_on_container_swap);

_MEMBER_FUNCTION_CHECK(construct);
_MEMBER_FUNCTION_CHECK(destroy);
_MEMBER_FUNCTION_CHECK(max_size);
_MEMBER_FUNCTION_CHECK(allocate);


template<typename T>
struct alloc_traits_fallback {
  using value_type = T;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;

  using propagate_on_container_copy_assignment = false_type;
  using propagate_on_container_move_assignment = false_type;
  using propagate_on_container_swap = false_type;

  template<typename Pointer> struct ptr_derived {
    using void_pointer =
        typename pointer_traits<Pointer>::template rebind<void>;
    using const_void_pointer =
        typename pointer_traits<Pointer>::template rebind<const void>;
    using difference_type =
        typename pointer_traits<Pointer>::difference_type;
  };

  template<typename DiffType> struct difftype_derived {
    using size_type = make_unsigned_t<DiffType>;
  };
};


template<typename Alloc> struct alloc_traits {
 private:
  using value_type = typename Alloc::value_type;
  using fallback = alloc_traits_fallback<value_type>;

 public:
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

  using void_pointer = typename conditional_t<
          member_type_check_void_pointer<Alloc>::value,
          Alloc,
          typename fallback::template ptr_derived<pointer>>::void_pointer;
  using const_void_pointer = typename conditional_t<
          member_type_check_const_void_pointer<Alloc>::value,
          Alloc,
          typename fallback::template ptr_derived<pointer>>::
              const_void_pointer;
  using difference_type = typename conditional_t<
          member_type_check_difference_type<Alloc>::value,
          Alloc,
          typename fallback::template ptr_derived<pointer>>::difference_type;

  using size_type = typename conditional_t<
          member_type_check_size_type<Alloc>::value,
          Alloc,
          typename fallback::template difftype_derived<difference_type>>::
              size_type;

  template<typename T, typename... Args>
  static auto construct(Alloc& alloc, T* ptr, Args&&... args)
      noexcept(noexcept(alloc.construct(ptr, forward<Args>(args)...))) ->
      enable_if_t<
          member_function_check_construct<Alloc, T*, Args...>::value,
          void> {
    alloc.construct(ptr, forward<Args>(args)...);
  }

  template<typename T, typename... Args>
  static auto construct(Alloc&, T* ptr, Args&&... args)
      noexcept(noexcept(new (ptr) value_type(forward<Args>(args)...))) ->
      enable_if_t<
          !member_function_check_construct<Alloc, T*, Args...>::value,
          void> {
    new (ptr) T(forward<Args>(args)...);
  }

  template<typename T>
  static auto destroy(Alloc& alloc, T* ptr)
      noexcept(noexcept(alloc.destroy(ptr))) ->
      enable_if_t<
          member_function_check_destroy<Alloc, T*>::value,
          void> {
    alloc.destroy(ptr);
  }

  template<typename T>
  static auto destroy(Alloc&, T* ptr)
      noexcept(noexcept(ptr->~value_type())) ->
      enable_if_t<
          !member_function_check_destroy<Alloc, T*>::value,
          void> {
    ptr->~T();
  }

  template<typename... Args>
  static auto max_size(const Alloc& alloc, Args&&... args)
      noexcept(noexcept(alloc.max_size(forward<Args>(args)...))) ->
      enable_if_t<member_function_check_max_size<const Alloc, Args...>::value,
                  size_type> {
    return alloc.max_size(forward<Args>(args)...);
  }

  template<typename... Args>
  static constexpr auto max_size(const Alloc&, Args&&... args) noexcept ->
      enable_if_t<!member_function_check_max_size<const Alloc, Args...>::value,
                  size_type> {
    return numeric_limits<size_type>::max(forward<Args>(args)...);
  }

  template<typename Hint>
  static auto allocate(Alloc& alloc, size_type n, Hint&& hint)
      noexcept(noexcept(alloc.allocate(n, forward<Hint>(hint)))) ->
      enable_if_t<
          member_function_check_allocate<Alloc, size_type, Hint>::value,
          pointer>
  {
    return alloc.allocate(n, forward<Hint>(hint));
  }

  template<typename Hint>
  static auto allocate(Alloc& alloc, size_type n, Hint&& hint)
      noexcept(noexcept(alloc.allocate(n))) ->
      enable_if_t<
          !member_function_check_allocate<Alloc, size_type, Hint>::value,
          pointer>
  {
    return alloc.allocate(n);
  }

  static auto deallocate(Alloc& alloc, value_type* p, size_type n)
      noexcept(noexcept(alloc.deallocate(p, n))) -> void
  {
    alloc.deallocate(p, n);
  }

  using propagate_on_container_move_assignment = typename conditional_t<
      member_type_check_propagate_on_container_move_assignment<Alloc>::value,
      Alloc,
      fallback>::propagate_on_container_move_assignment;
  using propagate_on_container_copy_assignment = typename conditional_t<
      member_type_check_propagate_on_container_copy_assignment<Alloc>::value,
      Alloc,
      fallback>::propagate_on_container_copy_assignment;
  using propagate_on_container_swap = typename conditional_t<
      member_type_check_propagate_on_container_swap<Alloc>::value,
      Alloc,
      fallback>::propagate_on_container_swap;



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
};


} /* namespace std::impl */
_namespace_end(std)

#endif /* _STDIMPL_ALLOC_TRAITS_H_ */
