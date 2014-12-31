#include <ilias/dwarf/read.h>
#include <cstdint>
#include <cassert>

_namespace_begin(ilias)
namespace dwarf {


auto read_uleb128(const void __attribute__((aligned(1))) * vptr) noexcept ->
    _namespace(std)::tuple<_namespace(std)::uint64_t, const void*> {
  using return_type =
      _namespace(std)::tuple<_namespace(std)::uint64_t, const void*>;
  using _namespace(std)::uint64_t;
  using _namespace(std)::uint8_t;

  using rv_type = uint64_t;

  rv_type rv = 0U;
  const uint8_t* ptr = static_cast<const uint8_t*>(vptr);
  uint8_t rd;
  unsigned int shift = 0;
  do {
    assert(shift < 64);

    rd = *ptr++;
    rv |= rv_type(rd & 0x7fU) << shift;
    shift += 7;
  } while (rd & 0x80);
  return return_type(rv, ptr);
}

auto read_sleb128(const void __attribute__((aligned(1))) * vptr) noexcept ->
    _namespace(std)::tuple<_namespace(std)::int64_t, const void*> {
  using return_type =
      _namespace(std)::tuple<_namespace(std)::int64_t, const void*>;
  using _namespace(std)::int64_t;
  using _namespace(std)::uint8_t;

  using rv_type = uint64_t;

  rv_type rv = 0U;
  const uint8_t* ptr = static_cast<const uint8_t*>(vptr);
  uint8_t rd;

  rd = *ptr++;
  if (rd & 0x40) rv = ~int64_t(0x7f);  // Sign-extend.
  rv |= rd;

  unsigned int shift = 7;
  while (rd & 0x80) {
    assert(shift < 64);

    rd = *ptr++;
    rv &= ~(int64_t(0x7fU) << shift);
    rv |= rv_type(rd & 0x7fU) << shift;
    shift += 7;
  }
  return return_type(rv, ptr);
}

auto read_utf8(const void __attribute__((aligned(1))) * vptr) noexcept ->
    _namespace(std)::tuple<_namespace(std)::string_ref, const void*> {
  using return_type =
      _namespace(std)::tuple<_namespace(std)::string_ref, const void*>;

  _namespace(std)::string_ref rv = static_cast<const char*>(vptr);
  const char* last_byte = rv.end();
  return return_type(rv, last_byte + 1U);
}


} /* namespace ilias::dwarf */
_namespace_end(ilias)
