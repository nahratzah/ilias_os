#ifndef _ILIAS_CYPTR_IMPL_TSTAMP_INL_H_
#define _ILIAS_CYPTR_IMPL_TSTAMP_INL_H_

#include <ilias/cyptr/impl/tstamp.h>

namespace ilias {
namespace cyptr {
namespace impl {


inline bool operator==(const tstamp& x, const tstamp& y) noexcept {
  return x.time_point == y.time_point && x.tid == y.tid && x.tick == y.tick;
}

inline bool operator!=(const tstamp& x, const tstamp& y) noexcept {
  return !(x == y);
}

inline bool operator<(const tstamp& x, const tstamp& y) noexcept {
  if (x.time_point != y.time_point) return x.time_point < y.time_point;
  if (x.tick != y.tick) return x.tick < y.tick;
  return x.tid < y.tid;
}

inline bool operator>(const tstamp& x, const tstamp& y) noexcept {
  return y < x;
}

inline bool operator<=(const tstamp& x, const tstamp& y) noexcept {
  return !(y < x);
}

inline bool operator>=(const tstamp& x, const tstamp& y) noexcept {
  return !(x < y);
}


}}} /* namespace ilias::cyptr::impl */

#endif /* _ILIAS_CYPTR_IMPL_TSTAMP_INL_H_ */
