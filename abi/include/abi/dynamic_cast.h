#include <abi/abi.h>
#include <abi/typeinfo.h>

namespace __cxxabiv1 {

_cdecl_begin

void* __dynamic_cast(const void*, const abi::__class_type_info*,
                     const abi::__class_type_info*, ptrdiff_t);

_cdecl_end

} /* namespace __cxxabiv1 */


/****** INLINE ********************************************************/
namespace __cxxabiv1 {


inline constexpr __pbase_type_info::__pbase_type_info(
  const char* type_name, unsigned int flags, const std::type_info& pti)
: std::type_info(type_name),
  __flags(flags),
  __pointee(&pti)
{}

inline constexpr __pointer_to_member_type_info::__pointer_to_member_type_info(
  const char* type_name, unsigned int flags, const std::type_info& pti,
  const __class_type_info& ctx)
: __pbase_type_info(type_name, flags, pti),
  __context(&ctx)
{}


} /* namespace __cxxabiv1 */


/***** IMPL *************************************************************/
#include <abi/typeinfo.h>

namespace __cxxabiv1 {


class dyn_cast_response {
 public:
  const void* target = nullptr;
  bool is_down_cast = false;
  bool ambiguous = false;
  bool is_public = false;

  inline void merge(const dyn_cast_response& r) noexcept;
  inline const void* resolution() const noexcept;
};

inline void dyn_cast_response::merge(const dyn_cast_response& r) noexcept {
  if (!r.target) return;
  if (!target) {
    *this = r;
    return;
  }

  /* Note: both *this and r have target set. */

  /*
   * If both resolutions describe the same target, the result does
   * not become ambiguous.
   * (But if one of the responses is already ambiguous, it stays that way).
   */
  if (target == r.target) {
    ambiguous |= r.ambiguous;
    is_public |= r.is_public;
    is_down_cast |= r.is_down_cast;
    return;
  }

  /* If one of the casts is a down cast, use that. */
  if (is_down_cast) return;
  if (r.is_down_cast) {
    *this = r;
    return;
  }

  /* If only 1 is public, that one will be resolved. */
  if (is_public && !r.is_public) return;
  if (!is_public && r.is_public) {
    *this = r;
    return;
  }

  /*
   * Note: both *this and r have a different resolution,
   * both should be preserved, but instead we mark the resolution
   * as ambiguous.
   */
  r.ambiguous = true;
}

inline const void* dyn_cast_response::resolution() const noexcept {
  if (ambiguous) return nullptr;
  if (!is_public && !is_down_cast) return nullptr;
  return target;
}


class dyn_cast_request {
 public:
  constexpr dyn_cast_request(const void*,
                             const std::type_info*, const std::type_info*,
                             ptrdiff_t = -1);

  const void*const subject;
  const std::type_info*const subject_ti;
  const std::type_info*const target_ti;
  const bool is_cross_cast;

  dyn_cast_response resolve(const void*,
                            const __class_type_info*) const noexcept;
};

dyn_cast_request::dyn_cast_request(const void* subject,
                                   const std::type_info* subject_ti,
                                   const std::type_info* target_ti,
                                   ptrdiff_t src2dst_offset)
: subject(subject),
  subject_ti(subject_ti),
  target_ti(target_ti),
  is_cross_cast(src2dst_offset == -2)
{}

dyn_cast_response resolve(const void* p,
                          const __class_type_info* p_ti,
			  bool is_public) const noexcept {
  if (!p_ti) return nullptr;  // Cannot resolve without table.

  /* Cross-cast can never resolve to non-public bases. */
  if (this->is_cross_cast && !is_public) return;

  /*
   * Abi spec:
   * compiler will not invoke __dynamic_cast if dst is a public base of src.
   */
  if (p == subject && *p_ti == *subject_ti) return;

  /* Pass resolution up the chain. */
  if (p_ti != target_ti) return p_ti->__dyn_up_cast(p, *this, is_public);

  dyn_cast_response response;
  response.target = vp;
  response.is_public = is_public;
  if (!this->is_cross_cast) {
    if (p_ti->find_subject(subject, subject_ti))
      response.is_cross_cast = false;
  }
  return response;
}


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


const void* __class_type_info::__dynamic_cast(const void* p,
                                              const std::type_info& ti)
                                                       const noexcept {
  return (*this == ti ? p : nullptr);
}

const void* __si_class_type_info::__dynamic_cast(const void* p,
                                                 const std::type_info& ti)
                                                          const noexcept {
  return (*this == ti ? p : __base_type->__dynamic_cast(p, ti));
}

const void* __vmi_class_type_info::__dynamic_cast(const void* p,
                                                  const std::type_info& ti)
                                                           const noexcept {
  if (*this == ti) return p;

  const __base_class_type_info* begin = &__base_info[0];
  const __base_class_type_info* end = begin + __base_count;
  for (const __base_class_type_info* i = begin; i != end; ++i) {
    const auto& base_ti = *i->__base_type;
    const auto off = __base_clas_type_info::get_offset(*i);
    const auto fl = __base_clas_type_info::get_flags(*i);

    const void* rv = ti.__dynamic_cast(void_add(p, off), ti);
    if (rv) return rv;
  }

  return nullptr;
}


namespace {


/*
 * Data preceding the vtable.
 */
struct vtable_precede_ {
  const void* offset_to_void_;  // actually ptrdiff_t
  const std::type_info* leaf_ti_;
};

struct vtt;
using vtt_pointer = const vtt*;


inline const void* void_add(const void* p, ptrdiff_t d) noexcept {
  typedef const uint8_t* byte;

  return (p == nullptr ? nullptr : reinterpret_cast<byte>(p) + d);
}

inline const void* void_sub(const void* p, ptrdiff_t d) noexcept {
  return void_add(p, -d);
}

inline const vtable_precede_* vtt_precede(vtt_pointer v) noexcept {
  const void* rv_addr = void_sub(v, sizeof(vtable_precede_));
  return reinterpret_cast<const vtable_precede_*>(rv_addr);
}

inline ptrdiff_t vtp_offset(const vtable_precede_& vtp) noexcept {
  return static_cast<ptrdiff_t>(vtp.offset_to_void_);
}


} /* namespace __cxxabiv1::<unnamed> */


void* __dynamic_cast(const void* subject, const __class_type_info* ti_src,
                     const __class_type_info* ti_dst,
                     ptrdiff_t src2dst_offset) {
  /*
   * subject: source address to be adjusted; nonnull, and since
   *   the source object is polymorphic, *(void**)subject is a virtual pointer.
   * src: static type of the source object.
   * dst: destination type (the "T" in "dynamic_cast<T>(v)").
   * src2dst_offset: a static hint about the location of the source subobject
   *   with respect to the complete object;
   *   special negative values are:
   *     -1: no hint
   *     -2: src is not a public base of dst
   *     -3: src is a multiple public base type, but never a virtual type
   *  otherwise, the src type is a unique public nonvirtual base type of dst
   *  at offset src2dst_offset from the origin of dst.
   */

  /*
   * Observation: if src is a non-virtual base of dst,
   *   the offset of src in dst >= 0,
   *   therefore, src2dst_offset >= 0, if provided.
   * src2dst models the void_add() required to go from derived to base,
   *   which in this case means from dst to src.
   * Since we want to go from src to dst (i.e. the opposite direction)
   *   we need to void_sub() from subject instead.
   */
  if (src2dst_offset >= 0) return void_sub(subject, src2dst_offset);

  /* Calculate address of the entire object (subject_base_addr). */
  vtt_pointer subject_vtt = *reinterpret_cast<const vtt_pointer*>(subject);
  const vtable_precede_* subject_vtp = vtt_precede(subject_vtt);
  const void* subject_base_addr = void_add(subject, vtp_offset(*subject_vtp));

  /*
   * Calculate address of subject_base std::type_info.
   * Note: subject is a class (otherwise it would not be dynamic-castable),
   *   therefore subject_base is a class,
   *   therefore the std::type_info for subject_base is an instance of
   *   __class_type_info.
   */
  vtt_pointer subject_base_vtt =
    *reinterpret_cast<const vtt_pointer*>(subject_base_addr);
  const vtable_precede_* subject_base_vtp = vtt_precede(subject_base_vtt);
  assert(vtp_offset(*subject_base_vtp) == 0);
  const __class_type_info* subject_base_ti =
    static_cast<const __class_type_info*>(subject_base_vtp->leaf_ti);

  dyn_cast_request request = dyn_cast_request(subject, ti_src, ti_dst,
                                              src2dst_offset);
  dyn_cast_response response = request.resolve(subject_base_addr,
                                               subject_base_ti);
  return response.resolution();
}


/*
 * __fundamental_type_info for cartesian product of:
 * - void, std::nullptr_t, bool, wchar_t, char, unsigned char, signed char,
 *   short, unsigned short, int, unsigned int, long, unsigned long,
 *   long long, unsigned long long, float, double, long double,
 *   char16_t, char32_t,
 *   IEEE 754r decimal and half-precision floating point types
 * - X, X*, const X*
 */


} /* namespace __cxxabiv1 */
