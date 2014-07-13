#ifndef _SEM_INLINE
# define _SEM_INLINE	extern inline
#endif

namespace __cxxabiv1 {

_SEM_INLINE void semaphore::decrement() noexcept {
  using abi::ext::pause;

  auto& t = reinterpret_cast<_namespace(std)::atomic<value_type>&>(t_);
  auto& a = reinterpret_cast<const _namespace(std)::atomic<value_type>&>(a_);
  static_assert(sizeof(t) == sizeof(t_), "semaphore broken");
  static_assert(sizeof(a) == sizeof(a_), "semaphore broken");

  const value_type ticket =
      t.fetch_add(1U, _namespace(std)::memory_order_acquire);
  while (a.load(_namespace(std)::memory_order_relaxed) - ticket > wnd) pause();
  _namespace(std)::atomic_thread_fence(_namespace(std)::memory_order_acquire);
}

_SEM_INLINE bool semaphore::try_decrement() noexcept {
  auto& t = reinterpret_cast<_namespace(std)::atomic<value_type>&>(t_);
  auto& a = reinterpret_cast<const _namespace(std)::atomic<value_type>&>(a_);
  const value_type a_ticket = a.load(_namespace(std)::memory_order_relaxed);
  value_type t_ticket = t.load(_namespace(std)::memory_order_relaxed);
  if (a_ticket - t_ticket > wnd) return false;
  return t.compare_exchange_strong(t_ticket, t_ticket + 1U,
                                   _namespace(std)::memory_order_acquire,
                                   _namespace(std)::memory_order_relaxed);
}

_SEM_INLINE void semaphore::increment() noexcept {
  auto& a = reinterpret_cast<_namespace(std)::atomic<value_type>&>(a_);
  a.fetch_add(1U, _namespace(std)::memory_order_release);
}


class semlock::scope_ {
 public:
  scope_() = delete;
  scope_(const scope_&) = delete;
  scope_& operator=(const scope_&) = delete;

  inline scope_(semlock&, bool) noexcept;
  inline ~scope_() noexcept;

 private:
  semlock& s_;
  bool l_;
};


inline semlock::scope_::scope_(semlock& s, bool l) noexcept
: s_(s),
  l_(l)
{
  if (l_)
    s_.lock();
  else
    s_.unlock();
}

inline semlock::scope_::~scope_() noexcept
{
  if (l_)
    s_.unlock();
  else
    s_.lock();
}


inline semlock::semlock(semlock&& o) noexcept
: sem_(o.sem_),
  locked_(o.locked_)
{
  o.locked_ = false;
}

inline semlock& semlock::operator=(semlock&& o) noexcept {
  if (locked_) unlock();
  sem_ = o.sem_;
  locked_ = o.locked_;
  o.locked_ = false;
  return *this;
}

inline semlock::semlock(semaphore& s) noexcept
: semlock(s, true)  // May block.
{}

inline semlock::semlock(semaphore& s, bool do_lock) noexcept
: sem_(&s)
{
  if (do_lock) lock();
}

inline semlock::~semlock() noexcept {
  if (locked_) unlock();
}

_SEM_INLINE void semlock::lock() noexcept {
  assert(!locked_);
  assert(sem_);

  sem_->decrement();
  locked_ = true;
}

_SEM_INLINE void semlock::unlock() noexcept {
  assert(locked_);
  assert(sem_);

  sem_->increment();
  locked_ = false;
}

inline semlock::operator bool() const noexcept {
  return locked_;
}

template<typename FN, typename... Args> auto semlock::do_locked(
    FN&& fn, Args&&... args)
    noexcept(noexcept(fn(_namespace(std)::forward<Args>(args)...))) ->
    decltype(fn(_namespace(std)::forward<Args>(args)...))
{
  assert(sem_);
  assert(locked_);

  scope_ s{ *this, true };
  return fn(_namespace(std)::forward<Args>(args)...);
}

template<typename FN, typename... Args> auto semlock::do_unlocked(
    FN&& fn, Args&&... args)
    noexcept(noexcept(fn(_namespace(std)::forward<Args>(args)...))) ->
    decltype(fn(_namespace(std)::forward<Args>(args)...))
{
  assert(sem_);
  assert(locked_);

  scope_ s{ *this, false };
  return fn(_namespace(std)::forward<Args>(args)...);
}

template<typename FN, typename... Args> auto semaphore::execute(
    FN&& fn, Args&&... args)
    noexcept(noexcept(fn(_namespace(std)::forward<Args>(args)...))) ->
    decltype(fn(_namespace(std)::forward<Args>(args)...))
{
  semlock lock{ *this };
  return fn(_namespace(std)::forward<Args>(args)...);
}

} /* namespace __cxxabiv1 */

#undef _SEM_INLINE
