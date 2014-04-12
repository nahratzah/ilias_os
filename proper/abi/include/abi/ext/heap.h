#ifndef _ABI_HEAP_H_
#define _ABI_HEAP_H_

#include <abi/_config.h>
#include <abi/abi.h>
#include <abi/ext/list.h>
#include <atomic>
#include <iterator>
#include <string>
#include <tuple>
#include <vector>

namespace __cxxabiv1 {
namespace ext {


class heap {
 private:
  class stats_data : public list_elem<void> {
   public:
    friend heap;

    const _namespace(std)::string_ref name;

    stats_data(_namespace(std)::string_ref name) noexcept : name(name) {};

    uintmax_t malloc_calls() const noexcept;
    uintmax_t resize_calls() const noexcept;
    uintmax_t free_calls() const noexcept;
    uintmax_t malloc_bytes() const noexcept;
    uintmax_t resize_bytes_up() const noexcept;
    uintmax_t resize_bytes_down() const noexcept;
    uintmax_t free_bytes() const noexcept;
    uintmax_t malloc_fail() const noexcept;
    uintmax_t resize_fail() const noexcept;

   private:
    using atomic_uintmax_t = _namespace(std)::atomic<uintmax_t>;

    atomic_uintmax_t malloc_calls_;  // count: malloc() called
    atomic_uintmax_t resize_calls_;  // count: resize() called
    atomic_uintmax_t free_calls_;  // count: free() called
    atomic_uintmax_t malloc_bytes_;  // bytes handed out by malloc()
    atomic_uintmax_t resize_bytes_up_;  // bytes grown by resize()
    atomic_uintmax_t resize_bytes_down_;  // bytes shrunk by resize()
    atomic_uintmax_t free_bytes_;  // bytes freeed by free()
    atomic_uintmax_t malloc_fail_;  // count: malloc() returned nullptr
    atomic_uintmax_t resize_fail_;  // count: resize() returned false
  };

 public:
  struct stats {
   public:
    stats() = default;
    stats(const stats&) = default;
    stats& operator=(const stats&) = default;
    stats(const stats_data&) noexcept;

    _namespace(std)::string_ref name;

    uintmax_t malloc_calls() const noexcept;
    uintmax_t resize_calls() const noexcept;
    uintmax_t free_calls() const noexcept;
    uintmax_t malloc_bytes() const noexcept;
    uintmax_t resize_bytes_up() const noexcept;
    uintmax_t resize_bytes_down() const noexcept;
    uintmax_t free_bytes() const noexcept;
    uintmax_t malloc_fail() const noexcept;
    uintmax_t resize_fail() const noexcept;
    uintmax_t used() const noexcept;

   private:
    uintmax_t malloc_calls_ = 0;
    uintmax_t resize_calls_ = 0;
    uintmax_t free_calls_ = 0;
    uintmax_t malloc_bytes_ = 0;
    uintmax_t resize_bytes_up_ = 0;
    uintmax_t resize_bytes_down_ = 0;
    uintmax_t free_bytes_ = 0;
    uintmax_t malloc_fail_ = 0;
    uintmax_t resize_fail_ = 0;
  };

  using stats_collection = _namespace(std)::vector<stats>;

  heap(_namespace(std)::string_ref) noexcept;
  heap(const heap&) = delete;
  heap& operator=(const heap&) = delete;
  ~heap() noexcept;

  void* malloc(size_t) noexcept;
  void* malloc(size_t, size_t) noexcept;
  void free(const void*) noexcept;
  void free(const void* p, size_t) noexcept { free(p); }
  _namespace(std)::tuple<bool, size_t> resize(const void*, size_t) noexcept;

  static stats_collection get_stats(stats_collection);
  static stats_collection get_stats();

 private:
  void* malloc_result(void*, size_t) noexcept;
  _namespace(std)::tuple<bool, size_t> resize_result(
      _namespace(std)::tuple<bool, size_t>,
      _namespace(std)::tuple<const void*, size_t>) noexcept;
  void free_result(size_t, const void*) noexcept;

  stats_data stats_;

  struct all_stats;
};


}} /* namespace __cxxabiv1::ext */

#include <abi/ext/heap-inl.h>

#endif /* _ABI_HEAP_H_ */
