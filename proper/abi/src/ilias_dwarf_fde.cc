#include <ilias/dwarf/fde.h>

_namespace_begin(ilias)
namespace dwarf {


auto fde::get_record() const noexcept -> record {
  using _namespace(std)::tie;

  record rv;
  rv.valid = is_fde();
  rv.is_64bit = is_64bit();

  if (is_fde()) {
    const void __attribute__((aligned(1))) * ptr;
    tie(rv.length, ptr) = length_();
    if (is_64bit())
      tie(rv.pointer, ptr) = read_unaligned<uint64_t>(ptr);
    else
      tie(rv.pointer, ptr) = read_unaligned<uint32_t>(ptr);
    // We assume we're not using segment selectors.
    tie(rv.initial_location, ptr) = read_unaligned<address_type>(ptr);
    tie(rv.address_range, ptr) = read_unaligned<address_type>(ptr);
    rv.instructions = _namespace(std)::make_tuple(
        static_cast<const ubyte*>(ptr),
        static_cast<const ubyte*>(succ_ptr()));

    /*
     * Optional:
     * reduce rv.instructions to not include the NOP padding.
     */
  }

  return rv;
}


} /* namespace ilias::dwarf */
_namespace_end(ilias)
