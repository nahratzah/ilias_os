#ifndef _ABI_SEMAPHORE_H_
#define _ABI_SEMAPHORE_H_

#include <abi/abi.h>

namespace __cxxabiv1 {


class semaphore {
 public:
  constexpr semaphore() = default;
  constexpr semaphore(uint32_t v) : v_(v) {};

  void increment() noexcept;
  void decrement() noexcept;

 private:
  uint32_t v_ = 0;
};


} /* namespace __cxxabiv1 */

#endif /* _ABI_SEMAPHORE_H_ */
