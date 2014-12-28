#ifndef _ILIAS_EH_FRAME_CIE_INL_H_
#define _ILIAS_EH_FRAME_CIE_INL_H_

#include <ilias/dwarf/cie.h>
#include <ilias/dwarf/read.h>

_namespace_begin(ilias)
namespace dwarf {


inline cie::cie(const void* data) noexcept
: data_(data)
{
  using _namespace(std)::tie;

  if (data_ == nullptr) return;

  const void* ptr = data_;
  uint32_t len32;
  tie(len32, ptr) = read_unaligned<uint32_t>(ptr);
  valid_ = (len32 == 0xffffffffU || len32 < 0xfffffff0U);
  if (!valid_) return;

  is_64bit_ = (len32 == 0xffffffffU);
  if (is_64bit_)
    tie(len_, ptr) = read_unaligned<uint64_t>(ptr);
  else
    len_ = len32;
}

inline auto cie::size() const noexcept -> uint64_t {
  return len_ + (is_64bit() ? 12 : 4);
}

inline auto cie::succ_ptr() const noexcept -> const void* {
  using _namespace(std)::tie;

  const void* ptr = nullptr;
  if (is_valid()) {
    _namespace(std)::size_t len;
    tie(len, ptr) = length_();
    ptr = advance_(ptr, len);
  }
  return ptr;
}

inline auto cie::succ() const noexcept -> cie {
  return cie(succ_ptr());
}

inline auto cie::advance_(const void* p, _namespace(std)::size_t n) noexcept ->
    const void* {
  return reinterpret_cast<const void*>(reinterpret_cast<uintptr_t>(p) + n);
}

inline auto cie::length_() const noexcept ->
    data_type<_namespace(std)::size_t> {
  using return_type = data_type<_namespace(std)::size_t>;

  return return_type(len_, advance_(data_, (is_64bit() ? 12 : 4)));
}


} /* namespace ilias::dwarf */
_namespace_end(ilias)

#endif /* _ILIAS_EH_FRAME_CIE_INL_H_ */
