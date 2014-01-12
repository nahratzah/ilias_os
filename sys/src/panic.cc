#include <panic.h>
#include <cstdarg>

namespace kernel {


void panic(const char* msg, ...) noexcept {
  va_list ap;

  va_start(ap, msg);	/* XXX implement printf semantics */

#ifdef _LOADER
  bios_put_str(msg);
#else
  /* XXX implement something. */
#endif

  va_end(ap);

  asm volatile("hlt":::"memory");

  /* Spin here. */
  for (;;);
}

void warn(const char* msg, ...) noexcept {
  va_list ap;

  va_start(ap, msg);	/* XXX implement printf semantics */

#ifdef _LOADER
  bios_put_str(msg);
#else
  /* XXX implement something. */
#endif

  va_end(ap);
}


} /* namespace kernel */
