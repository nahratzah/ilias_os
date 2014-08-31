#ifndef _STDIMPL_EXC_ERRNO_H_
#define _STDIMPL_EXC_ERRNO_H_

#include <cdecl.h>
#include <utility>

_namespace_begin(std)
namespace impl {

pair<bool, int> errno_from_current_exception(bool = false);
bool errno_catch_handler(bool = true);

} /* namespace std::impl */
_namespace_end(std)


#endif /* _STDIMPL_EXC_ERRNO_H_ */
