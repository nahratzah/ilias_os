#ifndef _STDIMPL_ISTREAM_RSHIFT_H_
#define _STDIMPL_ISTREAM_RSHIFT_H_

#include <cdecl.h>
#include <iosfwd>

_namespace_begin(std)
namespace impl {


template<typename Char, typename Traits, typename Fn, typename... Args>
auto op_rshift_stream(basic_istream<Char, Traits>&,
                      Fn, Args&&...) ->
    basic_istream<Char, Traits>&;


} /* namespace std::impl */
_namespace_end(std)

#include <stdimpl/istream_rshift-inl.h>

#endif /* _STDIMPL_ISTREAM_RSHIFT_H_ */
