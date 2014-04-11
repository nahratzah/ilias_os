#ifndef _STDIMPL_FUNCTION_H_
#define _STDIMPL_FUNCTION_H_

#include <cdecl.h>
#include <memory>
#include <stdimpl/invoke.h>
#include <stdimpl/allocator_cloneable.h>
#include <abi/panic.h>

_namespace_begin(std)
namespace impl {


template<typename> class functor_wrapper;  // Undefined.
template<typename, typename> class functor_wrapper_impl;  // Undefined.


template<typename R, typename... ArgTypes>
class functor_wrapper<R(ArgTypes...)>
: public allocator_cloneable
{
 public:
  functor_wrapper(size_align sa, const std::type_info& ti) noexcept;
  ~functor_wrapper() noexcept override = default;
  virtual R operator()(ArgTypes...) = 0;

  functor_wrapper* copy_into(void*) const override = 0;
  functor_wrapper* move_into(void*) override = 0;
  virtual void* get_impl_pointer(const std::type_info&) const noexcept = 0;

  const std::type_info& ti;
};


template<typename R, typename... ArgTypes, typename F>
class functor_wrapper_impl<R(ArgTypes...), F> final
: public functor_wrapper<R(ArgTypes...)>
{
  static_assert(is_move_constructible<F>::value,
                "Functor must be move constructible to be used with "
                "std::function.");

 public:
  functor_wrapper_impl(const functor_wrapper_impl&) = default;
  functor_wrapper_impl(F&& f);
  ~functor_wrapper_impl() noexcept override = default;

  R operator()(ArgTypes... args) override;
  functor_wrapper_impl* copy_into(void* p) const override;
  functor_wrapper_impl* move_into(void* p) override;
  void* get_impl_pointer(const std::type_info&) const noexcept override;

 private:
  F fn_;
};


/*
 * A small struct, that transforms non-reference values into references.
 */
template<typename T>
struct function_transform_to_ref {
  using type = T&&;
};

template<typename T>
struct function_transform_to_ref<T&> {
  using type = T&;
};

template<typename T>
struct function_transform_to_ref<T&&> {
  using type = T&&;
};

template<typename T> using function_transform_to_ref_t =
    typename function_transform_to_ref<T>::type;


} /* namespace std::impl */
_namespace_end(std)

#include <stdimpl/function-inl.h>

#endif /* _STDIMPL_FUNCTION_H_ */
