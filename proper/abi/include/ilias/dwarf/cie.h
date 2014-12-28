#ifndef _ILIAS_EH_FRAME_CIE_H_
#define _ILIAS_EH_FRAME_CIE_H_

#include <cdecl.h>
#include <cstdint>
#include <string>
#include <tuple>

_namespace_begin(ilias)
namespace dwarf {


class cie {
 private:
  template<typename T> using data_type =
      _namespace(std)::tuple<T, const void*>;

  using uint32_t = _namespace(std)::uint32_t;
  using uint64_t = _namespace(std)::uint64_t;

 public:
  using sbyte = _namespace(std)::int8_t;
  using ubyte = _namespace(std)::uint8_t;
  using uhalf = _namespace(std)::uint16_t;
  using uword = _namespace(std)::uint32_t;

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
  cie(const void*) noexcept;
  ~cie() noexcept = default;

  bool is_valid() const noexcept { return valid_; }
  bool is_64bit() const noexcept { return is_64bit_; }
  bool is_32bit() const noexcept { return !is_64bit_; }

  uint64_t size() const noexcept;
  const void* succ_ptr() const noexcept;
  cie succ() const noexcept;

  record get_record() const noexcept;

 private:
  static const void* advance_(const void*, _namespace(std)::size_t) noexcept;
  data_type<_namespace(std)::size_t> length_() const noexcept;

  _namespace(std)::size_t len_ = 0U;
  const void* data_ = nullptr;
  bool is_64bit_ = false;
  bool valid_ = false;
};


} /* namespace ilias::dwarf */
_namespace_end(ilias)

#include <ilias/dwarf/cie-inl.h>

#endif /* _ILIAS_EH_FRAME_CIE_H_ */
