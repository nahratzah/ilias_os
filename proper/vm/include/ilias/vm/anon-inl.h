#ifndef _ILIAS_VM_ANON_INL_H_
#define _ILIAS_VM_ANON_INL_H_

#include <ilias/vm/anon.h>
#include <algorithm>
#include <iterator>

namespace ilias {
namespace vm {


inline auto anon_vme::entry_deleter_::operator()(const entry* e)
    const noexcept -> void {
  if (e->refcnt_.fetch_sub(1U, memory_order_release) == 1U)
    delete e;
}


template<typename Iter>
anon_vme::anon_vme(Iter b, Iter e) {
  transform(b, e, back_inserter(data_), &copy_entry_);
}

inline auto anon_vme::empty() const noexcept -> bool {
  return data_.empty();
}

inline auto anon_vme::present(page_count<native_arch> off) const noexcept ->
    bool {
  assert(off.get() >= 0 &&
         (static_cast<make_unsigned_t<decltype(off.get())>>(off.get()) <
          data_.size()));

  return (data_[off.get()] != nullptr);
}


}} /* namespace ilias::vm */

#endif /* _ILIAS_VM_ANON_INL_H_ */
