#include <mutex>
#include <atomic>
#include <system_error>

_namespace_begin(std)
namespace impl {

struct owner_tag_ {};
thread_local owner_tag_ owner_tag;

} /* namespace std::impl */


mutex::~mutex() noexcept {
  assert_msg(owner_tag_ == nullptr, "Attempt to destroy owned mutex.");
}

auto mutex::lock() -> void {
  if (_predict_false(owner_tag_.load(memory_order_relaxed) ==
                     &impl::owner_tag))
    throw system_error(make_error_code(errc::resource_deadlock_would_occur));

  impl_.decrement();
  const void* prev_owner = owner_tag_.exchange(&impl::owner_tag,
                                               memory_order_relaxed);
  assert_msg(prev_owner == nullptr, "Mutex corrupted.");
}

auto mutex::try_lock() noexcept -> bool {
  assert_msg(owner_tag_.load(memory_order_relaxed) != &impl::owner_tag,
             "mutex::try_lock: current thread already owns the mutex");

  if (!impl_.try_decrement()) return false;
  const void* prev_owner = owner_tag_.exchange(&impl::owner_tag,
                                               memory_order_relaxed);
  assert_msg(prev_owner == nullptr, "Mutex corrupted.");
  return true;
}

auto mutex::unlock() noexcept -> void {
  const void* prev_owner = owner_tag_.exchange(nullptr, memory_order_relaxed);
  assert_msg(prev_owner == &impl::owner_tag,
             "mutex::unlock: current thread does not hold this lock");
  impl_.increment();
}


recursive_mutex::~recursive_mutex() noexcept {
  assert_msg(owner_tag_ == nullptr, "Attempt to destroy owned mutex.");
}

auto recursive_mutex::lock() -> void {
  if (owner_tag_.load(memory_order_relaxed) == &impl::owner_tag) {
    recurs_++;
    return;
  }

  impl_.decrement();
  const void* prev_owner = owner_tag_.exchange(&impl::owner_tag,
                                               memory_order_relaxed);
  assert_msg(prev_owner == nullptr, "Mutex corrupted.");
  const auto old_recurs = recurs_++;
  assert_msg(old_recurs == 0, "Mutex corrupted.");
}

auto recursive_mutex::try_lock() noexcept -> bool {
  if (owner_tag_.load(memory_order_relaxed) == &impl::owner_tag) {
    recurs_++;
    return true;
  }

  if (!impl_.try_decrement()) return false;
  const void* prev_owner = owner_tag_.exchange(&impl::owner_tag,
                                               memory_order_relaxed);
  assert_msg(prev_owner == nullptr, "Mutex corrupted.");
  const auto old_recurs = recurs_++;
  assert_msg(old_recurs == 0, "Mutex corrupted.");
  return true;
}

auto recursive_mutex::unlock() noexcept -> void {
  assert(owner_tag_.load(memory_order_relaxed) == &impl::owner_tag);
  if (--recurs_ == 0) {
    const void* prev_owner = owner_tag_.exchange(nullptr,
                                                 memory_order_relaxed);
    assert_msg(prev_owner == &impl::owner_tag,
               "mutex::unlock: current thread does not hold this lock");
    impl_.increment();
  }
}


_namespace_end(std)
