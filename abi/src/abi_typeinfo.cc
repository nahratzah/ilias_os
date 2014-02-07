#include <abi/typeinfo.h>
#include <abi/dynamic_cast.h>

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


__has_base_result __class_type_info::__has_base(
    const void* v, const void* p, const std::type_info& p_ti,
    bool) const noexcept {
  return (p == v && *this == p_ti ? __has_base_non_virtual : __has_base_no);
}

__has_base_result __si_class_type_info::__has_base(
    const void* v, const void* p, const std::type_info& p_ti,
    bool) const noexcept {
  if (p == v && *this == p_ti) return __has_base_non_virtual;
  return __base_type->__has_base(v, p, p_ti);
}

__has_base_result __vmi_class_type_info::__has_base(
    const void* v, const void* p, const std::type_info& p_ti,
    bool skip_virtual) const noexcept {
  if (p == v && *this == p_ti) return __has_base_non_virtual;

  const __base_class_type_info*const begin = &__base_info[0];
  const __base_class_type_info*const end = begin + __base_count;
  for (const __base_class_type_info* i = begin; i != end; ++i) {
    const void* v_of_i = reinterpret_cast<const uint8_t*>(v) +
                         __base_class_type_info::get_offset(*i);
    const auto fl = __base_class_type_info::get_flags(*i);
    bool is_public = (fl & __base_class_type_info::__public_mask);
    if (!is_public) continue;

    bool is_virt = (fl & __base_class_type_info::__virtual_mask);
    if (is_virt && skip_virtual) continue;

    __has_base_result hbrv = i->__base_type->__has_base(v_of_i, p, p_ti);
    if (hbrv != __has_base_no) return (is_virt ? __has_base_virtual : hbrv);
  }

  return __has_base_no;
}


__dyn_cast_response __class_type_info::__dyn_cast_support(
    const void* v, const __dyn_cast_request& request) const noexcept {
  return request.leaf(v, *this);
}

__dyn_cast_response __si_class_type_info::__dyn_cast_support(
    const void* v, const __dyn_cast_request& request) const noexcept {
  __dyn_cast_response r = request.leaf(v, *this);
  if (!r.return_now(request, v, *this))
    r.merge(request.resolve(v, *__base_type));
  return r;
}

__dyn_cast_response __vmi_class_type_info::__dyn_cast_support(
    const void* v, const __dyn_cast_request& request) const noexcept {
  __dyn_cast_response r = request.leaf(v, *this);

  const __base_class_type_info*const begin = &__base_info[0];
  const __base_class_type_info*const end = begin + __base_count;
  for (const __base_class_type_info* i = begin;
       !r.return_now(request, v, *this) && i != end;
       ++i) {
    const void* v_of_i = reinterpret_cast<const uint8_t*>(v) +
                         __base_class_type_info::get_offset(*i);
    const auto fl = __base_class_type_info::get_flags(*i);
    bool is_public = (fl & __base_class_type_info::__public_mask);
    if (is_public)
      r.merge(request.resolve(v_of_i, *i->__base_type));
  }

  return r;
}


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
