#ifndef _ABI_EXT_LOG10_H_
#define _ABI_EXT_LOG10_H_

#include <abi/abi.h>

namespace __cxxabiv1 {
namespace ext {


template<typename T> constexpr unsigned int log10(T);
template<typename T> constexpr unsigned int log10_up(T);


}} /* namespace __cxxabiv1::ext */

#include <abi/ext/log10-inl.h>

#endif /* _ABI_EXT_LOG10_H_ */
