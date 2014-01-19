#define _SHOW_UNWIND_INTERNAL
#include <abi/unwind.h>
#include <exception>
#include <utility>

namespace std {
namespace {

inline abi::__cxa_exception* exc_ptr(void* p) noexcept {
  if (p == nullptr) return nullptr;
  return reinterpret_cast<abi::__cxa_exception*>(p) - 1;
}

inline void* exc_acquire(void* p) noexcept {
  if (!p) return nullptr;
  abi::__cxa_exception::acquire(exc_ptr(p));
  return p;
}

inline void exc_release(void* p) noexcept {
  if (!p) return;
  abi::__cxa_exception::release(exc_ptr(p));
  return p;
}

exception_ptr::exception_ptr(const exception_ptr& other) noexcept
: ptr_(exc_acquire(other.ptr_))
{}

exception_ptr::exception_ptr(exception_ptr&& other) noexcept
: exception_ptr() {
  swap(ptr_, other.ptr_);
}

exception_ptr& exception_ptr::operator=(const exception_ptr& other) noexcept {
  exc_release(ptr_);
  ptr_ = exc_acquire(other.ptr_);
  return *this;
}

exception_ptr& exception_ptr::operator=(exception_ptr&& other) noexcept {
  exc_release(ptr_);
  ptr_ = other.ptr_;
  other.ptr_ = nullptr;
  return *this;
}

exception_ptr::~exception_ptr() noexcept {
  exc_release(ptr_);
}

void rethrow_exception(exception_ptr p) {
  /* XXX implement this */
  void* p = nullptr;
  swap(p, ptr_);
  __cxa_throw(p, ..., ...);
}


} /* namespace std */
