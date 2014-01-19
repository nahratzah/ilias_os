#include <abi/dynamic_cast.h>


namespace __cxxabiv1 {


__dyn_cast_request::__dyn_cast_request(const void* subject,
                                       const std::type_info& subject_ti,
                                       const std::type_info& target_ti,
				       ptrdiff_t s2d_off) noexcept
: subject_(subject),
  subject_ti_(subject_ti),
  target_ti_(target_ti),
  is_cross_cast_(s2d_off == -2),  // src is not a public base of dst
  skip_v_base_(s2d_off == -3)
{}

__dyn_cast_response __dyn_cast_request::leaf(
    const void* v, const __class_type_info& v_ti) const noexcept {
  if (v_ti != target_ti_) return __dyn_cast_response();

  bool subj_is_base = !is_cross_cast_ && subject_ != nullptr &&
                      v_ti.__has_base(subject_, subject_ti_, skip_v_base_);
  return __dyn_cast_response(v, subj_is_base);
}

__dyn_cast_response __dyn_cast_request::resolve(
    const void* p, const __class_type_info& p_ti) const noexcept {
  /* Cannot search without type info. */
  if (!p_ti) return __dyn_cast_response();

  /*
   * Since we are only invoked for subject is-base-of target,
   * skip searches in subject type.
   */
  if (is_cross_cast_ && p == subject_ && p_ti == subject_ti_)
    return __dyn_cast_response();

  /* Cascade into v. */
  return p_ti->__dyn_cast_support(p, *this);
}

void __dyn_cast_response::merge(__dyn_cast_response r) const noexcept {
  /* Handle case were at least one of { *this, r } has not located anything. */
  if (!r.target_) return;  // Nothing there.
  if (!target_) {  // r has a match, while *this does not.
    *this = r;
    return;
  }

  /* Select match that contains subject, if such a match exists. */
  if (subj_visited_ && !r.subj_visited_) return;  // *this has better match.
  if (r.subj_visited_ && !subj_visited_) {  // r has better match.
    *this = r;
    return;
  }

  /* If ambiguity strikes, preserve it. */
  if (ambiguous_) return;  // Already ambiguous.
  if (r.ambiguous_) {  // Merge with ambiguous result -> ambiguous.
    ambiguous_ = true;
    return;
  }

  /* Handle same (non-ambiguous) match. */
  if (target_ == r.target_) return;

  /* Different targets, same rank. */
  ambiguous_ = true;
  return;
}


namespace {


struct vtt;

inline std::type_info* vtt_2_ti(const vtt* v) noexcept {
  const void*const* entry = static_cast<const void*const*>(v) - 1;
  return static_cast<const std::type_info*>(*entry);
}

inline ptrdiff_t vtt_2_off(const vtt* v) noexcept {
  const void*const* entry = static_cast<const void*const*>(v) - 2;
  return static_cast<ptrdiff_t>(*entry);
}

inline const vtt* get_vtt(const void* obj) noexcept {
  return *static_cast<const vtt*const*>(obj);
}

inline const void* void_add(const void* p, ptrdiff_t d) noexcept {
  typedef const uint8_t byte;

  return (p == nullptr ? nullptr : static_cast<byte*>(p) + d);
}

inline const void* void_sub(const void* p, ptrdiff_t d) noexcept {
  return void_add(p, -d);
}

inline const void* base_addr(const void* obj) noexcept {
  return void_add(obj, vtt_2_off(get_vtt(obj)));
}

inline const __class_type_info* base_type(const void* obj) noexcept {
  return static_cast<const __class_type_info*>(vtt_2_ti(get_vtt(obj)));
}


} /* namespace __cxxabiv1::<unnamed> */


void* __dynamic_cast(const void* subject, const __class_type_info* ti_src,
                     const class_type_info* ti_dst,
                     ptrdiff_t src2dst_offset) noexcept {
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
  const void*const md = base_addr(subject);
  const __class_type_info*const md_ti = base_type(subject);

  /*
   * Observation: if src is a non-virtual base of dst,
   *   the offset of src in dst >= 0,
   *   therefore, src2dst_offset >= 0, if provided.
   * src2dst models the void_add() required to go from derived to base,
   *   which in this case means from dst to src.
   * Since we want to go from src to dst (i.e. the opposite direction)
   *   we need to void_sub() from subject instead.
   *
   * Ofcourse, we still have to check that dst is a public base of the derived
   * type (since dynamic_cast may only consider public bases of the
   * most derived object).
   * Note that the compiler guarantees that subject is a public base of target,
   * thus if target is public, so if subject.
   */
  if (src2dst_offset >= 0) {
    const void* maybe = void_sub(subject, src2dst_offset);
    if (_predict_true(md_ti->__has_base(md, maybe, ti_dst) !=
                      __has_base_no))
      return maybe;
  }

  const dyn_cast_request request = dyn_cast_request(subject, ti_src, ti_dst,
                                                    src2dst_offset);
  const dyn_cast_response response = request.resolve(md, md_ti);
  return response.resolution();
}


} /* namespace __cxxabiv1 */
