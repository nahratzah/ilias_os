#ifndef _STDIMPL_FUNCTIONAL_FNHELPER_H_
#define _STDIMPL_FUNCTIONAL_FNHELPER_H_

#include <cdecl.h>

_namespace_begin(std)
namespace impl {


template<typename T>
struct fnhelper {};

template<typename T, typename Arg>
struct fnhelper<T(Arg)> {
  using argument_type = Arg;
  using result_type = result_of_t<T(Arg)>;
};

template<typename T, typename Arg1, typename Arg2>
struct fnhelper<T(Arg1, Arg2)> {
  using first_argument_type = Arg1;
  using second_argument_type = Arg2;
  using result_type = result_of_t<T(Arg1, Arg2)>;
};

template<typename T, typename... A>
struct fnhelper<T(A...)> {
  using result_type = result_of_t<T(A...)>;
};


} /* namespace std::impl */
_namespace_end(std)

#endif /* _STDIMPL_FUNCTIONAL_FNHELPER_H_ */
