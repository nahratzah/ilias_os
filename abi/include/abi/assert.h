#ifndef _ABI_ASSERT_H_
#define _ABI_ASSERT_H_

#include <cdecl.h>
#include <abi/abi.h>

namespace __cxxabiv1 {

_cdecl_begin

void _assert_fail(const char*, const char*, int, const char*,
                  const char*) __attribute__((__noreturn__));

inline void _assert_test(bool p, const char* file, const char* func, int line,
                         const char* pred, const char* opt_msg) noexcept {
  if (_predict_false(!p)) _assert_fail(file, func, line, pred, opt_msg);
}

_cdecl_end

} /* namespace __cxxabiv1 */

using abi::_assert_test;

#endif /* _ABI_ASSERT_H_ */
