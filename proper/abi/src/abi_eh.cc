#include <abi/eh.h>
#include <abi/memory.h>
#include <abi/panic.h>
#include <abi/semaphore.h>
#include <array>
#include <thread>
#include <type_traits>

namespace __cxxabiv1 {

/* Anonymous namespace, gathers all emergency exception allocation code. */
namespace {


using thr_emergency_use_t = std::atomic<unsigned int>;
thread_local thr_emergency_use_t thr_emergency_use;

/* # threads that can use emergency buffers. */
semaphore emergency_use_threads{ 16U };

class emergency_slot {
 private:
  using emergency_space =
      _namespace(std)::aligned_storage_t<1024, alignof(max_align_t)>;

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
_namespace(std)::array<emergency_slot, EMERGENCY_SZ> emergency;

/* Allocate space from emergency. */
void* acquire_emergency_space(size_t sz) noexcept {
  if (sz > emergency_slot::space_size) return nullptr;

  auto use = thr_emergency_use.fetch_add(1U, std::memory_order_acquire);
  if (use <= 4U) {
    if (use == 0U) emergency_use_threads.decrement();  // May block.
    for (auto& e : emergency) {
      void* addr = e.claim();
      if (addr) return addr;
    }
  }

  if (thr_emergency_use.fetch_sub(1U, std::memory_order_release) == 1U)
    emergency_use_threads.increment();
  return nullptr;
}

/* Try to release space to emergency. */
bool release_emergency_space(void* p) noexcept {
  for (auto& e : emergency)
    if (e.release(p)) return true;
  return false;
}

/* Size of cxa exception. */
constexpr size_t header_sz = sizeof(__cxa_exception);

/* Exception heap. */
abi::heap& abi_eh_heap() noexcept {
  static _namespace(std)::once_flag once;
  static _namespace(std)::aligned_storage_t<sizeof(abi::heap),
                                            alignof(abi::heap)> store;

  void* p = &store;
  call_once(once,
            [](void* p) {
              new (p) abi::heap{ "abi/exception" };
            },
            p);
  return *static_cast<abi::heap*>(p);
}

inline __cxa_exception* exc2hdr(void* exc) noexcept {
  if (exc == nullptr) return nullptr;
  void* base = reinterpret_cast<uint8_t*>(exc) - header_sz;
  return reinterpret_cast<__cxa_exception*>(base);
}

thread_local _namespace(std)::aligned_storage_t<sizeof(__cxa_eh_globals),
                                                alignof(__cxa_eh_globals)>
    cxa_eh_globals;


} /* namespace __cxxabiv1::<unnamed> */


__cxa_eh_globals* __cxa_get_globals() noexcept {
  static thread_local _namespace(std)::once_flag cxa_eh_globals_once;

  void* p = &cxa_eh_globals;
  call_once(cxa_eh_globals_once,
      [](void* p) {
        new (p) __cxa_eh_globals{ nullptr, 0 };
      },
      p);
  return static_cast<__cxa_eh_globals*>(p);
}

__cxa_eh_globals* __cxa_get_globals_fast() noexcept {
  void* p = &cxa_eh_globals;
  return static_cast<__cxa_eh_globals*>(p);
}

void* __cxa_allocate_exception(size_t throw_sz) noexcept {
  const size_t sz = header_sz + throw_sz;

  /* Try allocating from heap. */
  __cxa_exception* storage =
    static_cast<__cxa_exception*>(abi_eh_heap().malloc(sz));
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
      abi_eh_heap().free(exc);
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
  switch (fail) {
  case _URC_NO_REASON:
    panic("__cxa_throw: no reason (for Unwind_RaiseException to return)");
    break;
  case _URC_FOREIGN_EXCEPTION_CAUGHT:
    panic("__cxa_throw: foreign exception caught");
    break;
  case _URC_FATAL_PHASE_2_ERROR:
    panic("__cxa_throw: fatal phase 2 error");
    break;
  case _URC_FATAL_PHASE_1_ERROR:
    panic("__cxa_throw: fatal phase 1 error");
    break;
  case _URC_NORMAL_STOP:
    panic("__cxa_throw: normal stop");
    break;
  case _URC_END_OF_STACK:
    panic("__cxa_throw: end of stack");
    break;
  case _URC_HANDLER_FOUND:
    panic("__cxa_throw: handler found");
    break;
  case _URC_INSTALL_CONTEXT:
    panic("__cxa_throw: install context");
    break;
  case _URC_CONTINUE_UNWIND:
    panic("__cxa_throw: continue unwind");
    break;
  }

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
  switch (fail) {
  case _URC_NO_REASON:
    panic("__cxa_throw: no reason (for Unwind_RaiseException to return)");
    break;
  case _URC_FOREIGN_EXCEPTION_CAUGHT:
    panic("__cxa_throw: foreign exception caught");
    break;
  case _URC_FATAL_PHASE_2_ERROR:
    panic("__cxa_throw: fatal phase 2 error");
    break;
  case _URC_FATAL_PHASE_1_ERROR:
    panic("__cxa_throw: fatal phase 1 error");
    break;
  case _URC_NORMAL_STOP:
    panic("__cxa_throw: normal stop");
    break;
  case _URC_END_OF_STACK:
    panic("__cxa_throw: end of stack");
    break;
  case _URC_HANDLER_FOUND:
    panic("__cxa_throw: handler found");
    break;
  case _URC_INSTALL_CONTEXT:
    panic("__cxa_throw: install context");
    break;
  case _URC_CONTINUE_UNWIND:
    panic("__cxa_throw: continue unwind");
    break;
  }

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

_Unwind_Reason_Code __gxx_personality_v0(int version, _Unwind_Action actions,
                                         uint64_t exceptionClass,
                                         _Unwind_Exception* exceptionObject,
                                         _Unwind_Context* context) noexcept {
  using ull = unsigned long long;
  panic("TODO: implement %s(%i, %i, %#llu, %p, %p)", __func__,
        version, int(actions), ull(exceptionClass),
        exceptionObject, context);  // XXX implement
  for (;;);
}

void _Unwind_Resume(_Unwind_Exception *exception_object) noexcept {
  panic("TODO: implement %s(%p)", __func__, exception_object);  // XXX implement
  for (;;);
}

void* __cxa_begin_catch(void* exc_addr) noexcept {
  panic("TODO: implement %s(%p)", __func__, exc_addr);  // XXX implement
  for (;;);

  __cxa_exception* exc = exc2hdr(exc_addr);
  ++exc->handlerCount;
  --__cxa_get_globals()->uncaughtExceptions;
}

void* __cxa_end_catch(void* exc_addr) noexcept {
  panic("TODO: implement %s()", __func__);  // XXX implement
  for (;;);

  __cxa_exception* exc = exc2hdr(exc_addr);
  --exc->handlerCount;
}

void __cxa_rethrow() noexcept {
  panic("TODO: implement %s()", __func__);  // XXX implement
  for (;;);
}

_Unwind_Reason_Code _Unwind_RaiseException(struct _Unwind_Exception *exception_object) noexcept {
  panic("TODO: implement %s(%p)", __func__, exception_object);  // XXX implement
  for (;;);
}


} /* namespace __cxxabiv1 */
