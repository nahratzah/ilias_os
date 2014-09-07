#ifndef _ILIAS_VM_PAGE_INL_H_
#define _ILIAS_VM_PAGE_INL_H_

#include <ilias/vm/page.h>
#include <ilias/pmap/page.h>

namespace ilias {
namespace vm {

inline page::page(page_no<native_arch> pgno) noexcept
: pgno_(pgno)
{}

}} /* namespace ilias::vm */

namespace std {

inline auto hash<ilias::vm::page>::operator()(const ilias::vm::page& pg)
    const noexcept -> size_t {
  return hash<ilias::pmap::page_no<ilias::native_arch>>()(pg.address());
}

} /* namespace std */

#endif /* _ILIAS_VM_PAGE_INL_H_ */
