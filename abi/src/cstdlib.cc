#include <cstdlib>
#include <abi/abi.h>
#include <abi/semaphore.h>
#include <abi/memory.h>
#include <abi/panic.h>
#include <abi/errno.h>
#include <new>

namespace std {

/* at{_quick_}exit function implementation. */
namespace {


constexpr _TYPES(size_t) fn_count = 256;

class fn_type {
 public:
  fn_type() = default;
  fn_type(const fn_type&) = default;
  fn_type& operator=(const fn_type&) = default;
  constexpr fn_type(void (*)(), bool);

  bool resolve(bool) noexcept;
  bool empty() const noexcept;

 private:
  void (*fn_)() = nullptr;
  bool is_quick_ = false;
};

fn_type fn_array[fn_count];

class fn_lock;

class fn_unlock {
 public:
  inline fn_unlock(fn_lock&) noexcept;
  fn_unlock(const fn_unlock&) = delete;
  fn_unlock& operator=(const fn_unlock&) = delete;
  inline ~fn_unlock() noexcept;

 private:
  fn_lock& lck_;
};

class fn_lock {
  friend fn_unlock;

 public:
  inline fn_lock(abi::semaphore& = fn_lck_) noexcept;
  fn_lock(const fn_lock&) = delete;
  fn_lock& operator=(const fn_lock&) = delete;
  inline ~fn_lock() noexcept;

 private:
  static abi::semaphore fn_lck_;
  abi::semaphore& s_;
};

class fn_stack {
 public:
  static bool push(fn_type) noexcept;
  static inline bool empty() noexcept;
  static fn_type pop(bool* = nullptr) noexcept;

 private:
  explicit inline fn_stack(fn_type) noexcept;

  fn_type fn_;
  fn_stack* tail_ = nullptr;
  static fn_stack* head_;
  static abi::heap heap;
};


constexpr fn_type::fn_type(void (*fn)(), bool is_quick)
: fn_(fn),
  is_quick_(is_quick)
{}

bool fn_type::resolve(bool quick_only) noexcept {
  if (!(fn_ && (is_quick_ == quick_only))) return false;
  (*fn_)();
  return true;
}

bool fn_type::empty() const noexcept {
  return !fn_;
}

inline fn_unlock::fn_unlock(fn_lock& lck) noexcept
: lck_(lck)
{
  lck_.s_.increment();
}

inline fn_unlock::~fn_unlock() noexcept {
  lck_.s_.decrement();
}

inline fn_lock::fn_lock(abi::semaphore& s) noexcept
: s_(s)
{
  s_.decrement();
}
inline fn_lock::~fn_lock() noexcept {
  s_.increment();
}

inline fn_stack::fn_stack(fn_type fn) noexcept
: fn_(fn)
{}

bool fn_stack::empty() noexcept {
  return head_ == nullptr;
}

bool fn_stack::push(fn_type fn) noexcept {
  fn_stack* e = static_cast<fn_stack*>(heap.malloc(sizeof(fn_stack)));
  if (!e) return false;
  new (e) fn_stack(fn);

  fn_lock();
  e->tail_ = head_;
  head_ = e;
  return true;
}

fn_type fn_stack::pop(bool* fail) noexcept {
  fn_stack* h = head_;
  if (h == nullptr) {
    if (fail) *fail = true;
    return fn_type();
  }

  head_ = h->tail_;
  fn_type rv = h->fn_;
  heap.free(h);
  if (fail) *fail = false;
  return rv;
}

int push(fn_type fn) noexcept {
  if (fn_stack::push(fn)) return 0;

  fn_lock lck;
  if (!fn_stack::empty()) return _ENOMEM;
  for (fn_type* f = &fn_array[0]; f != &fn_array[fn_count]; ++f) {
    if (f->empty()) {
      *f = fn;
      return 0;
    }
  }
  return _ENOMEM;
}

void resolve(bool quick_only) noexcept {
  static abi::semaphore highlander{ 1 };
  highlander.decrement();  // Only once ever will this be invoked.

  bool keep_going = true;
  fn_lock lck;
  while (keep_going) {
    keep_going = false;
    bool stop;
    do {
      fn_type fn = fn_stack::pop(&stop);
      if (!stop) {
        fn_unlock ulck{ lck };
        fn.resolve(quick_only);
      }
    } while (!stop);

    for (fn_type* f = &fn_array[fn_count - 1U]; f != &fn_array[0] - 1; --f) {
      fn_type fn = *f;
      *f = fn_type();
      fn_unlock ulck{ lck };
      if (fn.resolve(quick_only)) {
        keep_going = true;
	break;
      }
    }
  }
}


fn_stack* fn_stack::head_ = nullptr;
::abi::heap fn_stack::heap{ "abi/atexit" };
abi::semaphore fn_lock::fn_lck_{ 1U };


} /* namespace std::<unnamed> */


void abort() noexcept {
  ::abi::_config::abort();
  abi::panic("abi::_config::abort() returned");
  for (;;);
}

void quick_exit(int s) noexcept {
  resolve(true);
  abi::_config::exit(s);
  abi::panic("abi::_config::exit() returned");
  for (;;);
}

void exit(int s) noexcept {
  resolve(false);
  abi::_config::exit(s);
  abi::panic("abi::_config::exit() returned");
  for (;;);
}

int atexit(void (*fn)()) noexcept {
  if (fn == nullptr) return _EINVAL;
  return push(fn_type(fn, false));
}

int at_quick_exit(void (*fn)()) noexcept {
  if (fn == nullptr) return _EINVAL;
  return push(fn_type(fn, true));
}


} /* namespace std */
