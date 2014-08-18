#ifndef _ILIAS_I386_PAGING_H_
#define _ILIAS_I386_PAGING_H_

#include <ilias/pmap/page.h>
#include <ilias/pmap/pmap.h>
#include <ilias/i386/gdt.h>

namespace ilias {
namespace i386 {

void enable_paging(const gdt_t&, pmap::pmap<native_arch>&);

}} /* namespace ilias::i386 */

#endif /* _ILIAS_I386_PAGING_H_ */
