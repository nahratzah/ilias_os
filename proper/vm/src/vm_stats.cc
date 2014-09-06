#include <ilias/vm/stats.h>
#include <ilias/stats.h>

namespace ilias {
namespace vm {
namespace stats {


global_stats_group vm_group{ nullptr, "vm", {}, {} };

unique_lock<mutex> tracked_lock(mutex& m, stats_counter& c) {
  unique_lock<mutex> l{ m, std::defer_lock };

  if (!l.try_lock()) {
    c.add();
    l.lock();
  }
  return l;
}


}}} /* namespace ilias::vm::stats */
