#ifndef _ILIAS_ARCH_CPUID_H_
#define _ILIAS_ARCH_CPUID_H_

#include <cassert>
#include <string>
#include <tuple>

namespace ilias {

#if defined(__i386__) || defined(__amd64__) || defined(__x86_64__)
bool has_cpuid() noexcept;
#else
constexpr bool has_cpuid() noexcept { return false; }
#endif

#if defined(__i386__) || defined(__amd64__) || defined(__x86_64__)
inline auto cpuid(unsigned long function) noexcept ->
    std::tuple<unsigned long, unsigned long, unsigned long, unsigned long> {
  assert(has_cpuid());

  unsigned long a, b, c, d;
  asm volatile ("cpuid" : "=a"(a), "=b"(b), "=c"(c), "=d"(d) : "a"(function));
  return std::make_tuple(a, b, c, d);
}
#endif

/*
 * Returns the cpu vendor from the cpu.
 * If cpuid is not supported, returns an empty string.
 */
std::string cpu_vendor();

} /* namespace ilias */

#endif /* _ILIAS_ARCH_CPUID_H_ */
