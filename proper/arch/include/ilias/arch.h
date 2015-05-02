#ifndef _ILIAS_ARCH_H_
#define _ILIAS_ARCH_H_

#include <array>

namespace ilias {


enum class arch {
  i386,
  amd64
};

template<arch... Archs>
struct arch_set {
  using value_type = std::array<arch, sizeof...(Archs)>;
  static constexpr value_type value{{ Archs... }};
  using type = arch_set;

  constexpr operator value_type() const { return value; }
  constexpr value_type operator()() const { return value; }
};

#if defined(__i386__)
constexpr arch native_arch = arch::i386;
using native_varch = arch_set<arch::i386>;
#elif defined(__amd64__) || defined(__x86_64__)
constexpr arch native_arch = arch::amd64;
using native_varch = arch_set<arch::i386, arch::amd64>;
#else
# error "Architecture not recognized."
#endif


} /* namespace ilias */

#endif /* _ILIAS_ARCH_H_ */
