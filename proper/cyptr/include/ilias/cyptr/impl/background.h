#ifndef _ILIAS_CYPTR_IMPL_BACKGROUND_H_
#define _ILIAS_CYPTR_IMPL_BACKGROUND_H_

#include <ilias/cyptr/impl/fwd.h>
#include <ilias/cyptr/impl/tags.h>
#include <atomic>
#include <mutex>
#include <ilias/linked_list.h>

#if __has_include(<condition_variable>)
# include <condition_variable>
#endif

namespace ilias {
namespace cyptr {
namespace impl {

class background;

background* get_background() noexcept;
void enable_background();
void disable_background() noexcept;

class background {
  friend background* get_background() noexcept;
  friend void enable_background();
  friend void disable_background() noexcept;

 private:
  background() = default;

 public:
  background(const background&) = delete;
  background(background&&) = delete;
  background& operator=(const background&) = delete;
  background& operator=(background&&) = delete;

  ~background() noexcept;

  bool process_one(bool) noexcept;
  bool enqueue(generation_ptr) noexcept;
  void wait() const noexcept;

 private:
  static std::atomic<background*> instance_;

  std::mutex mtx_;
  ilias::linked_list<generation, background_tag> queue_;
#if __has_include(<condition_variable>)
  std::conditional_variable cv_;
#endif
};


}}} /* namespace ilias::cyptr::impl */

#endif /* _ILIAS_CYPTR_IMPL_BACKGROUND_H_ */
