#ifndef _MUTEX_INL_H_
#define _MUTEX_INL_H_

#include <atomic>
#include <utility>
#include <abi/ext/atomic.h>
#include <abi/linker.h>
#include <chrono>
#include <system_error>

_namespace_begin(std)


inline auto mutex::native_handle() -> native_handle_type {
  return impl_;
}


inline auto recursive_mutex::native_handle() -> native_handle_type {
  return impl_;
}


template<typename Mutex>
lock_guard<Mutex>::lock_guard(mutex_type& m)
: m_(m)
{
  m_.lock();
}

template<typename Mutex>
lock_guard<Mutex>::lock_guard(mutex_type& m, adopt_lock_t)
: m_(m)
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

template<typename Mutex>
template<typename Clock, typename Duration>
auto unique_lock<Mutex>::try_lock_until(
    const chrono::time_point<Clock, Duration>& abs_time) -> bool {
  if (_predict_false(m_ == nullptr))
    throw system_error(make_error_code(errc::operation_not_permitted));
  if (_predict_false(locked_))
    throw system_error(make_error_code(errc::resource_deadlock_would_occur));
  return locked_ = m_->try_lock_until(abs_time);
}

template<typename Mutex>
template<typename Rep, typename Period>
auto unique_lock<Mutex>::try_lock_for(
    const chrono::duration<Rep, Period>& rel_time) -> bool {
  if (_predict_false(m_ == nullptr))
    throw system_error(make_error_code(errc::operation_not_permitted));
  if (_predict_false(locked_))
    throw system_error(make_error_code(errc::resource_deadlock_would_occur));
  return locked_ = m_->try_lock_for(rel_time);
}

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


template<typename Callable, typename... Args>
void call_once(once_flag& flag, Callable&& func, Args&&... args)
    noexcept(noexcept(func(forward<Args>(args)...))) {
  const volatile uint8_t* done =
      reinterpret_cast<const uint8_t*>(&flag.guard_);

  /* Special lock for flag. */
  class once_flag_lock {
   public:
    once_flag_lock() = delete;
    once_flag_lock(const once_flag_lock&) = delete;
    once_flag_lock& operator=(const once_flag_lock&) = delete;

    once_flag_lock(once_flag& flag) noexcept
    : flag_(flag),
      acquired_(abi::__cxa_guard_acquire(
          reinterpret_cast<abi::__cxa_guard*>(&flag.guard_)))
    {}

    ~once_flag_lock() noexcept {
      if (!acquired_) {
        /* SKIP */
      } else if (succes_) {
        __cxa_guard_release(
            reinterpret_cast<abi::__cxa_guard*>(&flag_.guard_));
      } else {
        __cxa_guard_abort(
            reinterpret_cast<abi::__cxa_guard*>(&flag_.guard_));
      }
    }

    void commit() noexcept {
      assert(acquired_);
      succes_ = true;
    }

    explicit operator bool() const noexcept {
      return acquired_;
    }

   private:
    once_flag& flag_;
    bool acquired_ = false;
    bool succes_ = false;
  };


  if (_predict_true(*done)) return;

  once_flag_lock lck{ flag };
  if (lck) {
    assert(!*done);
    func(forward<Args>(args)...);
    lck.commit();
  }
}


_namespace_end(std)

#endif /* _MUTEX_INL_H_ */
