#include <ilias/vm/vmmap.h>
#include <ilias/stats.h>

namespace ilias {
namespace vm {
namespace stats {

global_stats_group vmmap_group{ &vm_group, "vmmap", {}, {} };
stats_counter vmmap_contention{ vmmap_group, "contention" };

} /* namespace ilias::vm::stats */


vmmap_entry::~vmmap_entry() noexcept {}


#if defined(__i386__) || defined(__amd64__) || defined(__x86_64__)
template class vmmap<arch::i386>;
#endif
#if defined(__amd64__) || defined(__x86_64__)
template class vmmap<arch::amd64>;
#endif


}} /* namespace ilias::vm */
