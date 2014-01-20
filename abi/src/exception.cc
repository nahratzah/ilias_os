#define _SHOW_UNWIND_INTERNAL
#include <abi/abi.h>
#include <abi/eh.h>
#include <abi/panic.h>
#include <atomic>
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
  abi::__cxa_exception::acquire(*exc_ptr(p));
  return p;
}

inline void exc_release(void* p) noexcept {
  if (!p) return;
  abi::__cxa_exception::release(*exc_ptr(p));
}


} /* namespace std::<unnamed> */


exception::~exception() noexcept {}

const char* exception::what() const noexcept {
  return "std::exception";
}


bad_exception::~bad_exception() noexcept {}

const char* bad_exception::what() const noexcept {
  return "std::bad_exception";
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
  if (p.ptr_) ::abi::__cxa_rethrow_primary_exception(p.ptr_);
  terminate();
  for (;;);
}


nested_exception::nested_exception() noexcept
: nested_(current_exception())
{}

nested_exception::~nested_exception() noexcept {}

void nested_exception::rethrow_nested() const {
  rethrow_exception(nested_);
}


namespace {

void dfl_unexpected() noexcept {
  ::abi::panic("std::unexpected");
}

void dfl_terminate() noexcept {
  ::abi::panic("std::terminate");
}

std::atomic<unexpected_handler> unexpected_impl;  // Zero initialized
std::atomic<terminate_handler> terminate_impl;  // Zero initialized

} /* namespace std::<unnamed> */


unexpected_handler set_unexpected(unexpected_handler h) noexcept {
  unexpected_handler rv = unexpected_impl.exchange(h,
                                                   std::memory_order_acq_rel);
  return (rv ? rv : &dfl_unexpected);
}

unexpected_handler get_unexpected() noexcept {
  unexpected_handler rv = unexpected_impl.load(std::memory_order_acquire);
  return (rv ? rv : &dfl_unexpected);
}

void unexpected() noexcept {
  unexpected_handler h = get_unexpected();
  (*h)();
  ::abi::panic("std::unexpected_handler %p returned", h);
  for (;;);
}


terminate_handler set_terminate(terminate_handler h) noexcept {
  terminate_handler rv = terminate_impl.exchange(h, std::memory_order_acq_rel);
  return (rv ? rv : &dfl_terminate);
}

terminate_handler get_terminate() noexcept {
  terminate_handler rv = terminate_impl.load(std::memory_order_acquire);
  return (rv ? rv : &dfl_terminate);
}

void terminate() noexcept {
  terminate_handler h = get_terminate();
  (*h)();
  ::abi::panic("std::terminate_handler %p returned", h);
  for (;;);
}

bool uncaught_exception() noexcept {
  return ::abi::__cxa_uncaught_exception();
}


} /* namespace std */
