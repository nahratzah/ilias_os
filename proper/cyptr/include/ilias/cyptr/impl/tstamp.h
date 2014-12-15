#ifndef _ILIAS_CYPTR_IMPL_TSTAMP_H_
#define _ILIAS_CYPTR_IMPL_TSTAMP_H_

#include <chrono>
#include <cstddef>
#include <cstdint>

namespace ilias {
namespace cyptr {
namespace impl {


/*
 * A timestamp class.
 *
 * Tstamp is designed to be roughly ordered by time.
 * tstamp::now() yields a new timestamp, that is guaranteed unique.
 */
struct tstamp {
 public:
  using clock = std::chrono::steady_clock;
  using time_point_type = clock::time_point;
  using tid_type = std::uint64_t;
  using tick_type = std::size_t;

  static tstamp now() noexcept;

  time_point_type time_point;
  tid_type tid;
  tick_type tick;
};

bool operator==(const tstamp&, const tstamp&) noexcept;
bool operator!=(const tstamp&, const tstamp&) noexcept;
bool operator<(const tstamp&, const tstamp&) noexcept;
bool operator>(const tstamp&, const tstamp&) noexcept;
bool operator<=(const tstamp&, const tstamp&) noexcept;
bool operator>=(const tstamp&, const tstamp&) noexcept;


}}} /* namespace ilias::cyptr::impl */

#include <ilias/cyptr/impl/tstamp-inl.h>

#endif /* _ILIAS_CYPTR_IMPL_TSTAMP_H_ */
