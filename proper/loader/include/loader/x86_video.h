#ifndef _X86_VIDEO_H_
#define _X86_VIDEO_H_

#include <string>

namespace loader {

void bios_put_char(char) noexcept;
void bios_put_str(std::string_ref) noexcept;
void bios_printf(std::string_ref, ...) noexcept;

} /* namespace loader */

#endif /* _X86_VIDEO_H_ */
