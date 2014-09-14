#ifndef _ILIAS_VM_ANON_INL_H_
#define _ILIAS_VM_ANON_INL_H_

#include <ilias/vm/anon.h>
#include <algorithm>
#include <iterator>

namespace ilias {
namespace vm {


inline auto anon_vme::entry::present() const noexcept -> bool {
  lock_guard<mutex> l{ guard_ };
  return page_ != nullptr || in_progress_.is_initialized();
}


template<typename Iter>
anon_vme::anon_vme(workq_ptr wq, Iter b, Iter e)
: vmmap_entry(move(wq)),
  data_(b, e)
{}

inline auto anon_vme::empty() const noexcept -> bool {
  return data_.empty();
}


}} /* namespace ilias::vm */

#endif /* _ILIAS_VM_ANON_INL_H_ */
