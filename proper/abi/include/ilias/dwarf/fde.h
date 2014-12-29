#ifndef _ILIAS_EH_FRAME_FDE_H_
#define _ILIAS_EH_FRAME_FDE_H_

#include <cdecl.h>
#include <cstdint>
#include <string>
#include <tuple>
#include <ilias/dwarf/frame_record.h>

_namespace_begin(ilias)
namespace dwarf {


class fde
: public frame_record
{
 private:
  using uint32_t = _namespace(std)::uint32_t;
  using uint64_t = _namespace(std)::uint64_t;

 public:
  using address_type = _namespace(std)::uintptr_t;  // XXX

  /* Decoded form of data. */
  struct record {
    uint64_t length;
    uint64_t pointer;
    address_type initial_location;
    address_type address_range;
    _namespace(std)::tuple<const ubyte*, const ubyte*> instructions;

    bool valid : 1;
    bool is_64bit : 1;
  };

  fde() noexcept = default;
  fde(const fde&) noexcept = default;
  fde& operator=(const fde&) noexcept = default;
  explicit fde(const void*) noexcept;
  fde(const frame_record& fr) noexcept;

  record get_record() const noexcept;
};


} /* namespace ilias::dwarf */
_namespace_end(ilias)

#include <ilias/dwarf/fde-inl.h>

#endif /* _ILIAS_EH_FRAME_FDE_H_ */
