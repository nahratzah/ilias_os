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


} /* namespace ilias::cyptr::background_processing */


#ifndef _SINGLE_THREADED
threaded_processing::threaded_processing() {
  background_processing::enable();
  try {
    thr_ = std::thread(&background_processing::process, true, 0);
  } catch (...) {
    background_processing::disable();
    throw;
  }
}

threaded_processing::~threaded_processing() noexcept {
  if (thr_.joinable()) {
    background_processing::disable();
    thr_.join();
  }
}
#endif


}} /* namespace ilias::cyptr */
