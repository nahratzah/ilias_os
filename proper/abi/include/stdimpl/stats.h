#ifndef _STDIMPL_STATS_H_
#define _STDIMPL_STATS_H_

#include <cdecl.h>

#if __has_include(<ilias/stats.h>)
# include <ilias/stats-fwd.h>
#endif

_namespace_begin(std)
namespace impl {


#if __has_include(<ilias/stats.h>)
extern _namespace(ilias)::global_stats_group std_stats;
#endif


} /* namespace std::impl */
_namespace_end(std)

#endif /* _STDIMPL_STATS_H_ */
