#include <new>
#include <cdecl.h>
#include <atomic>
#include <mutex>
#include <abi/memory.h>

_namespace_begin(std)


const nothrow_t nothrow{};

bad_alloc::bad_alloc() noexcept {}
bad_alloc::~bad_alloc() noexcept {}

const char* bad_alloc::what() const noexcept {
  return "std::bad_alloc";
}


bad_array_new_length::bad_array_new_length() noexcept {}
bad_array_new_length::~bad_array_new_length() noexcept {}

const char* bad_array_new_length::what() const noexcept {
  return "std::bad_array_new_length";
}


namespace {

atomic<new_handler> nh;

} /* namespace std::<unnamed> */


new_handler get_new_handler() noexcept {
  return nh.load(memory_order_acquire);
}

new_handler set_new_handler(new_handler n) noexcept {
  return nh.exchange(n, memory_order_acq_rel);
}


_namespace_end(std)


namespace {

using _namespace(std)::once_flag;
using _namespace(std)::aligned_storage_t;
using _namespace(std)::size_t;

abi::big_heap& throwing_heap() noexcept {
  static once_flag guard;
  static aligned_storage_t<sizeof(abi::big_heap), alignof(abi::big_heap)> impl;
  void*const impl_ptr = &impl;

  call_once(guard,
            [](void* p) { new (p) abi::big_heap("operator new"); },
            impl_ptr);
  return *static_cast<abi::big_heap*>(impl_ptr);
}

abi::big_heap& no_throw_heap() noexcept {
  static once_flag guard;
  static aligned_storage_t<sizeof(abi::big_heap), alignof(abi::big_heap)> impl;
  void*const impl_ptr = &impl;

  call_once(guard,
            [](void* p) { new (p) abi::big_heap("abi/operator new/nothrow"); },
            impl_ptr);
  return *static_cast<abi::big_heap*>(impl_ptr);
}

abi::big_heap& throwing_array_heap() noexcept {
  static once_flag guard;
  static aligned_storage_t<sizeof(abi::big_heap), alignof(abi::big_heap)> impl;
  void*const impl_ptr = &impl;

  call_once(guard,
            [](void* p) { new (p) abi::big_heap("abi/operator new[]"); },
            impl_ptr);
  return *static_cast<abi::big_heap*>(impl_ptr);
}

abi::big_heap& no_throw_array_heap() noexcept {
  static once_flag guard;
  static aligned_storage_t<sizeof(abi::big_heap), alignof(abi::big_heap)> impl;
  void*const impl_ptr = &impl;

  call_once(guard,
            [](void* p) {
              new (p) abi::big_heap("abi/operator new[]/nothrow");
            },
            impl_ptr);
  return *static_cast<abi::big_heap*>(impl_ptr);
}


void* new_impl(abi::big_heap& heap, size_t sz,
               size_t align = alignof(_TYPES(max_align_t))) {
  void* p;
  for (p = heap.malloc(sz); _predict_false(!p); p = heap.malloc(sz, align)) {
    _namespace(std)::new_handler nh = _namespace(std)::get_new_handler();
    if (!nh) _namespace(std)::__throw_bad_alloc();
    try {
      (*nh)();
    } catch (const _namespace(std)::bad_alloc&) {
      throw;
    } catch (...) {
      _namespace(std)::unexpected();
    }
  }

  abi::memzero(p, sz);
  return p;
}

void* new_impl_nothrow(abi::big_heap& heap, size_t sz,
                       size_t align = alignof(_TYPES(max_align_t))) noexcept {
  try {
    return new_impl(heap, sz, align);
  } catch (const _namespace(std)::bad_alloc&) {
    return nullptr;
  }
}

} /* namespace <unnamed> */


void* __attribute__((weak)) operator new(size_t sz) {
  return new_impl(throwing_heap(), sz);
}

void* __attribute__((weak)) operator new(
    size_t sz, const _namespace(std)::nothrow_t&) noexcept {
  return new_impl_nothrow(no_throw_heap(), sz);
}

void* __attribute__((weak)) operator new(
    size_t sz, _namespace(std)::align_val_t align) {
  return new_impl(throwing_heap(), sz, static_cast<size_t>(align));
}

void* __attribute__((weak)) operator new(
    size_t sz, _namespace(std)::align_val_t align,
    const _namespace(std)::nothrow_t&) noexcept {
  return new_impl_nothrow(no_throw_heap(), sz, static_cast<size_t>(align));
}

void __attribute__((weak)) operator delete(void* p) noexcept {
  if (p) throwing_heap().free(p);
}

void __attribute__((weak)) operator delete(
    void* p, const _namespace(std)::nothrow_t&) noexcept {
  if (p) no_throw_heap().free(p);
}

void __attribute__((weak)) operator delete(
    void* p, _namespace(std)::align_val_t) noexcept {
  if (p) throwing_heap().free(p);
}

void __attribute__((weak)) operator delete(
    void* p, _namespace(std)::align_val_t,
    const _namespace(std)::nothrow_t&) noexcept {
  if (p) no_throw_heap().free(p);
}

void __attribute__((weak)) operator delete(void* p, size_t sz) noexcept {
  if (p) throwing_heap().free(p, sz);
}

void __attribute__((weak)) operator delete(
    void* p, size_t sz, const _namespace(std)::nothrow_t&) noexcept {
  if (p) no_throw_heap().free(p, sz);
}

void __attribute__((weak)) operator delete(
    void* p, size_t sz, _namespace(std)::align_val_t) noexcept {
  if (p) throwing_heap().free(p, sz);
}

void __attribute__((weak)) operator delete(
    void* p, size_t sz, _namespace(std)::align_val_t,
    const _namespace(std)::nothrow_t&) noexcept {
  if (p) no_throw_heap().free(p, sz);
}


void* __attribute__((weak)) operator new[](size_t sz) {
  return new_impl(throwing_array_heap(), sz);
}

void* __attribute__((weak)) operator new[](
    size_t sz, const _namespace(std)::nothrow_t&) noexcept {
  return new_impl_nothrow(no_throw_array_heap(), sz);
}

void* __attribute__((weak)) operator new[](
    size_t sz, _namespace(std)::align_val_t align) {
  return new_impl(throwing_array_heap(), sz, static_cast<size_t>(align));
}

void* __attribute__((weak)) operator new[](
    size_t sz, _namespace(std)::align_val_t align,
    const _namespace(std)::nothrow_t&) noexcept {
  return new_impl_nothrow(no_throw_array_heap(),
                          sz, static_cast<size_t>(align));
}

void __attribute__((weak)) operator delete[](void* p) noexcept {
  if (p) throwing_array_heap().free(p);
}

void __attribute__((weak)) operator delete[](
    void* p, const _namespace(std)::nothrow_t&) noexcept {
  if (p) no_throw_array_heap().free(p);
}

void __attribute__((weak)) operator delete[](
    void* p, _namespace(std)::align_val_t) noexcept {
  if (p) throwing_array_heap().free(p);
}

void __attribute__((weak)) operator delete[](
    void* p, _namespace(std)::align_val_t,
    const _namespace(std)::nothrow_t&) noexcept {
  if (p) no_throw_array_heap().free(p);
}

void __attribute__((weak)) operator delete[](void* p, size_t sz) noexcept {
  if (p) throwing_array_heap().free(p, sz);
}

void __attribute__((weak)) operator delete[](
    void* p, size_t sz, const _namespace(std)::nothrow_t&) noexcept {
  if (p) no_throw_array_heap().free(p, sz);
}

void __attribute__((weak)) operator delete[](
    void* p, size_t sz, _namespace(std)::align_val_t) noexcept {
  if (p) throwing_array_heap().free(p, sz);
}

void __attribute__((weak)) operator delete[](
    void* p, size_t sz, _namespace(std)::align_val_t,
    const _namespace(std)::nothrow_t&) noexcept {
  if (p) no_throw_array_heap().free(p, sz);
}


_namespace_begin(std)

void __throw_bad_alloc() {
  throw _namespace(std)::bad_alloc();
}

_namespace_end(std)
