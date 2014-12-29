#include <ilias/dwarf/frame_record_collection.h>
#include <algorithm>

_namespace_begin(ilias)
namespace dwarf {


auto frame_record_collection::size() const noexcept -> size_type {
  return _namespace(std)::distance(begin(), end());
}


} /* namespace ilias::dwarf */
_namespace_end(ilias)
