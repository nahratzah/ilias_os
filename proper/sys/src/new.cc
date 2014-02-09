#include <new>
#include <utility>
#include <cstdlib>
#include <cdecl.h>
#include <atomic>


namespace std {


bad_alloc::bad_alloc() noexcept {}
bad_alloc::bad_alloc(const bad_alloc& o) noexcept : exception(o) {}
bad_alloc& bad_alloc::operator=(const bad_alloc& o) noexcept {
  this->std::exception::operator=(o);
  return *this;
}
bad_alloc::~bad_alloc() noexcept {}

const nothrow_t nothrow = {};

namespace {
std::atomic<new_handler> nh_{ nullptr };
}

new_handler set_new_handler(new_handler nh) noexcept {
  return nh_.exchange(nh, std::memory_order_relaxed);
}
new_handler get_new_handler() noexcept {
  return nh_.load(std::memory_order_relaxed);
}


} /* namespace std */


void* operator new(std::size_t sz) {
  void* rv = operator new(sz, std::nothrow);
  if (_predict_false(!rv))
    __throw_bad_alloc();
  return rv;
}

void* operator new(std::size_t sz, const std::nothrow_t&) noexcept {
  void* rv = std::malloc(sz);

  /* Try to run new_handler on allocation failure. */
  if (_predict_false(rv == nullptr)) {
    std::new_handler nh = std::get_new_handler();
    if (nh) {
      nh();
      rv = std::malloc(sz);
    }
  }

  return rv;
}

void operator delete(void* p) noexcept {
  operator delete(p, std::nothrow);
}

void operator delete(void* p, const std::nothrow_t&) noexcept {
  std::free(p);
}

void* operator new[](std::size_t sz) {
  return operator new(sz);
}

void* operator new[](std::size_t sz, const std::nothrow_t&) noexcept {
  return operator new(sz, std::nothrow);
}

void operator delete[](void* p) noexcept {
  operator delete(p);
}

void operator delete[](void* p, const std::nothrow_t&) noexcept {
  operator delete(p, std::nothrow);
}

void __throw_bad_alloc() {
  throw std::bad_alloc();
}
