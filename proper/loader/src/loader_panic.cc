#include <loader/panic.h>
#include <loader/x86_video.h>
#include <string>

namespace loader {

void panic(const char* fmt, ...) noexcept {
  va_list ap;

  va_start(ap, fmt);
  vpanic(std::string_ref(fmt), ap);
  va_end(ap);
}

void panic(std::string_ref fmt, ...) noexcept {
  va_list ap;

  va_start(ap, fmt);
  vpanic(fmt, ap);
  va_end(ap);
}

void vpanic(const char* fmt, va_list ap) noexcept {
  vpanic(std::string_ref(fmt), ap);
}

void vpanic(std::string_ref fmt, va_list ap) noexcept {
  bios_put_str("loader panic: ");
  bios_vprintf(fmt, ap);

  for (;;) {
#if defined(__i386__) || defined(__amd64__) || defined(__x86_64__)
    asm volatile ("hlt");
#endif
  }
}

} /* namespace loader */
