#include <abi/semaphore.h>
#include <atomic>
#include <limits>

namespace __cxxabiv1 {
namespace {

template<typename T> std::atomic<T>& as_atom(T& v) noexcept {
  return reinterpret_cast<std::atomic<T>&>(v);
}

template<typename T> const std::atomic<T>& as_atom(const T& v) noexcept {
  return reinterpret_cast<const std::atomic<T>&>(v);
}

template<typename T> const std::atomic<T>& as_catom(const T& v) noexcept {
  return reinterpret_cast<const std::atomic<T>&>(v);
}

inline void pause() noexcept {
#if defined(__i386__) || defined(__x86_64__) || defined(__amd64__)
  asm volatile("pause");
#endif
}

} /* namespace __cxxabiv1::<unnamed> */


void semaphore::decrement() noexcept {
  using type = decltype(t_);

  auto& t = as_atom(t_);
  auto& a = as_catom(a_);
  static_assert(sizeof(t) == sizeof(t_), "semaphore broken");
  static_assert(sizeof(a) == sizeof(a_), "semaphore broken");

  const type ticket = t.fetch_add(1U, std::memory_order_acquire);
  while (a.load(std::memory_order_relaxed) - ticket >
         std::numeric_limits<type>::max() / 2U) {
    pause();
  }
  std::atomic_thread_fence(std::memory_order_acquire);
}

void semaphore::increment() noexcept {
  as_atom(a_).fetch_add(1U, std::memory_order_release);
}

bool semaphore::try_decrement() noexcept {
  using type = decltype(t_);

  auto& t = as_atom(t_);
  auto& a = as_catom(a_);
  const type a_ticket = a.load(std::memory_order_relaxed);
  type t_ticket = t.load(std::memory_order_relaxed);
  if (a_ticket - t_ticket > std::numeric_limits<type>::max() / 2U)
    return false;
  return t.compare_exchange_strong(t_ticket, t_ticket + 1U,
                                   std::memory_order_acquire,
                                   std::memory_order_relaxed);
}


} /* namespace __cxxabiv1 */
