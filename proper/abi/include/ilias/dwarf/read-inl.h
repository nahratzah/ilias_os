#ifndef _ILIAS_EH_FRAME_READ_INL_H_
#define _ILIAS_EH_FRAME_READ_INL_H_

#include <ilias/dwarf/read.h>

_namespace_begin(ilias)
namespace dwarf {


template<typename T>
auto read_unaligned(const void* vptr) noexcept ->
    _namespace(std)::tuple<T, const void*> {
  using _namespace(std)::make_tuple;

  T v;
  __builtin_memcpy(&v, vptr, sizeof(v));
  return make_tuple(v,
                    reinterpret_cast<const unsigned char*>(vptr) + sizeof(v));
}


} /* namespace ilias::dwarf */
_namespace_end(ilias)

#endif /* _ILIAS_EH_FRAME_READ_INL_H_ */
