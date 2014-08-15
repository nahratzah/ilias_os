#include <istream>

_namespace_begin(std)


template class basic_istream<char>;
template class basic_istream<char16_t>;
template class basic_istream<char32_t>;
template class basic_istream<wchar_t>;

template basic_istream<char>& operator>> (basic_istream<char>&,
                                          char&);
template basic_istream<char16_t>& operator>> (basic_istream<char16_t>&,
                                              char16_t&);
template basic_istream<char32_t>& operator>> (basic_istream<char32_t>&,
                                              char32_t&);
template basic_istream<wchar_t>& operator>> (basic_istream<wchar_t>&,
                                             wchar_t&);

template basic_istream<char>& operator>> (basic_istream<char>&,
                                          char*);
template basic_istream<char16_t>& operator>> (basic_istream<char16_t>&,
                                              char16_t*);
template basic_istream<char32_t>& operator>> (basic_istream<char32_t>&,
                                              char32_t*);
template basic_istream<wchar_t>& operator>> (basic_istream<wchar_t>&,
                                             wchar_t*);

template basic_istream<char>& ws(basic_istream<char>&);
template basic_istream<char16_t>& ws(basic_istream<char16_t>&);
template basic_istream<char32_t>& ws(basic_istream<char32_t>&);
template basic_istream<wchar_t>& ws(basic_istream<wchar_t>&);

template class basic_iostream<char>;
template class basic_iostream<char16_t>;
template class basic_iostream<char32_t>;
template class basic_iostream<wchar_t>;


_namespace_end(std)
