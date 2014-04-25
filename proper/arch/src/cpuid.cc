#include <ilias/cpuid.h>
#include <string>
#include <tuple>

namespace ilias {

#if defined(__i386__) || defined(__amd64__) || defined(__x86_64__)
bool has_cpuid() noexcept {
  long supported;
  long clobber;

  asm volatile (
      "pushf\n"
      "pop %0\n"
      "mov %0, %1\n"
      "mov $0x200000, %0\n"
      "push %0\n"
      "popf\n"
      "pushf\n"
      "pop %0\n"
      "xor %1, %0\n"
      "push %1\n"
      "popf\n"
      : "=r"(supported), "=r"(clobber)
      :
      : "memory"
      );
  return (supported & 0x200000);
}
#endif

#if defined(__i386__) || defined(__amd64__) || defined(__x86_64__)
std::string cpu_vendor() {
  std::string out;
  if (!has_cpuid()) return out;

  unsigned long b, c, d;
  std::tie(std::ignore, b, c, d) = cpuid(0);
  out.resize(12);

  auto writer = out.begin();
  *writer++ = b & 0xff;
  *writer++ = (b >> 8) & 0xff;
  *writer++ = (b >> 16) & 0xff;
  *writer++ = (b >> 24) & 0xff;
  *writer++ = d & 0xff;
  *writer++ = (d >> 8) & 0xff;
  *writer++ = (d >> 16) & 0xff;
  *writer++ = (d >> 24) & 0xff;
  *writer++ = c & 0xff;
  *writer++ = (c >> 8) & 0xff;
  *writer++ = (c >> 16) & 0xff;
  *writer++ = (c >> 24) & 0xff;
  return out;
}
#else
std::string cpu_vendor() { return ""; }
#endif

std::string to_string(cpuid_feature_result features) {
  std::string out;

  for (const auto& f : cpuid_feature_const::all_features) {
    if (cpuid_feature_present(std::get<0>(f), features)) {
      if (!out.empty()) out += ", ";
      out += std::get<1>(f);
    }
  }

  return out;
}

} /* namespace ilias */
