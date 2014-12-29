#ifndef _ILIAS_EH_FRAME_READ_H_
#define _ILIAS_EH_FRAME_READ_H_

#include <cdecl.h>
#include <tuple>
#include <string>
#include <cstdint>

_namespace_begin(ilias)
namespace dwarf {


auto read_leb128(const void __attribute__((aligned(1))) * vptr) noexcept ->
    _namespace(std)::tuple<_namespace(std)::uint64_t, const void*>;

template<typename T>
auto read_unaligned(const void __attribute__((aligned(1))) * vptr) noexcept ->
    _namespace(std)::tuple<T, const void*>;

auto read_utf8(const void __attribute__((aligned(1))) * vptr) noexcept ->
    _namespace(std)::tuple<_namespace(std)::string_ref, const void*>;


} /* namespace ilias::dwarf */
_namespace_end(ilias)

#include <ilias/dwarf/read-inl.h>

#endif /* _ILIAS_EH_FRAME_READ_H_ */
