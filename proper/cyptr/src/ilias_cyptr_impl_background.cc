#include <ilias/cyptr/impl/background.h>
#include <ilias/cyptr/impl/generation.h>
#include <cassert>
#include <thread>

namespace ilias {
namespace cyptr {
namespace impl {


background* get_background() noexcept {
  return background::instance_.load(std::memory_order_relaxed);
}

void enable_background() {
  static background singleton;

  background::instance_.store(&singleton, std::memory_order_release);
}

void disable_background() noexcept {
  background* bg = background::instance_.exchange(nullptr,
                                                  std::memory_order_release);
  if (bg) {
#if __has_include(<condition_variable>)
    cv_.notify_all();
#endif
    do {
      if (!bg->process_one(false)) break;  // Process outstanding requests.
    } while (background::instance_.load(std::memory_order_relaxed) == nullptr);
  }
}

std::atomic<background*> background::instance_{ nullptr };

background::~background() noexcept {
  if (instance_.exchange(nullptr, std::memory_order_release) != nullptr)
    while (process_one(false));  // Process outstanding requests.
}

bool background::process_one(bool wait) noexcept {
  generation_ptr gp;
  {
    std::unique_lock<std::mutex> lck{ mtx_ };
    for (gp = generation_ptr(queue_.unlink_front(), false);
         wait && gp == nullptr && get_background() != nullptr;
         gp = generation_ptr(queue_.unlink_front(), false)) {
#if __has_include(<condition_variable>)
      cv_.wait(lck);
#else
      lck.unlock();
      std::this_thread::yield();
      lck.lock();
#endif
    }

    if (!gp) return false;
  }

  gp->marksweep_bg();
  return true;
}

bool background::enqueue(generation_ptr gp) noexcept {
  assert(gp != nullptr);

  std::lock_guard<std::mutex> lck{ mtx_ };
  if (instance_.load(std::memory_order_relaxed) == nullptr) return false;
  queue_.link_back(gp.release());
#if __has_include(<condition_variable>)
  cv_.notify_one();
#endif
  return true;
}


}}} /* namespace ilias::cyptr::impl */
