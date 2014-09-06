#ifndef _ILIAS_VM_STATS_H_
#define _ILIAS_VM_STATS_H_

#include <ilias/stats-fwd.h>
#include <mutex>

namespace ilias {
namespace vm {
namespace stats {

using std::unique_lock;
using std::mutex;

extern global_stats_group vm_group;

unique_lock<mutex> tracked_lock(mutex&, stats_counter&);

}}} /* namespace ilias::vm::stats */

#endif /* _ILIAS_VM_STATS_H_ */
