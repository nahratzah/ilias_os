#ifndef _ILIAS_PMAP_PAGE_ALLOC_H_
#define _ILIAS_PMAP_PAGE_ALLOC_H_

#include <ilias/arch.h>
#include <ilias/pmap/consts.h>

namespace ilias {
namespace pmap {


/* Implementation is external to pmap module. */
page_no<native_arch> allocate_page();
void deallocate_page(page_no<native_arch>) noexcept;

/* Implementation is external to pmap module. */
void* map_page(page_no<native_arch>);
void unmap_page(void*);


}} /* namespace ilias::pmap */

#endif /* _ILIAS_PMAP_PAGE_ALLOC_H_ */
