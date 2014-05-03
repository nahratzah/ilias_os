#include <loader/x86_video.h>
#include <cstdint>
#include <cstring>
#include <string>
#include <abi/ext/printf.h>

namespace loader {
namespace {


const uint8_t gray_on_black = 0x07;
const uint8_t white_on_blue = 0x1f;

struct alignas(2) vram_char {
  char ch;
  uint8_t color;
};

static_assert(sizeof(vram_char) == 2, "Video ram character size must be 2.");
static_assert(alignof(vram_char) <= 2, "Video ram character "
                                       "alignment constraint.");
constexpr uintptr_t vram_addr = 0xb8000;
constexpr size_t vram_lines = 25;
constexpr size_t vram_columns = 80;


} /* namespace loader::<unnamed> */


std::tuple<uintptr_t, uintptr_t> vram_ram() noexcept {
  constexpr auto vram_char_count = vram_lines * vram_columns;
  return std::make_tuple(vram_addr,
                         vram_addr + sizeof(vram_char) * vram_char_count);
}

void bios_put_char(char ch) noexcept {
  vram_char*const video_ram = reinterpret_cast<vram_char*>(vram_addr);
  static uint32_t video_off = 0;

  if (ch != '\n') {
    if (video_off == vram_columns * vram_lines) {
      memmove(video_ram, video_ram + vram_columns,
              (vram_lines - 1U) * vram_columns * sizeof(vram_char));
      video_off -= vram_columns;
    }
    video_ram[video_off++] = vram_char{ ch, white_on_blue };
  } else {
    while (video_off % vram_columns != 0)
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
  int error = 0;
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
