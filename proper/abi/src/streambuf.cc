#include <streambuf>

_namespace_begin(std)


template class basic_streambuf<char>;
template class basic_streambuf<char16_t>;
template class basic_streambuf<char32_t>;
template class basic_streambuf<wchar_t>;


namespace impl {

template streamsize copybuf(basic_streambuf<char>&,
                            basic_streambuf<char>&);
template streamsize copybuf(basic_streambuf<char16_t>&,
                            basic_streambuf<char16_t>&);
template streamsize copybuf(basic_streambuf<char32_t>&,
                            basic_streambuf<char32_t>&);
template streamsize copybuf(basic_streambuf<wchar_t>&,
                            basic_streambuf<wchar_t>&);

template streamsize copybuf(basic_streambuf<char>&,
                            basic_streambuf<char>&,
                            char);
template streamsize copybuf(basic_streambuf<char16_t>&,
                            basic_streambuf<char16_t>&,
                            char16_t);
template streamsize copybuf(basic_streambuf<char32_t>&,
                            basic_streambuf<char32_t>&,
                            char32_t);
template streamsize copybuf(basic_streambuf<wchar_t>&,
                            basic_streambuf<wchar_t>&,
                            wchar_t);

} /* namespace impl */
_namespace_end(std)
