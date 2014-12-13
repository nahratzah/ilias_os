#include <ilias/cyptr/impl/edge.h>

namespace ilias {
namespace cyptr {
namespace impl {


auto edge::reset() noexcept -> void {
  basic_obj_lock objlck{ src_ };
  std::unique_lock<edge> lck{ *this };
  basic_obj* old_dst =
      std::get<0>(dst_.exchange(std::make_tuple(nullptr, UNLOCKED),
                                std::memory_order_release));
  lck.release();  // Unlocked by assignment.

  if (old_dst != nullptr &&
      (std::get<0>(old_dst->gen_.load(std::memory_order_relaxed)) ==
       &objlck.get_generation())) {
    generation_ptr gen = objlck.release();
    gen->marksweep(std::unique_lock<generation>(*gen, std::adopt_lock));
  } else {
    objlck.unlock();
    if (old_dst != nullptr)
      refcnt_release(*old_dst);
  }
}

auto edge::reset(basic_obj* new_dst) noexcept -> void {
  if (_predict_false(new_dst == nullptr)) {
    reset();
    return;
  }

  /* Fix relationship between src and new_dst and lock src. */
  std::unique_lock<generation> objlck = generation::fix_relation(src_,
                                                                 *new_dst);
  assert(objlck.owns_lock());
  assert(src_.has_generation(*objlck.mutex()));

  /* Exchange dst pointer. */
  std::unique_lock<edge> lck{ *this };
  if (std::get<0>(new_dst->gen_.load(std::memory_order_relaxed)) !=
      objlck.mutex())
    refcnt_acquire(*new_dst);
  basic_obj* old_dst =
      std::get<0>(dst_.exchange(std::make_tuple(new_dst, UNLOCKED),
                                std::memory_order_release));
  lck.release();  // Unlocked by assignment.

  /* Drop reference to old value. */
  if (old_dst != nullptr &&
      (std::get<0>(old_dst->gen_.load(std::memory_order_relaxed)) ==
       objlck.mutex())) {
    generation_ptr gen = src_.get_generation();
    assert(gen == objlck.mutex());
    gen->marksweep(std::move(objlck));
  } else {
    objlck.unlock();
    if (old_dst != nullptr)
      refcnt_release(*old_dst);
  }
}


}}} /* namespace ilias::cyptr::impl */
