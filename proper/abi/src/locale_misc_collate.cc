#include <locale_misc/collate.h>

_namespace_begin(std)


template class collate<char>;
template class collate<char16_t>;
template class collate<char32_t>;
template class collate<wchar_t>;

template class collate_byname<char>;
template class collate_byname<char16_t>;
template class collate_byname<char32_t>;
template class collate_byname<wchar_t>;


_namespace_end(std)
