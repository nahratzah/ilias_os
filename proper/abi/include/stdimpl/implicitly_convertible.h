#ifndef _STDIMPL_IMPLICITLY_CONVIRTIBLE_H_
#define _STDIMPL_IMPLICITLY_CONVIRTIBLE_H_

#include <cdecl.h>
#include <stdimpl/member_check.h>

_namespace_begin(std)
namespace impl {
namespace implicit_conversion_check {

template<typename T>
struct test {
  static void implicitly_convertible(T);  // no implementation
};

_STATIC_MEMBER_FUNCTION_CHECK(implicitly_convertible);

} /* namespace std::impl::implicit_conversion_check */

template<typename T, typename U> using implicitly_convertible =
    typename implicit_conversion_check::
        static_member_function_check_implicitly_convertible<
            implicit_conversion_check::test<T>, U>::type;


} /* namespace std::impl */
_namespace_end(std)

#endif /* _STDIMPL_IMPLICITLY_CONVIRTIBLE_H_ */
