#include <ilias/vm/vmmap.h>

namespace ilias {
namespace vm {


vmmap_entry::~vmmap_entry() noexcept {}


#if defined(__i386__) || defined(__amd64__) || defined(__x86_64__)
template class vmmap_shard<arch::i386>;
#endif
#if defined(__amd64__) || defined(__x86_64__)
template class vmmap_shard<arch::amd64>;
#endif


}} /* namespace ilias::vm */
