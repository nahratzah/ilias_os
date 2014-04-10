#ifndef _ABI_ASSERT_H_
#define _ABI_ASSERT_H_

#include <cdecl.h>
#include <abi/abi.h>

namespace __cxxabiv1 {

_cdecl_begin

void _assert_fail(const char*, const char*, int, const char*,
                  const char*) noexcept __attribute__((__noreturn__));

inline void _assert_test(bool, const char*, const char*, int,
                         const char*, const char*) noexcept;

_cdecl_end

} /* namespace __cxxabiv1 */

using abi::_assert_test;

#include <abi/assert-inl.h>

#endif /* _ABI_ASSERT_H_ */
