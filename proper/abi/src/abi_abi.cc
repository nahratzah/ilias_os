#include <abi/abi.h>
#include <ilias/stats.h>

namespace __cxxabiv1 {

_namespace(ilias)::global_stats_group abi_group{
  nullptr, "abi", {}, {}
};

_namespace(ilias)::global_stats_group abi_ext_group{
  &abi_group, "abi", {}, {}
};

} /* namespace __cxxabiv1 */
