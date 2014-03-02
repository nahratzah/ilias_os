#include <loader/x86_video.h>
#include <cstdint>
#include <cstdarg>
#include <abi/ext/printf.h>

namespace loader {
namespace {


const uint8_t gray_on_black = 0x07;
const uint8_t white_on_blue = 0x1f;

volatile uint8_t*const video_ram = reinterpret_cast<uint8_t*>(0xb8000);


} /* namespace loader::<unnamed> */


void bios_put_char(char ch) noexcept {
  static uint32_t video_off = 0;

  if (ch == '\n') {
    do {
      video_ram[video_off++] = ' ';
      video_ram[video_off++] = gray_on_black;
    } while (video_off / 2 % 80);
  } else {
    video_ram[video_off++] = static_cast<abi::uint8_t>(ch);
    video_ram[video_off++] = white_on_blue;
  }
}

void bios_put_str(std::string_ref s) noexcept {
  for (auto c : s) bios_put_char(c);
}

void bios_printf(std::string_ref fmt, ...) noexcept {
  class renderer_impl : public abi::ext::printf_renderer<char> {
   public:
    int do_append(std::string_ref s) noexcept override {
      bios_put_str(s);
      return 0;
    }
  };

  renderer_impl impl;
  va_list ap;

  va_start(ap, fmt);
  abi::ext::vxprintf(impl, fmt, ap);
  va_end(ap);
}


} /* namespace loader */
