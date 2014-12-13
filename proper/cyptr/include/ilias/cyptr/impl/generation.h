#ifndef _ILIAS_CYPTR_IMPL_GENERATION_H_
#define _ILIAS_CYPTR_IMPL_GENERATION_H_

#include <mutex>
#include <new>
#include <vector>
#include <queue>
#include <memory>
#include <functional>
#include <ilias/linked_list.h>
#include <ilias/refcnt.h>
#include <ilias/cyptr/impl/fwd.h>
#include <ilias/cyptr/impl/tags.h>
#include <ilias/cyptr/impl/tstamp.h>

namespace ilias {
namespace cyptr {
namespace impl {


class generation final
: public refcount_base<generation>
{
 public:
  using obj_list = linked_list<basic_obj, basic_obj_gen_linktag>;

  generation() noexcept = default;
  generation(const generation&) = delete;
  generation(generation&&) = delete;
  generation& operator=(const generation&) = delete;
  generation& operator=(generation&&) = delete;

  tstamp get_tstamp() const noexcept { return tstamp_; }

  bool before(const generation&) const noexcept;
  static generation_ptr new_generation() throw (std::bad_alloc);

  /* Implement lockable. */
  void lock() noexcept;
  bool try_lock() noexcept;
  void unlock() noexcept;

  void register_obj(basic_obj&) noexcept;
  void unregister_obj(basic_obj&) noexcept;

  /* Execute mark-sweep on the generation. */
  void marksweep() noexcept;
  void marksweep(std::unique_lock<generation>) noexcept;

  /* Fix generation relation between two objects that are to be linked. */
  static std::unique_lock<generation> fix_relation(basic_obj&, basic_obj&)
      noexcept;

 private:
  class fix_relation_state;
  class basic_obj_gcmp_elem_;
  struct basic_obj_gencmp_;
  using fixrel_locks =
      std::vector<fix_relation_state,
                  std::temporary_buffer_allocator<fix_relation_state>>;
  using fixrel_queue =
      std::priority_queue<basic_obj_gcmp_elem_,
                          std::vector<basic_obj_gcmp_elem_,
                                      std::temporary_buffer_allocator<
                                          basic_obj_gcmp_elem_>>,
                          basic_obj_gencmp_>;

  static basic_obj_lock fix_relation_(basic_obj&, basic_obj&) noexcept;
  static void lock_2_(basic_obj_lock&, basic_obj_lock&) noexcept;
  static fixrel_locks fixrel_lock_(basic_obj&, basic_obj_lock);
  static bool fixrel_lock_1_(fix_relation_state&, fixrel_queue&);
  static void fixrel_lock_add_edges_(generation&, fixrel_queue&);
  static basic_obj_lock fixrel_lockcomplete_(fixrel_locks, basic_obj&)
      noexcept;
  static basic_obj_lock fixrel_slow_1_(basic_obj&, basic_obj_lock) noexcept;
  static basic_obj_lock fixrel_slow_(basic_obj&, basic_obj_lock) noexcept;
  static void fixrel_splice_(generation&, generation&) noexcept;

  /* Mark-sweep processing functions -- requires lock. */
  linked_list<basic_obj, wavefront_tag> marksweep_init_() noexcept;
  void marksweep_process_(linked_list<basic_obj, wavefront_tag>&&) noexcept;
  linked_list<basic_obj, wavefront_tag> marksweep_dead_() noexcept;

  std::mutex mtx_;
  obj_list obj_;
  const tstamp tstamp_ = tstamp::now();
};


}}} /* namespace ilias::cyptr::impl */

#include <ilias/cyptr/impl/generation-inl.h>

#endif /* _ILIAS_CYPTR_IMPL_GENERATION_H_ */
