#include <sstream>

_namespace_begin(std)


template class basic_stringbuf<char>;
template class basic_stringbuf<char16_t>;
template class basic_stringbuf<char32_t>;
template class basic_stringbuf<wchar_t>;

template class basic_istringstream<char>;
template class basic_istringstream<char16_t>;
template class basic_istringstream<char32_t>;
template class basic_istringstream<wchar_t>;

template class basic_ostringstream<char>;
template class basic_ostringstream<char16_t>;
template class basic_ostringstream<char32_t>;
template class basic_ostringstream<wchar_t>;

template class basic_stringstream<char>;
template class basic_stringstream<char16_t>;
template class basic_stringstream<char32_t>;
template class basic_stringstream<wchar_t>;


_namespace_end(std)
