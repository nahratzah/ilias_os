#ifndef _DEV_RND_RND_H_
#define _DEV_RND_RND_H_

#include <array>
#include <chrono>
#include <string>
#include <ilias/stats.h>


namespace ilias {
namespace rnd {


using clock_type = std::chrono::high_resolution_clock;
using duration = clock_type::duration;
using time_point = clock_type::time_point;


class entropy_source {
 private:
  class rnd_src_stats {
   public:
    rnd_src_stats(std::string_ref) noexcept;

   private:
    stats_group grp_;

   public:
    stats_counter calls, bits;
  };

 public:
  entropy_source(std::string_ref, bool = true, bool = false) noexcept;
  ~entropy_source() noexcept;
  void enqueue_randomness(int val) noexcept;

 private:
  const bool count_entropy_;
  const bool max_entropy_;
  time_point last_time_;
  std::array<duration, 2> last_delta_;
  rnd_src_stats stats_;
};

void random_start() noexcept;

entropy_source& get_timer_entropy_source() noexcept;
void add_timer_randomness(int) noexcept;


}} /* namespace ilias::dev::rnd */


#endif /* _DEV_RND_RND_H_ */
