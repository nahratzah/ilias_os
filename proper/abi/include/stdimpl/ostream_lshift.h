#ifndef _STDIMPL_OSTREAM_LSHIFT_H_
#define _STDIMPL_OSTREAM_LSHIFT_H_

#include <cdecl.h>
#include <iosfwd>

_namespace_begin(std)
namespace impl {


template<typename Char, typename Traits, typename SChar, typename STraits>
auto op_lshift_stream(basic_ostream<Char, Traits>&,
                      basic_string_ref<SChar, STraits>) ->
    enable_if_t<is_same<Char, SChar>::value, basic_ostream<Char, Traits>&>;

template<typename Char, typename Traits, typename SChar, typename STraits,
         typename Fn>
auto op_lshift_stream(basic_ostream<Char, Traits>&,
                      basic_string_ref<SChar, STraits>,
                      Fn) ->
    enable_if_t<!is_same<Char, SChar>::value, basic_ostream<Char, Traits>&>;


} /* namespace std::impl */
_namespace_end(std)

#include <stdimpl/ostream_lshift-inl.h>

#endif /* _STDIMPL_OSTREAM_LSHIFT_H_ */
