#include <abi/typeinfo.h>

namespace __cxxabiv1 {


__fundamental_type_info::~__fundamental_type_info() noexcept {}
__array_type_info::~__array_type_info() noexcept {}
__function_type_info::~__function_type_info() noexcept {}
__enum_type_info::~__enum_type_info() noexcept {}

__class_type_info::~__class_type_info() noexcept {}
__si_class_type_info::~__si_class_type_info() noexcept {}
__vmi_class_type_info::~__vmi_class_type_info() noexcept {}
__pbase_type_info::~__pbase_type_info() noexcept {}
__pointer_type_info::~__pointer_type_info() noexcept {}
__pointer_to_member_type_info::~__pointer_to_member_type_info() noexcept {}


/*
 * XXX: __fundamental_type_info for cartesian product of:
 * - void, std::nullptr_t, bool, wchar_t, char, unsigned char, signed char,
 *   short, unsigned short, int, unsigned int, long, unsigned long,
 *   long long, unsigned long long, float, double, long double,
 *   char16_t, char32_t,
 *   IEEE 754r decimal and half-precision floating point types
 * - X, X*, const X*
 */


} /* namespace __cxxabiv1 */
