#include <ilias/cyptr/background.h>
#include <ilias/cyptr/impl/background.h>

namespace ilias {
namespace cyptr {
namespace background_processing {


void enable() {
  ilias::cyptr::impl::enable_background();
}

void disable() noexcept {
  ilias::cyptr::impl::disable_background();
}

bool is_enabled() noexcept {
  return (ilias::cyptr::impl::get_background() != nullptr);
}

std::size_t process(bool wait, std::size_t n) noexcept {
  ilias::cyptr::impl::background* bg = ilias::cyptr::impl::get_background();
  if (bg == nullptr) return 0;

  bool do_all = (n == 0U);
  std::size_t processed;
  for (processed = 0; processed < n || do_all; ++processed)
    if (!bg->process_one(wait)) break;
  return processed;
}


}}} /* namespace ilias::cyptr::background_processing */
