#include <ilias/cyptr/impl/generation.h>

namespace ilias {
namespace cyptr {
namespace impl {


generation_ptr generation::new_generation() throw (std::bad_alloc) {
  return make_refpointer<generation>();
}

void generation::marksweep() noexcept {
  std::unique_lock<generation> lck{ *this };

  marksweep_process_(marksweep_init_());
  linked_list<basic_obj, wavefront_tag> dead = marksweep_dead_();

  while (!dead.empty()) {
    basic_obj* dead_obj = dead.unlink_front();
    lck.unlock();

    /* XXX invoke pre-destruction function for dead_obj. */

    obj_color expect = obj_color::dying;
    if (dead_obj->color_.compare_exchange_strong(expect,
                                                 obj_color::dead,
                                                 std::memory_order_acq_rel,
                                                 std::memory_order_relaxed)) {
      /* XXX invoke destructor function for dead_obj. */
    }

    lck.lock();
  }
}

/*
 * Build the wavefront and mark any object not on the wavefront as
 * maybe dead.
 */
linked_list<basic_obj, wavefront_tag> generation::marksweep_init_() noexcept {
  linked_list<basic_obj, wavefront_tag> wavefront;
  for (basic_obj& o : obj_) {
    obj_color expect = obj_color::linked;

    if (o.refcnt_.load(std::memory_order_relaxed) != 0U) {
      /* Add to wavefront below. */ ;
    } else if (o.color_.compare_exchange_strong(expect,
                                                obj_color::maybe_dying,
                                                std::memory_order_acq_rel,
                                                std::memory_order_acquire)) {
      continue;
    } else if (expect == obj_color::unlinked) {
      /* SKIP: Add to wavefront below. */
    } else {
      continue;
    }

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
      assert(dst.color_.load() != obj_color::linked);
      if (dst.color_.load(std::memory_order_relaxed) != obj_color::maybe_dying)
        continue;

      /* Add dst object to wavefront, since it is reachable. */
      dst.color_.store(obj_color::linked, std::memory_order_relaxed);
      wavefront.link_back(&dst);
    }

    /* Done processing front of wavefront. */
    wavefront.unlink_front();
  }
}

linked_list<basic_obj, wavefront_tag> generation::marksweep_dead_() noexcept {
  linked_list<basic_obj, wavefront_tag> rv;

  for (basic_obj o : obj_) {
    obj_color expect = obj_color::maybe_dying;
    if (o.color_.compare_exchange_strong(expect,
                                         obj_color::dying,
                                         std::memory_order_acq_rel,
                                         std::memory_order_relaxed))
      rv.link_back(&o);
  }

  return rv;
}


}}} /* namespace ilias::cyptr::impl */
