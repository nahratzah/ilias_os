#ifndef _ILIAS_DWARF_FRAME_RECORD_INL_H_
#define _ILIAS_DWARF_FRAME_RECORD_INL_H_

#include <ilias/dwarf/frame_record.h>
#include <ilias/dwarf/read.h>

_namespace_begin(ilias)
namespace dwarf {


inline auto frame_record::size() const noexcept -> _namespace(std)::size_t {
  return len_ + (is_64bit() ? 12U : 4U);
}

inline auto frame_record::advance_(const void* p, _namespace(std)::size_t n)
    noexcept -> const void* {
  return reinterpret_cast<const void*>(reinterpret_cast<uintptr_t>(p) + n);
}

inline auto frame_record::length_() const noexcept ->
    data_type<_namespace(std)::size_t> {
  using _namespace(std)::make_tuple;

  return make_tuple(len_, advance_(data_, (is_64bit() ? 12U : 4U)));
}


} /* namespace ilias::dwarf */
_namespace_end(ilias)

#endif /* _ILIAS_DWARF_FRAME_RECORD_INL_H_ */
