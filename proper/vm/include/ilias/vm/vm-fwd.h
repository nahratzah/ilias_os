#ifndef _ILIAS_VM_VM_FWD_H_
#define _ILIAS_VM_VM_FWD_H_

#include <ilias/refcnt.h>

namespace ilias {
namespace vm {


class page;
class page_owner;
class page_list;
class page_cache;
class page_alloc;
class default_page_alloc;

using page_refptr = refpointer<page>;


}} /* namespace ilias::vm */

#endif /* _ILIAS_VM_VM_FWD_H_ */
