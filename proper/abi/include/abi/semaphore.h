#ifndef _ABI_SEMAPHORE_H_
#define _ABI_SEMAPHORE_H_

#include <abi/abi.h>
#include <utility>
#include <cassert>

namespace __cxxabiv1 {


class semaphore {
 public:
  constexpr semaphore() = default;
  constexpr semaphore(uint32_t v) : v_(v) {};

  void increment() noexcept;
  void decrement() noexcept;

  template<typename FN, typename... Args> auto execute(FN&& fn, Args&&... args)
      noexcept(noexcept(fn(std::forward<Args>(args)...))) ->
      decltype(fn(std::forward<Args>(args)...));

 private:
  uint32_t v_ = 0;
};

class semlock {
 public:
  semlock() = default;
  semlock(const semlock&) = delete;
  semlock& operator=(const semlock&) = delete;
  inline semlock(semlock&&) noexcept;
  inline semlock& operator=(semlock&&) noexcept;
  explicit inline semlock(semaphore&) noexcept;
  inline semlock(semaphore&, bool) noexcept;
  inline ~semlock() noexcept;

  inline void lock() noexcept;
  inline void unlock() noexcept;
  explicit inline operator bool() const noexcept;
  template<typename FN, typename... Args> auto do_locked(FN&& fn,
                                                         Args&&... args)
      noexcept(noexcept(fn(std::forward<Args>(args)...))) ->
      decltype(fn(std::forward<Args>(args)...));
  template<typename FN, typename... Args> auto do_unlocked(FN&& fn,
                                                           Args&&... args)
      noexcept(noexcept(fn(std::forward<Args>(args)...))) ->
      decltype(fn(std::forward<Args>(args)...));

 private:
  class scope_;

  semaphore* sem_ = nullptr;
  bool locked_ = false;
};


} /* namespace __cxxabiv1 */

#include <abi/semaphore-inl.h>

#endif /* _ABI_SEMAPHORE_H_ */
