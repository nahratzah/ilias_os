#include <ilias/cyptr/impl/generation.h>

namespace ilias {
namespace cyptr {
namespace impl {


generation_ptr generation::new_generation() throw (std::bad_alloc) {
  return make_refpointer<generation>();
}

void generation::release_ref(basic_obj& o) noexcept {
  std::lock_guard<generation>{ *this };

  const auto o_refcnt = o.refcnt_.fetch_sub(1U, std::memory_order_release);
  assert(o_refcnt != 0U);
  if (o_refcnt != 1U) return;  // Still live.
  o.color_ = obj_color::gen_linked;

  marksweep_();
}


void generation::marksweep_() noexcept {
  auto wavefront = marksweep_init_();
  marksweep_process_(std::move(wavefront));
  /* XXX delete left overs */
}


/*
 * Build the wavefront and mark any object not on the wavefront as
 * maybe dead.
 */
linked_list<basic_obj, wavefront_tag> generation::marksweep_init_() noexcept {
  linked_list<basic_obj, wavefront_tag> wavefront;
  for (basic_obj& o : obj_) {
    assert(o.color_ == obj_color::unlinked ||
           o.color_ == obj_color::strong_linked ||
           o.color_ == obj_color::gen_linked);
    if (o.refcnt_.load(std::memory_order_relaxed) == 0U &&
        o.color_ == obj_color::gen_linked)
      o.color_ = obj_color::maybe_dead;
    else
      wavefront.link_back(&o);
  }

  return wavefront;
}


/*
 * Process the wavefront.
 */
void generation::marksweep_process_(
    linked_list<basic_obj, wavefront_tag>&& wavefront) noexcept {
  while (!wavefront.empty()) {
    basic_obj& o = wavefront.front();
    std::lock_guard<std::mutex> edge_lock{ o.mtx_ };

    /* Process all objects for each edge. */
    for (edge& e : o.edge_list_) {
      using flags_type = edge::dst_pointer::flags_type;

      /*
       * Read the destination pointer and acquire the lock on edge.
       */
      std::lock_guard<edge> edgeptr_lock{ e };
      basic_obj* dst_ptr = std::get<0>(e.dst_.load(std::memory_order_relaxed));

      /* Skip nullptr. */
      if (dst_ptr == nullptr)
        continue;

      basic_obj& dst = *dst_ptr;
      /* Skip edges in different generations. */
      if (std::get<0>(dst.gen_.load_no_acquire(std::memory_order_relaxed)) !=
          this)
        continue;
      /* Skip edges already declared reachable. */
      assert(dst.color_ != obj_color::gen_linked);
      if (dst.color_ != obj_color::maybe_dead)
        continue;

      /* Add dst object to wavefront, since it is reachable. */
      dst.color_ = obj_color::gen_linked;
      wavefront.link_back(&dst);
    }

    /* Done processing front of wavefront. */
    wavefront.unlink_front();
  }
}


}}} /* namespace ilias::cyptr::impl */
