#ifndef _ILIAS_ARCH_H_
#define _ILIAS_ARCH_H_

namespace ilias {

enum class arch {
  i386,
  amd64
};

#if defined(__i386__)
constexpr arch native_arch = arch::i386;
#elif defined(__amd64__) || defined(__x86_64__)
constexpr arch native_arch = arch::amd64;
#else
# error "Architecture not recognized."
#endif

} /* namespace ilias */

#endif /* _ILIAS_ARCH_H_ */
