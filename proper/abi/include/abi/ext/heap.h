#ifndef _ABI_EXT_HEAP_H_
#define _ABI_EXT_HEAP_H_

#include <abi/abi.h>
#include <string>
#include <tuple>
#include <ilias/stats.h>

namespace __cxxabiv1 {
namespace ext {


class heap {
 private:
  class stats_data {
   private:
    using stats_counter = _namespace(ilias)::stats_counter;

   public:
    stats_data(_namespace(std)::string_ref name) noexcept;

    _namespace(ilias)::stats_group group;

    stats_counter malloc_calls;  // count: malloc() called
    stats_counter resize_calls;  // count: resize() called
    stats_counter free_calls;  // count: free() called
    stats_counter malloc_bytes;  // bytes handed out by malloc()
    stats_counter resize_bytes_up;  // bytes grown by resize()
    stats_counter resize_bytes_down;  // bytes shrunk by resize()
    stats_counter free_bytes;  // bytes freeed by free()
    stats_counter malloc_fail;  // count: malloc() returned nullptr
    stats_counter resize_fail;  // count: resize() returned false
  };

 public:
  heap(_namespace(std)::string_ref) noexcept;
  heap(const heap&) = delete;
  heap& operator=(const heap&) = delete;
  ~heap() noexcept;

  void* malloc(size_t) noexcept;
  void* malloc(size_t, size_t) noexcept;
  void free(const void*) noexcept;
  void free(const void* p, size_t) noexcept { free(p); }
  _namespace(std)::tuple<bool, size_t> resize(const void*, size_t) noexcept;

 private:
  void* malloc_result(void*, size_t) noexcept;
  _namespace(std)::tuple<bool, size_t> resize_result(
      _namespace(std)::tuple<bool, size_t>,
      _namespace(std)::tuple<const void*, size_t>) noexcept;
  void free_result(size_t, const void*) noexcept;

  stats_data stats_;
};


}} /* namespace __cxxabiv1::ext */

#include <abi/ext/heap-inl.h>

#endif /* _ABI_EXT_HEAP_H_ */
