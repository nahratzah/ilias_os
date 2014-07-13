#ifndef _ABI_EXT_PRINTF_H_
#define _ABI_EXT_PRINTF_H_

#include <abi/abi.h>
#include <abi/panic.h>
#include <abi/ext/unicode.h>
#include <cstdarg>
#include <cerrno>
#include <cstdarg>
#include <limits>
#include <string>

namespace __cxxabiv1 {
namespace ext {


template<typename Char, typename Traits = _namespace(std)::char_traits<Char>>
class printf_renderer {
 public:
  using string_type = _namespace(std)::basic_string_ref<Char, Traits>;
  using size_type = typename string_type::size_type;

  virtual ~printf_renderer() noexcept;
  int append(string_type) noexcept;
  template<typename C, typename T> int append(
      _namespace(std)::basic_string_ref<C, T>)
      noexcept;
  size_type length() const noexcept { return len_; }
  size_type size() const noexcept { return len_; }
  virtual Char get_thousand_sep() const noexcept;
  virtual Char get_pos_sign() const noexcept;  // '+'
  virtual Char get_neg_sign() const noexcept;  // '-'

 private:
  virtual int do_append(string_type) noexcept = 0;

  size_type len_ = 0;
};

template<typename Char, typename Traits = _namespace(std)::char_traits<Char>>
struct vxprintf_locals;

template<typename Char, typename Traits = _namespace(std)::char_traits<Char>>
int vxprintf(printf_renderer<Char, Traits>&,
             typename printf_renderer<Char, Traits>::string_type,
             va_list) noexcept;

extern template class printf_renderer<char>;
extern template class printf_renderer<wchar_t>;
extern template class printf_renderer<char16_t>;
extern template class printf_renderer<char32_t>;

extern template int vxprintf(printf_renderer<char>&,
                             typename printf_renderer<char>::string_type,
                             va_list) noexcept;
extern template int vxprintf(printf_renderer<wchar_t>&,
                             typename printf_renderer<wchar_t>::string_type,
                             va_list) noexcept;
extern template int vxprintf(printf_renderer<char16_t>&,
                             typename printf_renderer<char16_t>::string_type,
                             va_list) noexcept;
extern template int vxprintf(printf_renderer<char32_t>&,
                             typename printf_renderer<char32_t>::string_type,
                             va_list) noexcept;


}} /* namespace __cxxabiv1::ext */

#include <abi/ext/printf-inl.h>

#endif /* _ABI_EXT_PRINTF_H_ */
