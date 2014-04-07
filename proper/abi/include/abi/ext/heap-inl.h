namespace __cxxabiv1 {
namespace ext {


inline auto heap::stats_data::malloc_calls() const noexcept -> uintmax_t {
  return malloc_calls_.load(_namespace(std)::memory_order_relaxed);
}

inline auto heap::stats_data::resize_calls() const noexcept -> uintmax_t {
  return resize_calls_.load(_namespace(std)::memory_order_relaxed);
}

inline auto heap::stats_data::free_calls() const noexcept -> uintmax_t {
  return free_calls_.load(_namespace(std)::memory_order_relaxed);
}

inline auto heap::stats_data::malloc_bytes() const noexcept -> uintmax_t {
  return malloc_bytes_.load(_namespace(std)::memory_order_relaxed);
}

inline auto heap::stats_data::resize_bytes_up() const noexcept -> uintmax_t {
  return resize_bytes_up_.load(_namespace(std)::memory_order_relaxed);
}

inline auto heap::stats_data::resize_bytes_down() const noexcept -> uintmax_t {
  return resize_bytes_down_.load(_namespace(std)::memory_order_relaxed);
}

inline auto heap::stats_data::free_bytes() const noexcept -> uintmax_t {
  return free_bytes_.load(_namespace(std)::memory_order_relaxed);
}

inline auto heap::stats_data::malloc_fail() const noexcept -> uintmax_t {
  return malloc_fail_.load(_namespace(std)::memory_order_relaxed);
}

inline auto heap::stats_data::resize_fail() const noexcept -> uintmax_t {
  return resize_fail_.load(_namespace(std)::memory_order_relaxed);
}


inline auto heap::stats::malloc_calls() const noexcept -> uintmax_t {
  return malloc_calls_;
}

inline auto heap::stats::resize_calls() const noexcept -> uintmax_t {
  return resize_calls_;
}

inline auto heap::stats::free_calls() const noexcept -> uintmax_t {
  return free_calls_;
}

inline auto heap::stats::malloc_bytes() const noexcept -> uintmax_t {
  return malloc_bytes_;
}

inline auto heap::stats::resize_bytes_up() const noexcept -> uintmax_t {
  return resize_bytes_up_;
}

inline auto heap::stats::resize_bytes_down() const noexcept -> uintmax_t {
  return resize_bytes_down_;
}

inline auto heap::stats::free_bytes() const noexcept -> uintmax_t {
  return free_bytes_;
}

inline auto heap::stats::malloc_fail() const noexcept -> uintmax_t {
  return malloc_fail_;
}

inline auto heap::stats::resize_fail() const noexcept -> uintmax_t {
  return resize_fail_;
}

inline auto heap::stats::used() const noexcept -> uintmax_t {
  return malloc_bytes() +
         resize_bytes_up() -
         resize_bytes_down() -
         free_bytes();
}


inline heap::stats::stats(const stats_data& sd) noexcept
: name(sd.name),
  malloc_calls_(sd.malloc_calls()),
  resize_calls_(sd.resize_calls()),
  free_calls_(sd.free_calls()),
  malloc_bytes_(sd.malloc_bytes()),
  resize_bytes_up_(sd.resize_bytes_up()),
  resize_bytes_down_(sd.resize_bytes_down()),
  free_bytes_(sd.free_bytes()),
  malloc_fail_(sd.malloc_fail()),
  resize_fail_(sd.resize_fail())
{}


}} /* namespace __cxxabiv1::ext */
