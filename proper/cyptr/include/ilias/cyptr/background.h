#ifndef _ILIAS_CYPTR_BACKGROUND_H_
#define _ILIAS_CYPTR_BACKGROUND_H_

#include <cstddef>
#ifndef _SINGLE_THREADED
# include <thread>
#endif

namespace ilias {
namespace cyptr {
namespace background_processing {


/*
 * Enable background processing.
 * All mark-sweep calls will be queued, instead of performed immediately.
 */
void enable();

/*
 * Disable background processing.
 * All mark-sweep calls will be executed immediately.
 * This is the default.
 */
void disable() noexcept;

/*
 * Test if background processing is enabled.
 */
bool is_enabled() noexcept;

/*
 * Process outstanding marksweep requests.
 *
 * Supply true if you want the function to wait for work (excellent in threads)
 * and supply a non-zero count to limit the units of work performed.
 *
 * Defaults: process 1 unit of work, iff immediately available.
 *
 * While this function won't wait for work to become available, it may still
 * block on lock acquisition.
 */
std::size_t process(bool = false, std::size_t = 1) noexcept;


} /* namespace ilias::cyptr::background_processing */


#ifndef _SINGLE_THREADED
/*
 * Trivial helper class that moves processing into a separate thread.
 *
 * Usage:
 * int main() {
 *   threaded_processing tp;
 *
 *   ... // Program code here.
 * }
 *
 * Due to the way it's implemented, you should only have one of those in your
 * program.
 *
 * (This manages a thread, don't stick it in global storage.)
 */
class threaded_processing {
 public:
  threaded_processing();
  threaded_processing(const threaded_processing&) = delete;
  threaded_processing& operator=(const threaded_processing&) = delete;
  threaded_processing(threaded_processing&&) = delete;
  threaded_processing& operator=(threaded_processing&&) = delete;
  ~threaded_processing() noexcept;

 private:
  std::thread thr_;
};
#endif


}} /* namespace ilias::cyptr */

#endif /* _ILIAS_CYPTR_BACKGROUND_H_ */
