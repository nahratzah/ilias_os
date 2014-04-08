#include <stdimpl/simple_bitset.h>
#include <abi/memory.h>

_namespace_begin(std)
namespace impl {


simple_bitset::simple_bitset(size_type count)
: len_(count),
  data_(new uintptr_t[(count + bits - 1U) / bits])
{
  abi::memzero(data_, sizeof(uintptr_t) * ((count + bits - 1U) / bits));
}

simple_bitset::~simple_bitset() noexcept {
  delete[] data_;
}

bool simple_bitset::all_set() const noexcept {
  for (size_type i = 0; i < len_ / bits; ++i)
    if (~data_[i] != 0U) return false;

  const uintptr_t mask = (uintptr_t(1) << (len_ % bits)) - 1U;
  if ((data_[len_ / bits] & mask) != mask) return false;

  return true;
}

bool simple_bitset::operator[](size_type idx) const noexcept {
  assert(idx < len_);

  return data_[idx / bits] & (uintptr_t(1) << (idx % bits));
}

simple_bitset::proxy simple_bitset::operator[](size_type idx) noexcept {
  assert(idx < len_);

  return proxy(&data_[idx / bits], uintptr_t(1) << (idx % bits));
}


simple_bitset::proxy& simple_bitset::proxy::operator=(bool v) noexcept {
  assert(elem_);

  if (v)
    *elem_ |= mask_;
  else
    *elem_ &= ~mask_;
  return *this;
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
