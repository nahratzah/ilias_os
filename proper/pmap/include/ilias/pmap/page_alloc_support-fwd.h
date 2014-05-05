#ifndef _ILIAS_PMAP_PAGE_ALLOC_SUPPORT_FWD_H_
#define _ILIAS_PMAP_PAGE_ALLOC_SUPPORT_FWD_H_

#include <ilias/arch.h>
#include <memory>

namespace ilias {
namespace pmap {

template<arch Arch = native_arch> class page_ptr;
template<typename T, arch Arch> struct unmap_page_deleter;

template<typename T, arch Arch> using pmap_mapped_ptr =
    std::unique_ptr<T, unmap_page_deleter<T, Arch>>;

}} /* namespace ilias::pmap */

#endif /* _ILIAS_PMAP_PAGE_ALLOC_SUPPORT_FWD_H_ */
