#ifndef _KERNEL_H_
#define _KERNEL_H_

#include <cstdint>

namespace loader {

class kernel {
 public:
  constexpr kernel() noexcept
  : start{ nullptr },
    len{ 0 }
  {}

  constexpr kernel(const void *start, std::uint32_t len) noexcept
  : start{ start },
    len{ len }
  {}

  kernel(const void* start, std::uint32_t* len) noexcept
  : kernel(len != nullptr ? start : nullptr,
           len != nullptr ? *len : 0U)
  {}

  bool present() const noexcept { return start != nullptr && len > 0; }
  explicit operator bool() const noexcept { return present(); }

  static const kernel i386;
  static const kernel amd64;

 private:
  const void* start;
  std::uint32_t len;
};

} /* namespace loader */

#endif /* _KERNEL_H_ */
