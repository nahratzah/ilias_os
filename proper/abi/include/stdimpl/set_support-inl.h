#ifndef _STDIMPL_SET_SUPPORT_INL_H_
#define _STDIMPL_SET_SUPPORT_INL_H_

#include <stdimpl/set_support.h>

_namespace_begin(std)
namespace impl {

template<typename T, class Tag>
template<typename... Args>
set_elem<T, Tag>::set_elem(Args&&... args)
: value(forward<Args>(args)...)
{}


template<typename T, class Tag>
template<typename... Args>
unordered_set_elem<T, Tag>::unordered_set_elem(Args&&... args)
: value(forward<Args>(args)...)
{}

} /* namespace std::impl */
_namespace_end(std)

#endif /* _STDIMPL_SET_SUPPORT_INL_H_ */
