#include <cdecl.h>
#include <stdimpl/member_check.h>
#include <type_traits>

_namespace_begin(std)
namespace impl {

struct unique_ptr_support {
  _MEMBER_TYPE_CHECK(pointer);

  template<typename, typename D> struct with_pointer_type {
    using type = typename D::pointer;
  };
  template<typename T, typename> struct without_pointer_type {
    using type = T*;
  };
  template<typename T, typename D> using pointer_type = typename conditional_t<
      member_type_check_pointer<remove_reference_t<D>>::value,
      with_pointer_type<T, D>,
      without_pointer_type<T, D>>::type;
};

} /* namespace std::impl */
_namespace_end(std)
