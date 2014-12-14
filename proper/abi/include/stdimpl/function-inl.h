#ifndef _STDIMPL_FUNCTION_INL_H_
#define _STDIMPL_FUNCTION_INL_H_

#include <stdimpl/function.h>
#include <stdimpl/invoke.h>

_namespace_begin(std)
namespace impl {


template<typename R, typename... ArgTypes>
functor_wrapper<R(ArgTypes...)>::functor_wrapper(size_align sa,
                                                const std::type_info& ti)
    noexcept
: allocator_cloneable(sa),
  ti(ti)
{}


template<typename Wrapper, typename T>
auto functor_wrapper_copy_impl_(void* dst, const T& src) ->
    enable_if_t<is_copy_constructible<T>::value, Wrapper*> {
  return new (dst) Wrapper(src);
}

template<typename Wrapper, typename T>
auto functor_wrapper_copy_impl_(void*, const T&) ->
    enable_if_t<!is_copy_constructible<T>::value, Wrapper*> {
  abi::panic("functor wrapper (%s, for std::function) is not copyable",
      typeid(Wrapper).name());  // XXX: demangle
  /* UNREACHABLE */
  for (;;);
  return nullptr;
}

template<typename R, typename... ArgTypes, typename F>
functor_wrapper_impl<R(ArgTypes...), F>::functor_wrapper_impl(F&& f)
: functor_wrapper<R(ArgTypes...)>(
      allocator_cloneable::size_align{ sizeof(functor_wrapper_impl),
                                       alignof(functor_wrapper_impl) },
      typeid(F)),
  fn_(forward<F>(f))
{}

template<typename R, typename... ArgTypes, typename F>
auto functor_wrapper_impl<R(ArgTypes...), F>::operator()(ArgTypes... args) ->
    R {
  return invoke(fn_, forward<ArgTypes>(args)...);
}

template<typename R, typename... ArgTypes, typename F>
auto functor_wrapper_impl<R(ArgTypes...), F>::copy_into(void* p) const ->
    functor_wrapper_impl* {
  return functor_wrapper_copy_impl_<functor_wrapper_impl>(p, *this);
}

template<typename R, typename... ArgTypes, typename F>
auto functor_wrapper_impl<R(ArgTypes...), F>::move_into(void* p) ->
    functor_wrapper_impl* {
  return new (p) functor_wrapper_impl(move(*this));
}

template<typename R, typename... ArgTypes, typename F>
auto functor_wrapper_impl<R(ArgTypes...), F>::get_impl_pointer(
    const std::type_info& ti) noexcept -> void* {
  return (this->ti == ti ? &fn_ : nullptr);
}


} /* namespace impl */
_namespace_end(std)

#endif /* _STDIMPL_FUNCTION_INL_H_ */
