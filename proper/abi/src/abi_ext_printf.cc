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
                      typename printf_renderer<char>::string_type,
                      va_list) noexcept;
template int vxprintf(printf_renderer<wchar_t>&,
                      typename printf_renderer<wchar_t>::string_type,
                      va_list) noexcept;
template int vxprintf(printf_renderer<char16_t>&,
                      typename printf_renderer<char16_t>::string_type,
                      va_list) noexcept;
template int vxprintf(printf_renderer<char32_t>&,
                      typename printf_renderer<char32_t>::string_type,
                      va_list) noexcept;

template int deduce_printf_spec(_namespace(std)::basic_string_ref<char>&,
                                printf_spec*, printf_type*) noexcept;
template int deduce_printf_spec(_namespace(std)::basic_string_ref<wchar_t>&,
                                printf_spec*, printf_type*) noexcept;
template int deduce_printf_spec(_namespace(std)::basic_string_ref<char16_t>&,
                                printf_spec*, printf_type*) noexcept;
template int deduce_printf_spec(_namespace(std)::basic_string_ref<char32_t>&,
                                printf_spec*, printf_type*) noexcept;

template int parse_num(_namespace(std)::basic_string_ref<char>&) noexcept;
template int parse_num(_namespace(std)::basic_string_ref<wchar_t>&) noexcept;
template int parse_num(_namespace(std)::basic_string_ref<char16_t>&) noexcept;
template int parse_num(_namespace(std)::basic_string_ref<char32_t>&) noexcept;

template int parse_num_spec(_namespace(std)::basic_string_ref<char>&,
                            int*, bool*, bool*) noexcept;
template int parse_num_spec(_namespace(std)::basic_string_ref<wchar_t>&,
                            int*, bool*, bool*) noexcept;
template int parse_num_spec(_namespace(std)::basic_string_ref<char16_t>&,
                            int*, bool*, bool*) noexcept;
template int parse_num_spec(_namespace(std)::basic_string_ref<char32_t>&,
                            int*, bool*, bool*) noexcept;

template int printf_renderer<char>::append(
    _namespace(std)::basic_string_ref<wchar_t>) noexcept;
template int printf_renderer<char>::append(
    _namespace(std)::basic_string_ref<char16_t>) noexcept;
template int printf_renderer<char>::append(
    _namespace(std)::basic_string_ref<char32_t>) noexcept;

template int printf_renderer<wchar_t>::append(
    _namespace(std)::basic_string_ref<char>) noexcept;
template int printf_renderer<wchar_t>::append(
    _namespace(std)::basic_string_ref<char16_t>) noexcept;
template int printf_renderer<wchar_t>::append(
    _namespace(std)::basic_string_ref<char32_t>) noexcept;

template int printf_renderer<char16_t>::append(
    _namespace(std)::basic_string_ref<char>) noexcept;
template int printf_renderer<char16_t>::append(
    _namespace(std)::basic_string_ref<wchar_t>) noexcept;
template int printf_renderer<char16_t>::append(
    _namespace(std)::basic_string_ref<char32_t>) noexcept;

template int printf_renderer<char32_t>::append(
    _namespace(std)::basic_string_ref<char>) noexcept;
template int printf_renderer<char32_t>::append(
    _namespace(std)::basic_string_ref<wchar_t>) noexcept;
template int printf_renderer<char32_t>::append(
    _namespace(std)::basic_string_ref<char16_t>) noexcept;

template int printf_arg::apply(printf_renderer<char>&,
                               printf_spec) const noexcept;
template int printf_arg::apply(printf_renderer<wchar_t>&,
                               printf_spec) const noexcept;
template int printf_arg::apply(printf_renderer<char16_t>&,
                               printf_spec) const noexcept;
template int printf_arg::apply(printf_renderer<char32_t>&,
                               printf_spec) const noexcept;

template struct vxprintf_locals<char>;
template struct vxprintf_locals<char16_t>;
template struct vxprintf_locals<char32_t>;
template struct vxprintf_locals<wchar_t>;


vxprintf_locals_base::vxprintf_locals_base() noexcept {}

int vxprintf_locals_base::resolve_fieldwidth() noexcept {
  printf_spec*const b = &specs[0];
  printf_spec*const e = b + lit_count;
  for (printf_spec* i = b; i != e; ++i) {
    if (i->pff & PFF_FIELDWIDTH_IS_ARGIDX) {
      int v;
      int error = va[i->fieldwidth].as_int(&v);
      i->fieldwidth = v;
      if (error) return error;
    }

    if (i->pff & PFF_PRECISION_IS_ARGIDX) {
      int v;
      int error = va[i->precision].as_int(&v);
      i->precision = v;
      if (error) return error;
    }

    i->pff &= ~(PFF_FIELDWIDTH_IS_ARGIDX | PFF_PRECISION_IS_ARGIDX);
  }
  return 0;
}

int vxprintf_locals_base::load_arguments(va_list ap) noexcept {
  int error;
  printf_arg*const b = &va[0];
  printf_arg*const e = b + argsize;
  for (printf_arg* i = b; i != e; ++i)
    if ((error = i->read(ap))) return error;
  return 0;
}


}} /* namespace __cxxabiv1::ext */
