#include <stdimpl/shared_ptr_ownership.h>

_namespace_begin(std)
namespace impl {


void shared_ptr_ownership::~shared_ptr_ownership() noexcept {
  assert(shared_refcount_.load() == 0);
  assert(refcount_.load() == 0);
}

bool shared_ptr_ownership::shared_ptr_acquire_from_weak_ptr() noexcept {
  long expect = 1;
  while (!shared_refcount_.compare_exchange_weak(expect, except + 1,
                                                 memory_order_acquire,
                                                 memory_order_relaxed)) {
    if (_predict_false(expect == 0)) return false;
  }
  return true;
}

void shared_ptr_ownership::panic_no_references() noexcept {
  abi::panic("shared_ptr: reference counter is 0");
  for (;;);
}

void shared_ptr_ownership::destroy_(shared_ptr_ownership* spd) {
  spd->destroy_me();
}

void shared_ptr_ownership::release_pointee_() noexcept {
  release_pointee();
}


} /* namespace std::impl */
_namespace_end(std)
