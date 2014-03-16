#include <cdecl.h>
#include <type_traits>
#include <functional-fwd.h>

_namespace_begin(std)
namespace impl {


template<typename T> struct _refwrap {
  using type = decay_t<T>;
};

template<typename T> struct _refwrap<reference_wrapper<T>> {
  using type = add_lvalue_reference_t<T>;
};

template<typename T> struct refwrap {
  using type = typename _refwrap<remove_cv_t<remove_reference_t<T>>>::type;
};


} /* namespace std::impl */
_namespace_end(std)
