#ifndef _ILIAS_VM_PAGE_INL_H_
#define _ILIAS_VM_PAGE_INL_H_

#include <ilias/vm/page.h>
#include <ilias/pmap/page.h>

namespace ilias {
namespace vm {

template<arch A>
page<A>::page(page_no<A> pgno) noexcept
: pgno_(pgno)
{}

}} /* namespace ilias::vm */

namespace std {

template<ilias::arch A>
auto hash<ilias::vm::page<A>>::operator()(const ilias::vm::page<A>& pg)
    const noexcept -> size_t {
  return hash<ilias::pmap::page_no<A>>()(pg.address());
}

} /* namespace std */

#endif /* _ILIAS_VM_PAGE_INL_H_ */
