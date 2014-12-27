#ifndef _ILIAS_VM_PAGE_UNBUSY_FUTURE_H_
#define _ILIAS_VM_PAGE_UNBUSY_FUTURE_H_

#include <ilias/future.h>
#include <ilias/workq.h>
#include <ilias/vm/vm-fwd.h>

namespace ilias {
namespace vm {

future<page_ptr> page_unbusy_future(workq_service&, future<page_ptr>&&);
future<page_ptr> page_unbusy_future(workq_ptr, future<page_ptr>&&);
future<page_ptr> page_unbusy_future(workq_service&, shared_future<page_ptr>);
future<page_ptr> page_unbusy_future(workq_ptr, shared_future<page_ptr>);

}} /* namespace ilias::vm */

#endif /* _ILIAS_VM_PAGE_UNBUSY_FUTURE_H_ */
