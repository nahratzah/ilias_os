#include <ilias/dwarf/frame_record.h>

_namespace_begin(ilias)
namespace dwarf {


frame_record::frame_record(const void* data) noexcept
: data_(data)
{
  using _namespace(std)::tie;

  if (data_ == nullptr) return;

  const void* ptr = data_;
  _namespace(std)::uint32_t len32;
  tie(len32, ptr) = read_unaligned<uint32_t>(ptr);
  valid_ = (len32 == 0xffffffffU || len32 < 0xfffffff0U);
  if (!valid_) return;

  is_64bit_ = (len32 == 0xffffffffU);
  if (is_64bit_)
    tie(len_, ptr) = read_unaligned<uint64_t>(ptr);
  else
    len_ = len32;
}

auto frame_record::succ_ptr() const noexcept -> const void* {
  using _namespace(std)::tie;

  const void* ptr = nullptr;
  if (is_valid()) {
    _namespace(std)::size_t len;
    tie(len, ptr) = length_();
    ptr = advance_(ptr, len);
  }
  return ptr;
}

auto frame_record::succ() const noexcept -> frame_record {
  return frame_record(succ_ptr());
}


} /* namespace ilias::dwarf */
_namespace_end(ilias)
