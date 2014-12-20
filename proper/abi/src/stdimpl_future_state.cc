#include <stdimpl/future_state.h>
#include <future>

_namespace_begin(std)
namespace impl {


auto future_state_base::lock() noexcept -> void {
  unsigned int spincount = 0U;

  for (;;) {
    bool expect = false;
    if (_predict_true(lck_.compare_exchange_weak(expect, true,
                                                 memory_order_acquire,
                                                 memory_order_relaxed)))
      return;

    ++spincount;
    if (spincount % 0x400U == 0U)
      this_thread::yield();
    else if (spincount > 40U)
      abi::ext::pause();
  }
}

auto future_state_base::ensure_uninitialized() const -> void {
  if (_predict_false(get_state() != state_t::uninitialized))
    future_error::__throw(future_errc::promise_already_satisfied);
}


} /* namespace std::impl */
_namespace_end(std)
