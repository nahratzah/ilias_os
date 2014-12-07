#ifndef _ILIAS_CYPTR_IMPL_GENERATION_INL_H_
#define _ILIAS_CYPTR_IMPL_GENERATION_INL_H_

#include <ilias/cyptr/impl/generation.h>

namespace ilias {
namespace cyptr {
namespace impl {


inline bool generation::before(const generation& o) const noexcept {
  return tstamp_ < o.tstamp_;
}

inline void generation::lock() noexcept {
  mtx_.lock();
}

inline bool generation::try_lock() noexcept {
  return mtx_.try_lock();
}

inline void generation::unlock() noexcept {
  mtx_.unlock();
}


}}} /* namespace ilias::cyptr::impl */

#endif /* _ILIAS_CYPTR_IMPL_GENERATION_INL_H_ */
