#ifndef _X86_VIDEO_H_
#define _X86_VIDEO_H_

#include <cstdarg>
#include <string>

namespace loader {

void bios_put_char(char) noexcept;
void bios_put_str(std::string_ref) noexcept;

/*
 * These are extern "C", since the va_list trips up the name mangling
 * in the compiler.
 */
extern "C" void bios_vprintf(std::string_ref, va_list) noexcept;
void bios_printf(std::string_ref, ...) noexcept;

} /* namespace loader */

#endif /* _X86_VIDEO_H_ */
