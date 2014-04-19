#include <stdimpl/simple_bitset.h>
#include <abi/memory.h>
#include <cstring>
#include <memory>
#include <new>

_namespace_begin(std)
namespace impl {


simple_bitset::simple_bitset(size_type count)
: len_(count),
  data_(count == 0 ?
        nullptr :
        get<0>(get_temporary_buffer<uintptr_t>((count + bits - 1U) / bits)))
{
  if (data_ != nullptr)
    bzero(data_, sizeof(uintptr_t) * ((count + bits - 1U) / bits));
}

bool simple_bitset::all_set() const noexcept {
  for (size_type i = 0; i < len_ / bits; ++i)
    if (~data_[i] != 0U) return false;

  const uintptr_t mask = (uintptr_t(1) << (len_ % bits)) - 1U;
  if ((data_[len_ / bits] & mask) != mask) return false;

  return true;
}


bool exchange(simple_bitset::proxy& self, bool v) noexcept {
  bool rv = self;
  self = v;
  return rv;
}

void swap(simple_bitset::proxy& self, simple_bitset::proxy& other) noexcept {
  other = exchange(self, other);
}


} /* namespace std::impl */
_namespace_end(std)
