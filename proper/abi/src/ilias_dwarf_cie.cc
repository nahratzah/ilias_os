#include <ilias/dwarf/cie.h>

_namespace_begin(ilias)
namespace dwarf {


auto cie::get_record() const noexcept -> record {
  using _namespace(std)::tie;

  record rv;
  rv.valid = is_cie();
  rv.is_64bit = is_64bit();

  if (is_cie()) {
    const void __attribute__((aligned(1))) * ptr;
    tie(rv.length, ptr) = length_();
    if (is_64bit())
      tie(rv.id, ptr) = read_unaligned<uint64_t>(ptr);
    else
      tie(rv.id, ptr) = read_unaligned<uint32_t>(ptr);
    tie(rv.version, ptr) = read_unaligned<ubyte>(ptr);
    tie(rv.augmentation, ptr) = read_utf8(ptr);
    tie(rv.address_size, ptr) = read_unaligned<ubyte>(ptr);
    tie(rv.segment_size, ptr) = read_unaligned<ubyte>(ptr);
    tie(rv.code_alignment_factor, ptr) = read_uleb128(ptr);
    tie(rv.data_alignment_factor, ptr) = read_uleb128(ptr);
    tie(rv.return_address_register, ptr) = read_uleb128(ptr);
    rv.initial_instructions = _namespace(std)::make_tuple(
        static_cast<const ubyte*>(ptr),
        static_cast<const ubyte*>(succ_ptr()));

    /*
     * Optional:
     * reduce rv.initial_instructions to not include the NOP padding.
     */
  }

  return rv;
}


} /* namespace ilias::dwarf */
_namespace_end(ilias)
