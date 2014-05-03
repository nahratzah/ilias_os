#ifndef _MUTEX_INL_H_
#define _MUTEX_INL_H_

#include <atomic>
#include <utility>
#include <abi/ext/atomic.h>
#include <system_error>

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


template<typename Mutex>
lock_guard<Mutex>::lock_guard(mutex_type& m)
: m_(&m)
{
  m_.lock();
}

template<typename Mutex>
lock_guard<Mutex>::lock_guard(mutex_type& m, adopt_lock_t)
: m_(&m)
{}

template<typename Mutex>
lock_guard<Mutex>::~lock_guard() noexcept {
  m_.unlock();
}


template<typename Mutex>
unique_lock<Mutex>::unique_lock(mutex_type& m)
: m_(&m)
{
  m_->lock();
  locked_ = true;
}

template<typename Mutex>
unique_lock<Mutex>::unique_lock(mutex_type& m, defer_lock_t) noexcept
: m_(&m)
{}

template<typename Mutex>
unique_lock<Mutex>::unique_lock(mutex_type& m, try_to_lock_t)
: m_(&m),
  locked_(m_->try_lock())
{}

template<typename Mutex>
unique_lock<Mutex>::unique_lock(mutex_type& m, adopt_lock_t)
: m_(&m),
  locked_(true)
{}

#if 0 // XXX implement chrono for this
template<typename Mutex>
template<typename Clock, typename Duration>
unique_lock<Mutex>::unique_lock(
    mutex_type& m, const chrono::time_point<Clock, Duration>& abs_time)
: m_(&m),
  locked_(m_->try_lock_until(abs_time))
{}

template<typename Mutex>
template<typename Rep, typename Period>
unique_lock<Mutex>::unique_lock(mutex_type& m,
                                const chrono::duration<Rep, Period>& rel_time)
: m_(&m),
  locked_(m_->try_lock_for(rel_time))
{}
#endif

template<typename Mutex>
unique_lock<Mutex>::unique_lock(unique_lock&& o) noexcept
: m_(exchange(o.m_, nullptr)),
  locked_(exchange(o.locked_, false))
{}

template<typename Mutex>
auto unique_lock<Mutex>::operator=(unique_lock&& o) noexcept -> unique_lock& {
  unique_lock(move(o)).swap(*this);
}

template<typename Mutex>
unique_lock<Mutex>::~unique_lock() noexcept {
  if (locked_) m_->unlock();
}

template<typename Mutex>
auto unique_lock<Mutex>::lock() -> void {
  if (_predict_false(m_ == nullptr))
    throw system_error(make_error_code(errc::operation_not_permitted));
  if (_predict_false(locked_))
    throw system_error(make_error_code(errc::resource_deadlock_would_occur));
  m_->lock();
  locked_ = true;
}

template<typename Mutex>
auto unique_lock<Mutex>::try_lock() -> bool {
  if (_predict_false(m_ == nullptr))
    throw system_error(make_error_code(errc::operation_not_permitted));
  if (_predict_false(locked_))
    throw system_error(make_error_code(errc::resource_deadlock_would_occur));
  return locked_ = m_->try_lock();
}

#if 0 // XXX implement chrono for this
template<typename Mutex>
template<typename Clock, typename Duration>
auto unique_lock<Mutex>::try_lock_until(
    const chrono::time_point<Clock, Duration>& abs_time) noexcept -> bool {
  if (_predict_false(m_ == nullptr))
    throw system_error(make_error_code(errc::operation_not_permitted));
  if (_predict_false(locked_))
    throw system_error(make_error_code(errc::resource_deadlock_would_occur));
  return locked_ = m_->try_lock_until(abs_time);
}

template<typename Mutex>
template<typename Rep, typename Period>
auto unique_lock<Mutex>::try_lock_for(
    const chrono::duration<Rep, Period>& rel_time) noexcept -> bool {
  if (_predict_false(m_ == nullptr))
    throw system_error(make_error_code(errc::operation_not_permitted));
  if (_predict_false(locked_))
    throw system_error(make_error_code(errc::resource_deadlock_would_occur));
  return locked_ = m_->try_lock_for(rel_time);
}
#endif

template<typename Mutex>
auto unique_lock<Mutex>::unlock() -> void {
  if (_predict_false(!locked_))
    throw system_error(make_error_code(errc::operation_not_permitted));
  m_->unlock();
  locked_ = false;
}

template<typename Mutex>
auto unique_lock<Mutex>::swap(unique_lock& o) noexcept -> void {
  using _namespace(std)::swap;

  swap(m_, o.m_);
  swap(locked_, o.locked_);
}

template<typename Mutex>
auto unique_lock<Mutex>::release() noexcept -> mutex_type* {
  locked_ = false;
  return exchange(m_, nullptr);
}

template<typename Mutex>
auto unique_lock<Mutex>::owns_lock() const noexcept -> bool {
  return locked_;
}

template<typename Mutex>
unique_lock<Mutex>::operator bool() const noexcept {
  return locked_;
}

template<typename Mutex>
auto unique_lock<Mutex>::mutex() const noexcept -> mutex_type* {
  return m_;
}

template<typename Mutex>
void swap(unique_lock<Mutex>& a, unique_lock<Mutex>& b) noexcept {
  a.swap(b);
}


_namespace_end(std)

#endif /* _MUTEX_INL_H_ */
