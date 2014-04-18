#ifndef _STDIMPL_SIMPLE_BITSET_H_
#define _STDIMPL_SIMPLE_BITSET_H_

#include <cdecl.h>
#include <abi/abi.h>
#include <cassert>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <utility>

_namespace_begin(std)
namespace impl {


class simple_bitset {
 public:
  using size_type = size_t;
  class proxy;

  simple_bitset(size_type count);
  simple_bitset(const simple_bitset&) = delete;
  simple_bitset& operator=(const simple_bitset&) = delete;

  simple_bitset(simple_bitset&& other) noexcept
  : len_(other.len_),
    data_(exchange(other.data_, nullptr))
  {}

  ~simple_bitset() noexcept;

  bool all_set() const noexcept;
  bool operator[](size_type idx) const noexcept;
  proxy operator[](size_type idx) noexcept;

  explicit operator bool() const noexcept {
    return data_ != nullptr || len_ == 0;
  }

 private:
  static constexpr size_t bits = sizeof(uintptr_t) * CHAR_BIT;

  size_type len_;
  uintptr_t* data_;
};

class simple_bitset::proxy {
 private:
  friend simple_bitset;

  constexpr proxy(uintptr_t* elem, uintptr_t mask) noexcept
  : elem_(elem),
    mask_(mask)
  {}

 public:
  proxy() = default;
  proxy(const proxy&) = default;
  proxy& operator=(const proxy&) = default;

  operator bool() const noexcept { return *elem_ & mask_; }
  bool operator==(bool v) noexcept { return bool(*this) == v; }
  bool operator!=(bool v) noexcept { return !(*this == v); }

  proxy& operator=(bool v) noexcept;
  friend bool exchange(proxy& self, bool v) noexcept;
  friend void swap(proxy& self, proxy& other) noexcept;

 private:
  uintptr_t* elem_ = nullptr;
  uintptr_t mask_ = 0;
};


inline simple_bitset::~simple_bitset() noexcept {
  if (data_) return_temporary_buffer(data_);
}

inline bool simple_bitset::operator[](size_type idx) const noexcept {
  assert(idx < len_);

  return data_[idx / bits] & (uintptr_t(1) << (idx % bits));
}

inline simple_bitset::proxy simple_bitset::operator[](size_type idx) noexcept {
  assert(idx < len_);

  return proxy(&data_[idx / bits], uintptr_t(1) << (idx % bits));
}


inline simple_bitset::proxy& simple_bitset::proxy::operator=(bool v) noexcept {
  assert(elem_);

  if (v)
    *elem_ |= mask_;
  else
    *elem_ &= ~mask_;
  return *this;
}


} /* namespace std::impl */
_namespace_end(std)

#endif /* _STDIMPL_SIMPLE_BITSET_H_ */
