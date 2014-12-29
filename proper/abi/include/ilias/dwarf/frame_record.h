#ifndef _ILIAS_DWARF_FRAME_RECORD_H_
#define _ILIAS_DWARF_FRAME_RECORD_H_

#include <cdecl.h>
#include <cstdint>
#include <cstddef>
#include <tuple>
#include <ilias/dwarf/read.h>

_namespace_begin(ilias)
namespace dwarf {


class frame_record {
 protected:
  template<typename T> using data_type =
      _namespace(std)::tuple<T, const void*>;

 public:
  using sbyte = _namespace(std)::int8_t;
  using ubyte = _namespace(std)::uint8_t;
  using uhalf = _namespace(std)::uint16_t;
  using uword = _namespace(std)::uint32_t;

  frame_record() noexcept = default;
  frame_record(const frame_record&) noexcept = default;
  frame_record& operator=(const frame_record&) noexcept = default;
  explicit frame_record(const void*) noexcept;
  ~frame_record() noexcept = default;

  bool is_valid() const noexcept { return valid_; }
  bool is_64bit() const noexcept { return is_64bit_; }
  bool is_32bit() const noexcept { return !is_64bit_; }
  bool is_cie() const noexcept { return is_valid() && is_cie_; }
  bool is_fde() const noexcept { return is_valid() && !is_cie_; }

  _namespace(std)::size_t size() const noexcept;
  const void* succ_ptr() const noexcept;
  frame_record succ() const noexcept;

 protected:
  static const void* advance_(const void*, _namespace(std)::size_t) noexcept;
  data_type<_namespace(std)::size_t> length_() const noexcept;

 private:
  const void* data_ = nullptr;
  _namespace(std)::size_t len_ = 0U;
  bool is_64bit_ = false;
  bool valid_ = false;
  bool is_cie_;
};


} /* namespace ilias::dwarf */
_namespace_end(ilias)

#include <ilias/dwarf/frame_record-inl.h>

#endif /* _ILIAS_DWARF_FRAME_RECORD_H_ */
