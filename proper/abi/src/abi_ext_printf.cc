#include <abi/ext/printf.h>

namespace __cxxabiv1 {
namespace ext {


const char DIGITS[MAX_BASE] = { '0', '1', '2', '3', '4', '5', '6', '7',
                                '8', '9', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z' };
const char DIGITS_U[MAX_BASE] = { '0', '1', '2', '3', '4', '5', '6', '7',
                                  '8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
                                  'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
                                  'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
                                  'W', 'X', 'Y', 'Z' };


template class printf_renderer<char>;
template class printf_renderer<wchar_t>;
template class printf_renderer<char16_t>;
template class printf_renderer<char32_t>;

template int vxprintf(printf_renderer<char>&,
                      std::basic_string_ref<char>, va_list) noexcept;
template int vxprintf(printf_renderer<wchar_t>&,
                      std::basic_string_ref<wchar_t>, va_list) noexcept;
template int vxprintf(printf_renderer<char16_t>&,
                      std::basic_string_ref<char16_t>, va_list) noexcept;
template int vxprintf(printf_renderer<char32_t>&,
                      std::basic_string_ref<char32_t>, va_list) noexcept;


}} /* namespace __cxxabiv1::ext */
