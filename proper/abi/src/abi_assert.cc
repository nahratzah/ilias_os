#include <abi/assert.h>
#include <abi/panic.h>


namespace __cxxabiv1 {


void _assert_fail(const char* file, const char* func, int line,
                  const char* pred, const char* opt_msg) noexcept {
  panic("Assertion %s FAILED at: %s:%d %s()%s%s",
        pred, file, line, func,
        (opt_msg ? "  --  " : ""),
        (opt_msg ? opt_msg : ""));
  for (;;);
}


} /* namespace __cxxabiv1 */
