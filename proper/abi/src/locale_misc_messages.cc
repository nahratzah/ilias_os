#include <locale_misc/messages.h>

_namespace_begin(std)


template class messages<char>;
template class messages<char16_t>;
template class messages<char32_t>;
template class messages<wchar_t>;

template class messages_byname<char>;
template class messages_byname<char16_t>;
template class messages_byname<char32_t>;
template class messages_byname<wchar_t>;


_namespace_end(std)
