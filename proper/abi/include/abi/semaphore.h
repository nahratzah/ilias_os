#ifndef _ABI_SEMAPHORE_H_
#define _ABI_SEMAPHORE_H_

#include <abi/abi.h>
#include <abi/ext/atomic.h>
#include <atomic>
#include <cassert>
#include <limits>
#include <utility>

namespace __cxxabiv1 {


class semaphore {
 public:
  using value_type = uint32_t;

  constexpr semaphore() noexcept = default;
  constexpr semaphore(uint32_t v) noexcept : a_(v) {};

  void increment() noexcept;
  void decrement() noexcept;
  bool try_decrement() noexcept;

  template<typename FN, typename... Args> auto execute(FN&& fn, Args&&... args)
      noexcept(noexcept(fn(std::forward<Args>(args)...))) ->
      decltype(fn(std::forward<Args>(args)...));

 private:
  static constexpr uint32_t wnd = std::numeric_limits<uint32_t>::max() / 2U;
  value_type t_ = 0;
  value_type a_ = 0;
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

  void lock() noexcept;
  void unlock() noexcept;
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
