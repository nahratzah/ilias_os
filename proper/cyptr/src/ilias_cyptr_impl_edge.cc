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
  if (new_dst == nullptr) {
    reset();
    return;
  }

  basic_obj_lock objlck{ src_ };
  if (_predict_false(get_generation_seq(*new_dst) <
                     objlck.get_generation().get_tstamp()))
    generation::fix_relation(objlck, src_, *new_dst);

  std::unique_lock<edge> lck{ *this };
  if (std::get<0>(new_dst->gen_.load(std::memory_order_relaxed)) !=
      &objlck.get_generation())
    refcnt_acquire(*new_dst);
  basic_obj* old_dst =
      std::get<0>(dst_.exchange(std::make_tuple(new_dst, UNLOCKED),
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


}}} /* namespace ilias::cyptr::impl */
