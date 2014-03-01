#include <abi/linker.h>
#include <abi/panic.h>
#include <abi/ext/atomic.h>
#include <atomic>
#include <cassert>
#include <cstring>
#include <new>

namespace __cxxabiv1 {
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

semaphore rtdt_spl{ 1U };


} /* namespace kernel::<unnamed> */

void *__dso_handle = &__dso_handle;

int __cxa_atexit(void (*fn)(void*) noexcept, void* arg, void* dso_handle)
    noexcept {
  rt_dtor v{ fn, arg, dso_handle };
  if (!fn) return 1;

  semlock lck{ rtdt_spl };
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
    semlock lck{ rtdt_spl };

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
  semlock lck{ rtdt_spl };

  rt_dtor_array todo;
  std::memcpy(todo, rtdt, sizeof(todo));
  unsigned int todo_len = rtdt_len;
  rtdt_len = 0;
  lck.unlock();

  /* Execute in todo (in reverse order). */
  while (todo_len-- > 0) (*todo[todo_len].fn)(todo[todo_len].arg);
}

void __cxa_finalize_dso(const void* handle) noexcept {
  if (handle == nullptr) return;

  rt_dtor_array todo;
  unsigned int todo_len = 0;

  /* Figure out which items need to be run. */
  {
    semlock lck{ rtdt_spl };

    for (unsigned int i = rtdt_len; i; --i) {
      const unsigned int idx = i - 1;
      auto& elem = rtdt[i - 1];

      if (handle == elem.dso_handle) {
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

void __cxa_pure_virtual() noexcept {
  panic("pure virtual function called");
  for (;;);
}

void __cxa_deleted_virtual() noexcept {
  panic("deleted virtual function called");
  for (;;);
}


struct __cxa_guard {
  struct impl_t {
    uint8_t mark;  // Must by byte 0.
    std::atomic<uint8_t> mtx;
  };

  union data_t {
    uint64_t _unused;
    impl_t impl;
  };

  data_t data;
};

static_assert(offsetof(__cxa_guard, data.impl.mark) == 0,
              "mark byte must be first byte in __cxa_guard");
static_assert(sizeof(__cxa_guard) == 8,
              "__cxa_guard is 8 bytes (source: itanium abi)");

namespace {

uint8_t* cxa_guard_mark_byte(__cxa_guard* g_) noexcept {
  return &g_->data.impl.mark;
}

std::atomic<uint8_t>& cxa_guard_mutex(__cxa_guard* g_) noexcept {
  return g_->data.impl.mtx;
}

} /* namespace __cxxabiv1::<unnamed> */

int __cxa_guard_acquire(__cxa_guard* g_) noexcept {
  auto& lock = cxa_guard_mutex(g_);

  uint8_t lock_zero = 0;
  while (!lock.compare_exchange_weak(lock_zero, 1,
                                     std::memory_order_acquire,
                                     std::memory_order_relaxed)) {
    lock_zero = 0;
    abi::ext::pause();
  }

  auto rv = cxa_guard_mark_byte(g_);
  if (rv) lock.store(0, std::memory_order_relaxed);
  return !rv;
}

void __cxa_guard_release(__cxa_guard* g_) noexcept {
  *cxa_guard_mark_byte(g_) = 1;
  cxa_guard_mutex(g_).store(0, std::memory_order_release);
}

void __cxa_guard_abort(__cxa_guard* g_) noexcept {
  cxa_guard_mutex(g_).store(0, std::memory_order_release);
}

void __cxa_throw_bad_array_new_length() {
  throw std::bad_array_new_length();
}

} /* namespace __cxxabiv1 */
