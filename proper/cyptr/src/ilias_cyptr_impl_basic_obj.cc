#include <ilias/cyptr/impl/basic_obj.h>
#include <cassert>
#include <system_error>

namespace ilias {
namespace cyptr {
namespace impl {


basic_obj::~basic_obj() noexcept {
  std::lock_guard<std::mutex> guard{ mtx_ };
  assert(edge_list_.empty());
}


tstamp get_generation_seq(const basic_obj& o) noexcept {
  generation_ptr gen = std::get<0>(o.gen_.load(std::memory_order_relaxed));
  return gen->get_tstamp();
}


auto basic_obj_lock::lock() -> void {
  using std::system_error;
  using std::make_error_code;
  using std::errc;
  using std::memory_order_relaxed;

  if (_predict_false(obj_ == nullptr))
    throw system_error(make_error_code(errc::operation_not_permitted));
  if (_predict_false(gen_ != nullptr))
    throw system_error(make_error_code(errc::resource_deadlock_would_occur));

  std::unique_lock<generation> gen_lck;
  generation_ptr gen = std::get<0>(obj_->gen_.load(memory_order_relaxed));
  for (;;) {
    assert(gen != nullptr);
    gen_lck = std::unique_lock<generation>(*gen);

    /*
     * Check that optimistic lock succeeded:
     * if the generation changed during our lock attempt, it failed.
     */
    generation_ptr check = std::get<0>(obj_->gen_.load(memory_order_relaxed));
    if (check == gen) break;  // GUARD

    /*
     * Failed to lock, use check generation for next attempt to lock.
     *
     * Note that we need to unlock gen_lck prior to overwriting gen,
     * since gen may now be the last valid reference to its generation!
     */
    gen_lck.unlock();
    gen = std::move(check);
  }

  /*
   * Mark this as locked.
   */
  gen_ = std::move(gen);
  gen_lck.release();
}

auto basic_obj_lock::try_lock() -> bool {
  using std::system_error;
  using std::make_error_code;
  using std::errc;
  using std::memory_order_relaxed;

  if (_predict_false(obj_ == nullptr))
    throw system_error(make_error_code(errc::operation_not_permitted));
  if (_predict_false(gen_ != nullptr))
    throw system_error(make_error_code(errc::resource_deadlock_would_occur));

  /*
   * Try to lock generation.
   */
  generation_ptr gen = std::get<0>(obj_->gen_.load(memory_order_relaxed));
  std::unique_lock<generation> gen_lck =
      std::unique_lock<generation>(*gen, std::try_to_lock);

  /*
   * Check if attempt to lock succeeded and if the generation didn't change
   * during the attempt to lock.
   */
  if (!gen_lck ||
      std::get<0>(obj_->gen_.load_no_acquire(memory_order_relaxed)) != gen)
    return false;

  /*
   * Mark this as locked.
   */
  gen_ = std::move(gen);
  gen_lck.release();
  return true;
}

auto basic_obj_lock::unlock() -> generation_ptr {
  using std::system_error;
  using std::make_error_code;
  using std::errc;

  if (_predict_false(gen_ == nullptr))
    throw system_error(make_error_code(errc::operation_not_permitted));

  gen_->unlock();
  return std::exchange(gen_, nullptr);
}


}}} /* namespace ilias::cyptr::impl */
