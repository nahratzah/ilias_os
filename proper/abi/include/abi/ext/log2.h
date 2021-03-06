#ifndef _ABI_EXT_LOG2_H_
#define _ABI_EXT_LOG2_H_

#include <abi/abi.h>
#include <climits>
#include <cstdint>

namespace __cxxabiv1 {
namespace ext {


/* Check if s is a power-of-2. */
constexpr bool is_pow2(uintmax_t);

/* Returns the largest number x, such that 1 << x <= s. */
constexpr unsigned int log2_down(uintmax_t);

/* Returns the smallest number x, such that 1 << x >= s. */
constexpr unsigned int log2_up(uintmax_t);


}} /* namespace __cxxabi::ext */

#include <abi/ext/log2-inl.h>

#endif /* _ABI_EXT_LOG2_H_ */
