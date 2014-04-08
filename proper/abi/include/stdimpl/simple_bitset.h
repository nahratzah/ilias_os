#ifndef _STDIMPL_SIMPLE_BITSET_H_
#define _STDIMPL_SIMPLE_BITSET_H_

#include <cdecl.h>
#include <abi/abi.h>
#include <climits>
#include <cstddef>
#include <cstdint>

_namespace_begin(std)
namespace impl {


class simple_bitset {
 public:
  using size_type = size_t;
  class proxy;

  simple_bitset(size_type count);
  simple_bitset(const simple_bitset&) = delete;
  simple_bitset& operator=(const simple_bitset&) = delete;
  ~simple_bitset() noexcept;

  bool all_set() const noexcept;
  bool operator[](size_type idx) const noexcept;
  proxy operator[](size_type idx) noexcept;

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


} /* namespace std::impl */
_namespace_end(std)

#endif /* _STDIMPL_SIMPLE_BITSET_H_ */
