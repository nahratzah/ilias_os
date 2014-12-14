#include <ilias/cyptr/impl/basic_obj.h>
#include <cassert>
#include <system_error>

namespace ilias {
namespace cyptr {
namespace impl {


basic_obj::basic_obj() throw (std::bad_alloc)
: basic_obj(generation::new_generation())
{}

basic_obj::basic_obj(refpointer<generation> gen) noexcept
: gen_(std::make_tuple(std::cref(gen), std::bitset<0>()))
{
  assert(gen != nullptr);
  std::lock_guard<generation> genlck{ *gen };
  gen->register_obj(*this);
}

basic_obj::~basic_obj() noexcept {
  /*
   * Acquire object lock, because otherwise another thread may drag us away
   * from our current generation,
   * as part of a generation::fix_relation() invocation.
   */
  basic_obj_lock lck{ *this };

  lck.get_generation().unregister_obj(*this);
  assert(edge_list_.empty());
}


tstamp get_generation_seq(const basic_obj& o) noexcept {
  generation_ptr gen = std::get<0>(o.gen_.load(std::memory_order_relaxed));
  return gen->get_tstamp();
}

basic_obj* refcnt_initialize(basic_obj* o,
                             std::function<void (void*)> destructor,
                             void* destructor_arg) noexcept {
  if (_predict_false(o == nullptr)) return nullptr;

  auto old_refcnt = o->refcnt_.exchange(1U,
                                        std::memory_order_acquire);
  assert_msg(old_refcnt == 0U, "Pointer initialized twice!");

  o->set_destructor_(std::move(destructor), std::move(destructor_arg));
  return o;
}

void basic_obj::set_destructor_(std::function<void (void*)> destructor,
                                void* destructor_arg) noexcept {
  assert(!destructor_);
  assert(!destructor_arg_);

  /* Assign destructor. */
  destructor_ = std::move(destructor);
  destructor_arg_ = destructor_arg;

  /* Mark as linked. */
  obj_color old_color = color_.exchange(obj_color::linked,
                                        std::memory_order_release);
  assert(old_color == obj_color::unlinked);
}

void refcnt_acquire(const basic_obj& o, std::uintptr_t n) noexcept {
  const auto old = o.refcnt_.fetch_add(n, std::memory_order_acquire);
  assert(old + n != 0U);
}

void refcnt_release(const basic_obj& o, std::uintptr_t n) noexcept {
  generation_ptr gen;

  std::uintptr_t expect = o.refcnt_.load(std::memory_order_relaxed);
  do {
    assert(expect >= n);
    if (gen == nullptr && expect == n)
      gen = std::get<0>(o.gen_.load(std::memory_order_acquire));
  } while (!o.refcnt_.compare_exchange_weak(expect,
                                            expect - n,
                                            std::memory_order_release,
                                            std::memory_order_relaxed));

  if (expect == n)
    gen->marksweep();
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
