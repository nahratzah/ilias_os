#ifndef _ILIAS_EH_FRAME_CIE_H_
#define _ILIAS_EH_FRAME_CIE_H_

#include <cdecl.h>
#include <cstdint>
#include <string>
#include <tuple>
#include <ilias/dwarf/frame_record.h>

_namespace_begin(ilias)
namespace dwarf {


class cie
: public frame_record
{
 private:
  using uint32_t = _namespace(std)::uint32_t;
  using uint64_t = _namespace(std)::uint64_t;

 public:
  /* Decoded form of data. */
  struct record {
    _namespace(std)::string_ref augmentation;
    _namespace(std)::tuple<const ubyte*, const ubyte*> initial_instructions;
    uint64_t length;
    uint64_t id;
    uint64_t code_alignment_factor;
    uint64_t data_alignment_factor;
    uint64_t return_address_register;
    ubyte version;
    ubyte address_size;
    ubyte segment_size;
    bool valid : 1;
    bool is_64bit : 1;
  };

  cie() noexcept = default;
  cie(const cie&) noexcept = default;
  cie& operator=(const cie&) noexcept = default;
  explicit cie(const void*) noexcept;
  cie(const frame_record&) noexcept;

  record get_record() const noexcept;
};


} /* namespace ilias::dwarf */
_namespace_end(ilias)

#include <ilias/dwarf/cie-inl.h>

#endif /* _ILIAS_EH_FRAME_CIE_H_ */
