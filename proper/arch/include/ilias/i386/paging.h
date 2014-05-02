#ifndef _ILIAS_I386_PAGING_H_
#define _ILIAS_I386_PAGING_H_

#include <ilias/pmap/pmap.h>

namespace ilias {
namespace i386 {

void enable_paging(pmap::pmap<native_arch>&);

}} /* namespace ilias::i386 */

#endif /* _ILIAS_I386_PAGING_H_ */
