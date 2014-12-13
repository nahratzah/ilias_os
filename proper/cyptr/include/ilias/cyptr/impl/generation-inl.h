#ifndef _ILIAS_CYPTR_IMPL_GENERATION_INL_H_
#define _ILIAS_CYPTR_IMPL_GENERATION_INL_H_

#include <ilias/cyptr/impl/generation.h>
#include <mutex>
#include <ilias/cyptr/impl/basic_obj.h>

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

inline void generation::register_obj(basic_obj& o) noexcept {
  obj_.link_back(&o);
}

inline void generation::unregister_obj(basic_obj& o) noexcept {
  obj_.unlink(&o);
}


}}} /* namespace ilias::cyptr::impl */

#endif /* _ILIAS_CYPTR_IMPL_GENERATION_INL_H_ */
