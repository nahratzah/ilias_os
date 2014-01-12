#include <cxxrt.h>
#include <atomic>
#include <cassert>
#include <cstring>

namespace kernel {
namespace {


struct rt_dtor {
  void (*fn)(void*) noexcept;
  void* arg;
  void* dso_handle;
};

const unsigned int RTDT_LEN = 256;
typedef rt_dtor rt_dtor_array[RTDT_LEN];
rt_dtor_array rtdt;
unsigned int rtdt_len = 0;

std::atomic<bool> rtdt_spl;

class rtdt_lock {
 public:
  explicit inline rtdt_lock(bool = true) noexcept;
  inline ~rtdt_lock() noexcept;

  inline void unlock() noexcept;
  inline void lock() noexcept;

 private:
  bool locked_ = false;
};

inline rtdt_lock::rtdt_lock(bool locked) noexcept {
  if (locked) lock();
}

inline rtdt_lock::~rtdt_lock() noexcept {
  if (locked_) unlock();
}

inline void rtdt_lock::unlock() noexcept {
  assert(locked_);
  rtdt_spl.store(false, std::memory_order_release);
  locked_ = false;
}

inline void rtdt_lock::lock() noexcept {
  assert(!locked_);
  while (rtdt_spl.exchange(true, std::memory_order_acquire));
  locked_ = true;
}


} /* namespace kernel::<unnamed> */

void *__dso_handle = &__dso_handle;

int __cxa_at_exit(void (*fn)(void*) noexcept, void* arg, void* dso_handle) noexcept {
  rt_dtor v{ fn, arg, dso_handle };
  if (!fn) return 1;

  rtdt_lock lck;
  if (rtdt_len == RTDT_LEN)
    return 1;
  rtdt[rtdt_len++] = v;
  return 0;
}

void __cxa_finalize(void* fn) noexcept {
  if (fn == nullptr) {
    __cxa_finalize_0();
    return;
  }

  rt_dtor_array todo;
  unsigned int todo_len = 0;

  /* Figure out which items need to be run. */
  {
    rtdt_lock lck;

    for (unsigned int i = rtdt_len; i; --i) {
      const unsigned int idx = i - 1;
      auto& elem = rtdt[i - 1];

      if (fn == elem.fn) {
        /* Mark function as to-be-executed. */
        todo[todo_len++] = elem;

        /* Move each element after elem back. */
        --rtdt_len;
        for (unsigned int j = idx; j < rtdt_len; ++j)
          rtdt[j] = rtdt[j + 1];
      }
    }
  }

  /* Execute all todo-queued items. */
  for (rt_dtor* elem = &todo[0]; elem != &todo[todo_len]; ++elem)
    (*elem->fn)(elem->arg);
}

void __cxa_finalize_0() noexcept {
  /* Mark all items as todo. */
  rtdt_lock lck;

  rt_dtor_array todo;
  std::memcpy(todo, rtdt, sizeof(todo));
  unsigned int todo_len = rtdt_len;
  rtdt_len = 0;
  lck.unlock();

  /* Execute in todo (in reverse order). */
  while (todo_len-- > 0) (*todo[todo_len].fn)(todo[todo_len].arg);
}

} /* namespace kernel */
