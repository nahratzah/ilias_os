#include <stdimpl/stats.h>

#if __has_include(<ilias/stats.h>)
# include <ilias/stats.h>
#endif

_namespace_begin(std)
namespace impl {


#if __has_include(<ilias/stats.h>)
_namespace(ilias)::global_stats_group std_stats{ nullptr, "std", {}, {} };
#endif


} /* namespace std::impl */
_namespace_end(std)
