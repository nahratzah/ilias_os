#ifndef _ILIAS_VM_PAGE_UNBUSY_FUTURE_H_
#define _ILIAS_VM_PAGE_UNBUSY_FUTURE_H_

#include <ilias/promise.h>
#include <ilias/workq.h>
#include <ilias/vm/vm-fwd.h>

namespace ilias {
namespace vm {

future<page_refptr> page_unbusy_future(workq_service&, future<page_refptr>);
future<page_refptr> page_unbusy_future(workq_ptr, future<page_refptr>);

}} /* namespace ilias::vm */

#endif /* _ILIAS_VM_PAGE_UNBUSY_FUTURE_H_ */
