#include <ilias/cyptr/impl/tstamp.h>
#include <cdecl.h>
#include <atomic>
#include <tuple>

namespace ilias {
namespace cyptr {
namespace impl {


tstamp tstamp::now() noexcept {
  using std::make_tuple;
  using std::tie;
  using clock = time_point_type::clock;

  static std::atomic<tid_type> tid_allocator{ 1U };
  static thread_local time_point_type prev_time_point;
  static thread_local tid_type tid;
  static thread_local tick_type prev_tick;

  /*
   * Ensure a unique thread-id.
   * Since thread-ids may be re-used after a thread terminates,
   * we use our own thread ids which are never re-used.
   */
  if (_predict_false(tid == 0U)) {
    tid = tid_allocator.fetch_add(1U, std::memory_order_relaxed);
    assert(tid != 0U);
  }

  /*
   * Calculate timestamp.
   * Sequence inside the timestamp is handled below
   * (due to its value needs to be reset when the timestamp increments).
   */
  tstamp rv;
  tie(rv.time_point, rv.tid, rv.tick) = make_tuple(clock::now(), tid, 0U);

  /*
   * Assign tick value.
   * If timestamp is the same, tick must increase,
   * otherwise it must reset.
   */
  if (rv.time_point == prev_time_point) {
    rv.tick = prev_tick++;
    assert(prev_tick != 0U);
  } else {
    assert(prev_time_point < rv.time_point);
    tie(prev_time_point, prev_tick) = make_tuple(rv.time_point, 1U);
  }

  return rv;
}


}}} /* namespace ilias::cyptr::impl */
