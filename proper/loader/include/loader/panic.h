#ifndef _LOADER_PANIC_H_
#define _LOADER_PANIC_H_

#include <cstdarg>
#include <iosfwd>

namespace loader {

void panic(const char*, ...) noexcept __attribute__((noreturn));
void panic(std::string_ref, ...) noexcept __attribute__((noreturn));
void vpanic(std::string_ref, va_list) noexcept __attribute__((noreturn));

} /* namespace loader */

#endif /* _LOADER_PANIC_H_ */
