#include <loader/x86_video.h>
#include <cstdint>
#include <string>
#include <abi/ext/printf.h>

namespace loader {
namespace {


const uint8_t gray_on_black = 0x07;
const uint8_t white_on_blue = 0x1f;

struct vram_char {
  char ch;
  uint8_t color;
};

static_assert(sizeof(vram_char) == 2, "Video ram character size must be 2.");
static_assert(alignof(vram_char) <= 2, "Video ram character "
                                       "alignment constraint.");


} /* namespace loader::<unnamed> */


void bios_put_char(char ch) noexcept {
  vram_char*const video_ram = reinterpret_cast<vram_char*>(0xb8000);
  static uint32_t video_off = 0;

  if (ch != '\n') {
    video_ram[video_off++] = vram_char{ ch, white_on_blue };
  } else {
    while (video_off % 80 != 0)
      video_ram[video_off++] = vram_char{ ' ', gray_on_black };
  }
}

void bios_put_str(std::string_ref s) noexcept {
  for (auto c : s) bios_put_char(c);
}

void bios_vprintf(std::string_ref fmt, va_list ap) noexcept {
  class renderer_impl : public abi::ext::printf_renderer<char> {
   public:
    int do_append(std::string_ref s) noexcept override {
      bios_put_str(s);
      return 0;
    }
  };

  renderer_impl impl;
  int error;
  abi::ext::vxprintf_locals<char> locals;

  /* Parse fmt, gathering types and collecting specs. */
  if (!error) error = locals.parse_fmt(fmt);
  /* Read all va_list arguments. */
  if (!error) error = locals.load_arguments(ap);
  /* Resolve all fieldwidth, precision. */
  if (!error) error = locals.resolve_fieldwidth();
  /* Start rendering loop. */
  if (!error) error = locals.render(impl);
}

void bios_printf(std::string_ref fmt, ...) noexcept {
  va_list ap;

  va_start(ap, fmt);
  bios_vprintf(fmt, ap);
  va_end(ap);
}


} /* namespace loader */
