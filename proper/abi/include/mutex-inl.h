#ifndef _MUTEX_INL_H_
#define _MUTEX_INL_H_

#include <atomic>
#include <utility>
#include <abi/ext/atomic.h>

_namespace_begin(std)


inline auto mutex::native_handle() -> native_handle_type {
  return impl_;
}


template<typename Callable, typename... Args>
void call_once(once_flag& flag, Callable&& func, Args&&... args)
    noexcept(noexcept(func(forward<Args>(args)...))) {
  /* Special lock for flag. */
  class once_flag_lock {
   public:
    once_flag_lock() = delete;
    once_flag_lock(const once_flag_lock&) = delete;
    once_flag_lock& operator=(const once_flag_lock&) = delete;

    once_flag_lock(once_flag& flag) noexcept
    : flag_(flag)
    {
      while (flag_.lock.exchange(true, memory_order_relaxed))
        abi::ext::pause();
      atomic_thread_fence(memory_order_acquire);
    }

    ~once_flag_lock() noexcept {
      atomic_thread_fence(memory_order_acq_rel);
      if (succes_) flag_.done = true;
      flag_.lock.exchange(false, memory_order_release);
    }

    void commit() noexcept {
      succes_ = true;
    }

   private:
    bool succes_ = false;
    once_flag& flag_;
  };


  if (_predict_true(flag.done)) return;

  once_flag_lock lck{ flag };
  if (!flag.done) {
    func(forward<Args>(args)...);
    lck.commit();
  }
}


_namespace_end(std)

#endif /* _MUTEX_INL_H_ */
