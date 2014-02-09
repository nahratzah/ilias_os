#ifndef _ABI_HASHCODE_H_
#define _ABI_HASHCODE_H_

#include <abi/abi.h>

namespace __cxxabiv1 {


size_t hash_code(intmax_t) noexcept;
size_t hash_code(uintmax_t) noexcept;
size_t hash_code(const void*) noexcept;
size_t hash_code(const char*) noexcept;


} /* namespace __cxxabiv1 */

#endif /* _ABI_HASHCODE_H_ */
