#ifndef _STDIMPL_ISTREAM_RSHIFT_INL_H_
#define _STDIMPL_ISTREAM_RSHIFT_INL_H_

#include <stdimpl/istream_rshift.h>

_namespace_begin(std)
namespace impl {


template<typename Char, typename Traits, typename Fn, typename... Args>
auto op_rshift_stream(basic_istream<Char, Traits>& is,
                      Fn fn, Args&&... args) ->
    basic_istream<Char, Traits>& {
  return is.op_rshift_(fn, forward<Args>(args)...);
}


} /* namespace std::impl */
_namespace_end(std)

#endif /* _STDIMPL_ISTREAM_RSHIFT_INL_H_ */
