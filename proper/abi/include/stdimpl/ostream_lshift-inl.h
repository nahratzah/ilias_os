#ifndef _STDIMPL_OSTREAM_LSHIFT_INL_H_
#define _STDIMPL_OSTREAM_LSHIFT_INL_H_

#include <stdimpl/ostream_lshift.h>

_namespace_begin(std)
namespace impl {


template<typename Char, typename Traits, typename SChar, typename STraits>
auto op_lshift_stream(basic_ostream<Char, Traits>& os,
                      basic_string_ref<SChar, STraits> s) ->
    enable_if_t<is_same<Char, SChar>::value, basic_ostream<Char, Traits>&> {
  using ios_base = basic_ostream<Char, Traits>;

  return os.op_lshift_([&]() -> bool {
                         using out_iter = ostreambuf_iterator<Char, Traits>;

                         const auto os_width = os.width();
                         const make_unsigned_t<decltype(os_width)> width =
                             (os.width() < 0 ? 0 : os_width);
                         const bool left =
                             ((os.flags() & ios_base::adjustfield) ==
                              ios_base::left);
                         out_iter out{ os };

                         if (!left && width > s.length())
                           out = fill_n(out, width - s.length(), os.fill());

                         os.rdbuf()->sputn(s.data(), s.size());

                         if (left && width > s.length())
                           out = fill_n(out, width - s.length(), os.fill());

                         os.width(0);
                         return out.failed();
                       });
}

template<typename Char, typename Traits, typename SChar, typename STraits,
         typename Fn>
auto op_lshift_stream(basic_ostream<Char, Traits>& os,
                      basic_string_ref<SChar, STraits> s,
                      Fn tf) ->
    enable_if_t<!is_same<Char, SChar>::value, basic_ostream<Char, Traits>&> {
  using ios_base = basic_ostream<Char, Traits>;

  return os.op_lshift_([&]() -> bool {
                         using out_iter = ostreambuf_iterator<Char, Traits>;

                         const auto os_width = os.width();
                         const make_unsigned_t<decltype(os_width)> width =
                             (os.width() < 0 ? 0 : os_width);
                         const bool left =
                             ((os.flags() & ios_base::adjustfield) ==
                              ios_base::left);
                         out_iter out{ os };

                         if (!left && width > s.length())
                           out = fill_n(out, width - s.length(), os.fill());

                         out = transform(begin(s), end(s), out, move(tf));

                         if (left && width > s.length())
                           out = fill_n(out, width - s.length(), os.fill());

                         os.width(0);
                         return out.failed();
                       });
}


} /* namespace std::impl */
_namespace_end(std)

#endif /* _STDIMPL_OSTREAM_LSHIFT_INL_H_ */
