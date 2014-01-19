#define _SHOW_UNWIND_INTERNAL
#include <abi/eh.h>

namespace __cxxabiv1 {

/* Anonymous namespace, gathers all emergency exception allocation code. */
namespace {


typedef std::atomic<unsigned int> thr_emergency_use_t;
thread_local thr_emergency_use_t thr_emergency_use;

semaphore emergency_use_threads = 16;  // # threads that can use emergency buffers.

class emergency_slot {
 private:
  struct alignas(max_align_t) emergency_space {
    uint8_t data_[1024];  // Emergency block space.
  };

 public:
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

  std::memset(&space_, 0, sizeof(space_));
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
  if (sz > sizeof(emergency_slot::emergency_space)) return nullptr;

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


} /* namespace __cxxabiv1::<unnamed> */

/* Size of cxa exception. */
constexpr size_t header_sz = sizeof(__cxa_exception);

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
  void* storage = abi::malloc(sz);
  if (storage)
    std::memset(storage, 0, sz);
  else
    storage = acquire_emergency_space(sz);

  if (_predict_false(!storage)) {
    /*
     * This point is reached if the current thread is using too many exceptions
     * (max allowed by abi is 4) or if the emergency storage is exhausted.
     */
    std::terminate();
  }

  /* Return address where exception is to be created. */
  return static_cast<uint8_t*>(storage) + header_sz;
}

void __cxa_free_exception(void* exc_addr) noexcept {
  void* addr = reinterpret_cast<uint8_t*>(exc_addr) - header_sz;
  __cxa_exception* exc = reinterpret_cast<__cxa_exception*>(addr);

  /* Destroy exception object. */
  if (exc->exceptionDestructor) exc->exceptionDestructor(exc_addr);

  /* Release emergency resources. */
  if (_predict_true(!release_emergency_space(addr))) {
    /* Release heap resources. */
    abi::free(addr);
  }
}

void __cxa_throw(void* exc_addr, const std::type_info* ti,
                 void (*destructor)(void*)) noexcept {
  /*
   * Note: __cxa_exception of exc_addr was zeroed at allocation.
   * Thus no initializing to zero is required.
   */
  void* addr = reinterpret_cast<uint8_t*>(exc_addr) - header_sz;
  __cxa_exception* exc = reinterpret_cast<__cxa_exception*>(addr);

  exc->refcount = 1;

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
}


} /* namespace __cxxabiv1 */
