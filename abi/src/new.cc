#include <new>
#include <atomic>
#include <abi/memory.h>
#include <cdecl.h>

namespace std {


const nothrow_t nothrow{};

bad_alloc::~bad_alloc() noexcept {}

const char* bad_alloc::what() const noexcept {
  return "std::bad_alloc";
}


bad_array_new_length::~bad_array_new_length() noexcept {}

const char* bad_array_new_length::what() const noexcept {
  return "std::bad_array_new_length";
}


namespace {

std::atomic<new_handler> nh;

} /* namespace std::<unnamed> */


new_handler get_new_handler() noexcept {
  return nh.load(std::memory_order_acquire);
}

new_handler set_new_handler(new_handler n) noexcept {
  return nh.exchange(n, std::memory_order_acq_rel);
}


} /* namespace std */


namespace {

abi::heap& throwing_heap() noexcept {
  static abi::big_heap impl{ "abi/operator new" };
  return impl;
}
abi::heap& no_throw_heap() noexcept {
  static abi::big_heap impl{ "abi/operator new/nothrow" };
  return impl;
}
abi::heap& throwing_array_heap() noexcept {
  static abi::big_heap impl{ "abi/operator new[]" };
  return impl;
}
abi::heap& no_throw_array_heap() noexcept {
  static abi::big_heap impl{ "abi/operator new[]/nothrow" };
  return impl;
}

void* new_impl(abi::heap& heap, std::size_t sz) {
  void* p;
  for (p = heap.malloc(sz); _predict_false(!p); p = heap.malloc(sz)) {
    std::new_handler nh = std::get_new_handler();
    if (!nh) std::__throw_bad_alloc();
    try {
      (*nh)();
    } catch (const std::bad_alloc&) {
      throw;
    } catch (...) {
      std::unexpected();
    }
  }

  abi::memzero(p, sz);
  return p;
}

void* new_impl_nothrow(abi::heap& heap, std::size_t sz) noexcept {
  try {
    return new_impl(heap, sz);
  } catch (const std::bad_alloc&) {
    return nullptr;
  }
}

} /* namespace <unnamed> */


void* __attribute__((weak)) operator new(std::size_t sz) {
  return new_impl(throwing_heap(), sz);
}

void* __attribute__((weak)) operator new(
    std::size_t sz, const std::nothrow_t&) noexcept {
  return new_impl_nothrow(no_throw_heap(), sz);
}

void __attribute__((weak)) operator delete(void* p) noexcept {
  if (p) throwing_heap().free(p);
}

void __attribute__((weak)) operator delete(
    void* p, const std::nothrow_t&) noexcept {
  if (p) no_throw_heap().free(p);
}


void* __attribute__((weak)) operator new[](std::size_t sz) {
  return new_impl(throwing_array_heap(), sz);
}

void* __attribute__((weak)) operator new[](
    std::size_t sz, const std::nothrow_t&) noexcept {
  return new_impl_nothrow(no_throw_array_heap(), sz);
}

void __attribute__((weak)) operator delete[](void* p) noexcept {
  if (p) throwing_array_heap().free(p);
}

void __attribute__((weak)) operator delete[](
    void* p, const std::nothrow_t&) noexcept {
  if (p) no_throw_array_heap().free(p);
}

void __throw_bad_alloc() {
  throw std::bad_alloc();
}
