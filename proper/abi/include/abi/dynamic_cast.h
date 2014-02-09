#ifndef _ABI_DYNAMIC_CAST_H_
#define _ABI_DYNAMIC_CAST_H_

#include <cdecl.h>
#include <abi/abi.h>
#include <abi/typeinfo.h>

namespace __cxxabiv1 {

_cdecl_begin

void* __dynamic_cast(const void*, const abi::__class_type_info*,
                     const abi::__class_type_info*, ptrdiff_t) noexcept;

_cdecl_end


class __dyn_cast_request {
  friend __dyn_cast_response;

 public:
  __dyn_cast_request(const void*, const std::type_info&, const std::type_info&,
                     ptrdiff_t = -1) noexcept;
  __dyn_cast_request(const __dyn_cast_request&) = default;
  __dyn_cast_request& operator=(const __dyn_cast_request&) = default;

  __dyn_cast_response resolve(const void*,
                              const __class_type_info&) const noexcept;
  __dyn_cast_response leaf(const void*,
                           const __class_type_info&) const noexcept;

 private:
  const void* subject_;
  const std::type_info& subject_ti_;
  const std::type_info& target_ti_;
  bool is_cross_cast_ = false;
  bool skip_v_base_ = false;  // Skip virtual bases in __has_base()
};

class __dyn_cast_response {
 public:
  __dyn_cast_response() = default;
  __dyn_cast_response(const __dyn_cast_response&) = default;
  __dyn_cast_response& operator=(const __dyn_cast_response&) = default;

  constexpr __dyn_cast_response(const void*, __has_base_result);

  inline bool return_now(const __dyn_cast_request&,
                         const void*, const __class_type_info&) const noexcept;
  inline const void* resolution() const noexcept;
  void merge(__dyn_cast_response r) noexcept;

 private:
  const void* target_ = nullptr;
  __has_base_result subj_visited_ = __has_base_no;
  bool ambiguous_ = false;
};


} /* namespace __cxxabiv1 */

#include <abi/dynamic_cast-inl.h>

#endif /* _ABI_DYNAMIC_CAST_H_ */
