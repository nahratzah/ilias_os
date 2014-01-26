#ifndef _ABI_EXT_PRINTF_H_
#define _ABI_EXT_PRINTF_H_

#include <abi/abi.h>
#include <cstddef_misc/valist.h>
#include <abi/ext/string_piece.h>

namespace __cxxabiv1 {
namespace ext {


#define NL_ARGMAX	128	// XXX find correct header file


class printf_renderer {
 public:
  virtual ~printf_renderer() noexcept;
  int append(c_string_piece) noexcept;
  uintmax_t length() const noexcept { return len_; }
  uintmax_t size() const noexcept { return len_; }
  virtual char get_thousand_sep() const noexcept;

 private:
  virtual int do_append(c_string_piece) noexcept = 0;

  uintmax_t len_ = 0;
};

int vxprintf(printf_renderer& r, c_string_piece fmt, va_list& ap) noexcept;


}} /* namespace __cxxabiv1::ext */

#endif /* _ABI_EXT_PRINTF_H_ */
