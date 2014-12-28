#ifndef _ILIAS_EH_FRAME_FDE_H_
#define _ILIAS_EH_FRAME_FDE_H_

#include <cdecl.h>
#include <cstdint>
#include <string>
#include <tuple>

_namespace_begin(ilias)
namespace dwarf {


class fde {
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
  fde(const void*) noexcept;
  ~fde() noexcept = default;

  bool is_valid() const noexcept { return valid_; }
  bool is_64bit() const noexcept { return is_64bit_; }
  bool is_32bit() const noexcept { return !is_64bit_; }

  uint64_t size() const noexcept;
  const void* succ_ptr() const noexcept;
  fde succ() const noexcept;

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

#include <ilias/dwarf/fde-inl.h>

#endif /* _ILIAS_EH_FRAME_FDE_H_ */
