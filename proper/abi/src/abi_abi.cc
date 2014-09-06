#include <abi/abi.h>

#if __has_include(<ilias/stats.h>)
# include <ilias/stats.h>
#endif

namespace __cxxabiv1 {

#if __has_include(<ilias/stats.h>)
_namespace(ilias)::global_stats_group abi_group{
  nullptr, "abi", {}, {}
};

_namespace(ilias)::global_stats_group abi_ext_group{
  &abi_group, "ext", {}, {}
};
#endif /* __has_include(<ilias/stats.h>) */

} /* namespace __cxxabiv1 */
