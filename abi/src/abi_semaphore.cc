#include <abi/semaphore.h>
#include <atomic>

namespace __cxxabiv1 {


void semaphore::decrement() noexcept {
  uint32_t expect = 1;
  auto& atom = reinterpret_cast<std::atomic<decltype(v_)>&>(v_);

  static_assert(sizeof(atom) == sizeof(v_), "semaphore broken");

  for (;;) {
    if (atom.compare_exchange_weak(expect, expect - 1,
                                   std::memory_order_acquire,
                                   std::memory_order_relaxed))
      return;
    if (expect <= 0) expect = 1;
  }
}

void semaphore::increment() noexcept {
  auto& atom = reinterpret_cast<std::atomic<decltype(v_)>&>(v_);

  static_assert(sizeof(atom) == sizeof(v_), "semaphore broken");

  atom.fetch_add(1U, std::memory_order_release);
}


} /* namespace __cxxabiv1 */
