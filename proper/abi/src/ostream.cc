#include <ostream>

_namespace_begin(std)


template class basic_ostream<char>;
template class basic_ostream<char16_t>;
template class basic_ostream<char32_t>;
template class basic_ostream<wchar_t>;

template basic_ostream<char>& operator<< (basic_ostream<char>&,
                                          char);
template basic_ostream<char16_t>& operator<< (basic_ostream<char16_t>&,
                                              char16_t);
template basic_ostream<char32_t>& operator<< (basic_ostream<char32_t>&,
                                              char32_t);
template basic_ostream<wchar_t>& operator<< (basic_ostream<wchar_t>&,
                                             wchar_t);
template basic_ostream<char16_t>& operator<< (basic_ostream<char16_t>&,
                                              char);
template basic_ostream<char32_t>& operator<< (basic_ostream<char32_t>&,
                                              char);
template basic_ostream<wchar_t>& operator<< (basic_ostream<wchar_t>&,
                                             char);

template basic_ostream<char>& operator<< (basic_ostream<char>&,
                                          const char*);
template basic_ostream<char16_t>& operator<< (basic_ostream<char16_t>&,
                                              const char16_t*);
template basic_ostream<char32_t>& operator<< (basic_ostream<char32_t>&,
                                              const char32_t*);
template basic_ostream<wchar_t>& operator<< (basic_ostream<wchar_t>&,
                                             const wchar_t*);
template basic_ostream<char16_t>& operator<< (basic_ostream<char16_t>&,
                                              const char*);
template basic_ostream<char32_t>& operator<< (basic_ostream<char32_t>&,
                                              const char*);
template basic_ostream<wchar_t>& operator<< (basic_ostream<wchar_t>&,
                                             const char*);

extern template basic_ostream<char>& endl(basic_ostream<char>&);
extern template basic_ostream<char16_t>& endl(basic_ostream<char16_t>&);
extern template basic_ostream<char32_t>& endl(basic_ostream<char32_t>&);
extern template basic_ostream<wchar_t>& endl(basic_ostream<wchar_t>&);

extern template basic_ostream<char>& ends(basic_ostream<char>&);
extern template basic_ostream<char16_t>& ends(basic_ostream<char16_t>&);
extern template basic_ostream<char32_t>& ends(basic_ostream<char32_t>&);
extern template basic_ostream<wchar_t>& ends(basic_ostream<wchar_t>&);

extern template basic_ostream<char>& flush(basic_ostream<char>&);
extern template basic_ostream<char16_t>& flush(basic_ostream<char16_t>&);
extern template basic_ostream<char32_t>& flush(basic_ostream<char32_t>&);
extern template basic_ostream<wchar_t>& flush(basic_ostream<wchar_t>&);


_namespace_end(std)
