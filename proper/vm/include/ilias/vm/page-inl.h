#ifndef _ILIAS_VM_PAGE_INL_H_
#define _ILIAS_VM_PAGE_INL_H_

#include <ilias/vm/page.h>
#include <ilias/pmap/page.h>
#include <abi/ext/atomic.h>

namespace ilias {
namespace vm {


inline page::page(page_no<native_arch> pgno) noexcept
: pgno_(pgno)
{}

inline auto page::get_flags() const noexcept -> flags_type {
  return flags_.load(memory_order_relaxed);
}

inline auto page::set_flag(flags_type f) noexcept -> flags_type {
  return flags_.fetch_or(f, memory_order_acq_rel);
}

inline auto page::clear_flag(flags_type f) noexcept -> flags_type {
  return flags_.fetch_and(~f, memory_order_acq_rel);
}

inline auto page::assign_masked_flags(flags_type f, flags_type msk) noexcept ->
    flags_type {
  assert((f & msk) == 0);
  flags_type expect = 0;

  while (!flags_.compare_exchange_weak(expect, (expect & ~msk) | f,
                                       memory_order_acq_rel,
                                       memory_order_relaxed)) {
    abi::ext::pause();
  }

  return expect;
}


inline auto page_list::empty() const noexcept -> bool {
  return data_.empty();
}

inline auto page_list::size() const noexcept -> page_count<native_arch> {
  return size_;
}

inline auto page_list::push(page_ptr pg) noexcept -> void {
  push_pages(pg, page_count<native_arch>(1));
}


}} /* namespace ilias::vm */

namespace std {

inline auto hash<ilias::vm::page>::operator()(const ilias::vm::page& pg)
    const noexcept -> size_t {
  return hash<ilias::pmap::page_no<ilias::native_arch>>()(pg.address());
}

} /* namespace std */

#endif /* _ILIAS_VM_PAGE_INL_H_ */
