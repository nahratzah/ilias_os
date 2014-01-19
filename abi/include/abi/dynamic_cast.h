#include <abi/abi.h>
#include <abi/typeinfo.h>

namespace __cxxabiv1 {

_cdecl_begin

void* __dynamic_cast(const void*, const abi::__class_type_info*,
                     const abi::__class_type_info*, ptrdiff_t);

_cdecl_end


class __dyn_cast_request {
  friend __dyn_cast_response;

 public:
  __dyn_cast_request(const void*, const std::type_info&, const std::type_info&,
                     ptrdiff_t = -1) noexcept;
  __dyn_cast_request(const __dyn_cast_request&) = default;
  __dyn_cast_request& operator=(const __dyn_cast_request&) = default;

  __dyn_cast_response resolve(const void*,
                              const __class_type_info*) const noexcept;
  __dyn_cast_response leaf(const void*,
                           const __class_type_info*) const noexcept;

 private:
  const void* subject_;
  const std::type_info& subject_ti_;
  const std::type_info& target_ti_;
  bool is_cross_cast_ = false;
  bool skip_v_base_ = false;  // Skip virtual bases in __has_base()
};

class __dyn_cast_response {
 public:
  __dyn_cast_respone() = default;
  __dyn_cast_respone(const __dyn_cast_response&) = default;
  __dyn_cast_respone& operator=(const __dyn_cast_response&) = default;

  inline __dyn_cast_response(const void*, __has_base_result) noexcept;

  inline bool return_now(const __dyn_cast_request&,
                         const void*) const noexcept;
  inline const void* resolution() const noexcept;
  void merge(__dyn_cast_response r) const noexcept;

 private:
  const void* target_ = nullptr;
  __has_base_result subj_visited_ = __has_base_no;
  bool ambiguous_ = false;
};

constexpr __dyn_cast_response::__dyn_cast_response(
    const void* target, __has_base_result subj_visited)
: target_(target),
  subj_visited_(subj_visited)
{}

inline bool __dyn_cast_response::return_now(
    const __dyn_cast_request& req, const void* v) const noexcept {
  if (!target) return false;
  if (target == v) return true;  // Type cannot inherit itself.

  /*
   * Stop the search if:
   * - target located and subject is a non-virtual base class of target
   *   (we'll never find another target having subject as a base, due to
   *   subject being a non-virtual base)
   * - multiple targets located with subject as base (virtual inheritance)
   *   (we'll never find anything that inherits from subject, but is better
   *   than what we've found)
   * - multiple targets located and subject is not a base of target-type
   *   (compiler guarantees cross cast, which has been proven to be ambiguous).
   */
  if (subj_visited_ == __has_base_non_virtual) return true;
  if (subj_visited_ == __has_base_virtual && ambiguous_) return true;
  if (ambiguous_ && req.is_cross_cast_) return true;

  /* Default: we are not sure, therefore continue search. */
  return false;
}

inline const void* __dyn_cast_response::resolution() const noexcept {
  return (ambiguous_ ? nullptr : target_);
}


} /* namespace __cxxabiv1 */
