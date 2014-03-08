#ifndef _STDIMPL_POINTER_TRAITS_H_
#define _STDIMPL_POINTER_TRAITS_H_

#include <cdecl.h>

_namespace_begin(std)
namespace impl {


template<typename T, typename Ptr> struct pointer_rebind;


/* Maintain const/volatile across type replacement. */
template<typename Type, typename Orig> using maintain_const_t =
    conditional_t<is_const<Orig>::value,
                  add_const_t<Type>,
                  Type>;
template<typename Type, typename Orig> using maintain_volatile_t =
    conditional_t<is_volatile<Orig>::value,
                  add_volatile_t<Type>,
                  Type>;
template<typename Type, typename Orig> using maintain_cv_t =
    maintain_const_t<maintain_volatile_t<Type, Orig>, Orig>;


template<typename T, typename U> struct pointer_rebind<T, U*> {
  using type = maintain_cv_t<T, U>*;
};

template<typename T, template<typename U, typename... Tail> class PtrTmpl,
         typename U, typename... Tail>
struct pointer_rebind<T, PtrTmpl<U, Tail...>> {
  using type = PtrTmpl<maintain_cv_t<T, U>, Tail...>;
};


} /* namespace std::impl */
_namespace_end(std)

#endif /* _STDIMPL_POINTER_TRAITS_H_ */
