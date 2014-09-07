#include <ilias/vm/page.h>
#include <abi/ext/atomic.h>

namespace ilias {
namespace vm {


auto page::try_set_flag_iff_zero(flags_type f) noexcept ->
    pair<flags_type, bool> {
  flags_type expect = flags_.load(memory_order_relaxed);
  expect &= ~f;

  bool succes = !flags_.compare_exchange_strong(expect, expect | f,
                                                memory_order_acquire,
                                                memory_order_relaxed);
  return { expect, succes };
}

auto page::set_flag_iff_zero(flags_type f) noexcept -> flags_type {
  flags_type expect = 0;

  while (!flags_.compare_exchange_weak(expect, expect | f,
                                       memory_order_acquire,
                                       memory_order_relaxed)) {
    expect &= ~f;
    abi::ext::pause();
  }

  return expect;
}


}} /* namespace ilias::vm */
