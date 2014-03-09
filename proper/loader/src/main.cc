#include <loader/main.h>
#include <loader/x86_video.h>
#include <loader/ldexport_init.h>

namespace loader {

void main() {
  bios_put_str(ldexport_get().to_string());
}

} /* namespace loader */
