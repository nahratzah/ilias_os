#ifndef _ILIAS_EH_FRAME_FDE_INL_H_
#define _ILIAS_EH_FRAME_FDE_INL_H_

#include <ilias/dwarf/fde.h>
#include <ilias/dwarf/read.h>

_namespace_begin(ilias)
namespace dwarf {


inline fde::fde(const void* data) noexcept
: frame_record(data)
{}

inline fde::fde(const frame_record& fr) noexcept
: frame_record(fr)
{}


} /* namespace ilias::dwarf */
_namespace_end(ilias)

#endif /* _ILIAS_EH_FRAME_FDE_INL_H_ */
