#ifndef _STDIMPL_INVOKE_H_
#define _STDIMPL_INVOKE_H_

#include <cdecl.h>
#include <type_traits>
#include <utility>

_namespace_begin(std)
namespace impl {


/*
 * INVOKE:  (t1.*f)(t2, ..., tN)
 * when f is a pointer to member function of a class T and
 * - t1 is an object of type T or
 * - t1 is a reference to an object of type T or
 * - t1 is a reference to an object derived from T.
 */
template<typename F, typename T, typename... Args, typename T1, typename... TT>
auto invoke(F (T::*f)(Args...), T1&& t1, TT&&... tt)
    noexcept(noexcept((forward<T1>(t1).*f)(forward<TT>(tt)...))) ->
    enable_if_t<(is_same<T, T1>::value ||
                 is_base_of<T, remove_reference_t<T1>>::value),
                decltype((forward<T1>(t1).*f)(forward<TT>(tt)...))> {
  return (forward<T1>(t1).*f)(forward<TT>(tt)...);
}

/*
 * INVOKE:  ((*t1).*f)(t2, ..., tN)
 * when f is a pointer to member function of a class T and
 * t1 is not one of the types described in the previous item.
 */
template<typename F, typename T, typename... Args, typename T1, typename... TT>
auto invoke(F (T::*f)(Args...), T1&& t1, TT&&... tt)
    noexcept(noexcept(((*forward<T1>(t1)).*f)(forward<TT>(tt)...))) ->
    enable_if_t<!(is_same<T, T1>::value ||
                  is_base_of<T, remove_reference_t<T1>>::value),
                decltype(((*forward<T1>(t1)).*f)(forward<TT>(tt)...))> {
  return ((*forward<T1>(t1)).*f)(forward<TT>(tt)...);
}

/*
 * INVOKE:  t1.*f
 * when N == 1 and f is a pointer to member data of a class T and
 * - t1 is an object of type T or
 * - t1 is a reference to an object of type T or
 * - t1 is a reference to an object derived from T.
 */
template<typename F, typename T, typename T1>
auto invoke(F T::*f, T1&& t1)
    noexcept(noexcept(forward<T1>(t1).*f)) ->
    enable_if_t<(is_same<T, T1>::value ||
                 is_base_of<T, remove_reference_t<T1>>::value),
                decltype(forward<T1>(t1).*f)> {
  return forward<T1>(t1).*f;
}

/*
 * INVOKE:  (*t1).*f
 * when N == 1 and f is a pointer to member data of a class T and
 * t1 is not one of the types described in the previous item.
 */
template<typename F, typename T, typename T1>
auto invoke(F T::*f, T1&& t1)
    noexcept(noexcept((*forward<T1>(t1)).*f)) ->
    enable_if_t<!(is_same<T, T1>::value ||
                  is_base_of<T, remove_reference_t<T1>>::value),
                decltype((*forward<T1>(t1)).*f)> {
  return (*forward<T1>(t1)).*f;
}

/*
 * INVOKE:  f(t1, t2, ..., tN)
 * in all other cases.
 */
template<typename F, typename... Args>
auto invoke(F& f, Args&&... args)
    noexcept(noexcept(f(forward<Args>(args)...))) ->
    decltype(f(forward<Args>(args)...)) {
  return f(forward<Args>(args)...);
}
template<typename F, typename... Args>
auto invoke(F&& f, Args&&... args)
    noexcept(noexcept(f(forward<Args>(args)...))) ->
    decltype(f(forward<Args>(args)...)) {
  return f(forward<Args>(args)...);
}
template<typename F, typename... Args>
auto invoke(const F& f, Args&&... args)
    noexcept(noexcept(f(forward<Args>(args)...))) ->
    decltype(f(forward<Args>(args)...)) {
  return f(forward<Args>(args)...);
}


} /* namespace std::impl */
_namespace_end(std)

#endif /* _STDIMPL_INVOKE_H_ */
