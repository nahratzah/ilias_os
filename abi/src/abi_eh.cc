#include <abi/eh.h>
#include <abi/memory.h>
#include <abi/panic.h>
#include <abi/semaphore.h>

namespace __cxxabiv1 {

/* Anonymous namespace, gathers all emergency exception allocation code. */
namespace {


typedef std::atomic<unsigned int> thr_emergency_use_t;
thread_local thr_emergency_use_t thr_emergency_use;

/* # threads that can use emergency buffers. */
semaphore emergency_use_threads{ 16U };

class emergency_slot {
 private:
  struct alignas(max_align_t) emergency_space {
    uint8_t data_[1024];  // Emergency block space.
  };

 public:
  static constexpr size_t space_size = sizeof(emergency_space);

  void* claim() noexcept;
  bool release(void*) noexcept;

 private:
  emergency_space space_;
  std::atomic<thr_emergency_use_t*> user_;
};

void* emergency_slot::claim() noexcept {
  thr_emergency_use_t* expect = nullptr;
  if (!user_.compare_exchange_strong(expect, &thr_emergency_use,
                                     std::memory_order_acquire,
                                     std::memory_order_relaxed))
    return nullptr;

  memzero(&space_, space_size);
  return &space_;
}

bool emergency_slot::release(void* p) noexcept {
  if (_predict_true(p != &space_)) return false;

  thr_emergency_use_t* user = user_.exchange(nullptr,
                                             std::memory_order_release);
  if (user->fetch_sub(1U, std::memory_order_release) == 1U)
    emergency_use_threads.increment();
  return true;
}


const unsigned int EMERGENCY_SZ = 64;
emergency_slot emergency[EMERGENCY_SZ];

/* Allocate space from emergency. */
void* acquire_emergency_space(size_t sz) noexcept {
  if (sz > emergency_slot::space_size) return nullptr;

  auto use = thr_emergency_use.fetch_add(1U, std::memory_order_acquire);
  if (use <= 4U) {
    if (use == 0U) emergency_use_threads.decrement();  // May block.
    for (emergency_slot* e = emergency; e != emergency + EMERGENCY_SZ; ++e) {
      void* addr = e->claim();
      if (addr) return addr;
    }
  }

  if (thr_emergency_use.fetch_sub(1U, std::memory_order_release) == 1U)
    emergency_use_threads.increment();
  return nullptr;
}

/* Try to release space to emergency. */
bool release_emergency_space(void* p) noexcept {
  for (emergency_slot* e = emergency; e != emergency + EMERGENCY_SZ; ++e)
    if (e->release(p)) return true;
  return false;
}

/* Size of cxa exception. */
constexpr size_t header_sz = sizeof(__cxa_exception);

/* Exception heap. */
abi::heap abi_eh_heap{ "abi/exception" };

inline __cxa_exception* exc2hdr(void* exc) noexcept {
  if (exc == nullptr) return nullptr;
  void* base = reinterpret_cast<uint8_t*>(exc) - header_sz;
  return reinterpret_cast<__cxa_exception*>(base);
}


} /* namespace __cxxabiv1::<unnamed> */


__cxa_eh_globals* __cxa_get_globals() noexcept {
  static thread_local __cxa_eh_globals impl{ nullptr, 0 };
  return &impl;
}

__cxa_eh_globals* __cxa_get_globals_fast() noexcept {
  return __cxa_get_globals();
}

void* __cxa_allocate_exception(size_t throw_sz) noexcept {
  const size_t sz = header_sz + throw_sz;

  /* Try allocating from heap. */
  __cxa_exception* storage =
    static_cast<__cxa_exception*>(abi_eh_heap.malloc(sz));
  if (_predict_true(storage))
    memzero(storage, sz);
  else
    storage = static_cast<__cxa_exception*>(acquire_emergency_space(sz));

  if (_predict_false(!storage)) {
    /*
     * This point is reached if the current thread is using too many exceptions
     * (max allowed by abi is 4) or if the emergency storage is exhausted.
     */
    std::terminate();
  }

  /* Return address where exception is to be created. */
  return storage + 1;
}

void __cxa_free_exception(void* exc_addr) noexcept {
  __cxa_exception* next = nullptr;
  __cxa_exception* exc = exc2hdr(exc_addr);
  do {
    if (exc->refcount.load(std::memory_order_acquire) != 0U) {
      panic("%s: %s", __func__, "exception reference count != 0");
      for (;;);
    }

    /*
     * Release next exception in the chain;
     * if the exception has to be destroyed as well,
     * keep it in next.
     */
    next = exc->nextException;
    exc->nextException = nullptr;
    if (next && !__cxa_exception::release(*next)) next = nullptr;

    /* Destroy exception object. */
    if (exc->exceptionDestructor) exc->exceptionDestructor(exc_addr);

    /* Release emergency resources. */
    if (_predict_true(!release_emergency_space(exc))) {
      /* Release heap resources. */
      abi_eh_heap.free(exc);
    }
  } while ((exc = next) != nullptr);
}

void __cxa_throw(void* exc_addr, const std::type_info* ti,
                 void (*destructor)(void*)) noexcept {
  /*
   * Note: __cxa_exception of exc_addr was zeroed at allocation.
   * Thus no initializing to zero is required.
   */
  __cxa_exception* exc = exc2hdr(exc_addr);

  __cxa_exception::acquire(*exc);

  exc->exceptionType = ti;
  exc->exceptionDestructor = destructor;
  exc->unexpectedHandler = std::get_unexpected();
  std::terminate_handler terminate = exc->terminateHandler =
                                     std::get_terminate();

  exc->unwindHeader.exception_class = _Unwind_Exception::GNU_CXX();

  /* Increment # uncaught exceptions. */
  __cxa_get_globals()->uncaughtExceptions++;

  _Unwind_Reason_Code fail = _Unwind_RaiseException(&exc->unwindHeader);

  /* If _Unwind_RaiseException returns, there are serious problems. */
  (*terminate)();  // XXX report fail
  for (;;);
}

bool __cxa_uncaught_exception() noexcept {
  return __cxa_get_globals()->uncaughtExceptions != 0;
}

void __cxa_rethrow_primary_exception(void* exc_addr) {
  if (_predict_false(!exc_addr)) {
    std::terminate();
    for (;;);
  }
  /* Resolve primary exception. */
  __cxa_exception* exc = exc2hdr(exc_addr);
  while (exc->nextException) exc = exc->nextException;

  __cxa_exception* rv = exc2hdr(__cxa_allocate_exception(0));
  __cxa_exception::acquire(*rv);

  std::terminate_handler terminate = rv->terminateHandler =
                                     std::get_terminate();
  rv->unexpectedHandler = std::get_unexpected();
  rv->nextException = exc;
  __cxa_exception::acquire(*exc);  // Because of rv->nextException above.

  rv->unwindHeader.exception_class = _Unwind_Exception::GNU_CXX_dependant();

  /* Increment # uncaught exceptions. */
  __cxa_get_globals()->uncaughtExceptions++;

  _Unwind_Reason_Code fail = _Unwind_RaiseException(&rv->unwindHeader);

  /* If _Unwind_RaiseException returns, there are serious problems. */
  (*terminate)();  // XXX report fail
  for (;;);
}

void* __cxa_current_primary_exception() noexcept {
  __cxa_exception* exc = __cxa_get_globals()->caughtExceptions;
  if (!exc) return nullptr;
  while (exc->nextException) exc = exc->nextException;
  return exc + 1;
}

void __cxa_bad_cast() {
  throw std::bad_cast();
}

void __cxa_bad_typeid() {
  throw std::bad_typeid();
}


} /* namespace __cxxabiv1 */
