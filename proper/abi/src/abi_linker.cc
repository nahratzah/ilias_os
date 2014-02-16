#include <abi/linker.h>
#include <atomic>
#include <cassert>
#include <cstring>

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

int __cxa_at_exit(void (*fn)(void*) noexcept, void* arg, void* dso_handle) noexcept {
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

} /* namespace __cxxabiv1 */
