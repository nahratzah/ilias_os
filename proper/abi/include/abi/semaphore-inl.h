namespace __cxxabiv1 {

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


semlock::scope_::scope_(semlock& s, bool l) noexcept
: s_(s),
  l_(l)
{
  if (l_)
    s_.lock();
  else
    s_.unlock();
}

semlock::scope_::~scope_() noexcept
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

inline void semlock::lock() noexcept {
  assert(!locked_);
  assert(sem_);

  sem_->decrement();
  locked_ = true;
}

inline void semlock::unlock() noexcept {
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
    noexcept(noexcept(fn(std::forward<Args>(args)...))) ->
    decltype(fn(std::forward<Args>(args)...))
{
  assert(sem_);
  assert(locked_);

  scope_ s{ *this, true };
  return fn(std::forward<Args>(args)...);
}

template<typename FN, typename... Args> auto semlock::do_unlocked(
    FN&& fn, Args&&... args)
    noexcept(noexcept(fn(std::forward<Args>(args)...))) ->
    decltype(fn(std::forward<Args>(args)...))
{
  assert(sem_);
  assert(locked_);

  scope_ s{ *this, false };
  return fn(std::forward<Args>(args)...);
}

template<typename FN, typename... Args> auto semaphore::execute(
    FN&& fn, Args&&... args)
    noexcept(noexcept(fn(std::forward<Args>(args)...))) ->
    decltype(fn(std::forward<Args>(args)...))
{
  semlock lock{ *this };
  return fn(std::forward<Args>(args)...);
}

}
