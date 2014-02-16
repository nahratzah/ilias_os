#ifndef _ABI_HEAP_H_
#define _ABI_HEAP_H_

#include <abi/_config.h>
#include <abi/abi.h>
#include <atomic>

namespace __cxxabiv1 {
namespace ext {


class heap {
 public:
  struct stats {
    const char* name;

    std::atomic<uintmax_t> malloc_calls;  // count: malloc() called
    std::atomic<uintmax_t> realloc_calls;  // count: realloc() called
    std::atomic<uintmax_t> free_calls;  // count: free() called
    std::atomic<uintmax_t> malloc_bytes;  // bytes handed out by malloc()
    std::atomic<uintmax_t> resize_bytes_up;  // bytes grown by resize()
    std::atomic<uintmax_t> resize_bytes_down;  // bytes shrunk by resize()
    std::atomic<uintmax_t> free_bytes;  // bytes freeed by free()
    std::atomic<uintmax_t> malloc_fail;  // count: malloc() returned nullptr
    std::atomic<uintmax_t> resize_fail;  // count: resize() returned false

    stats* chain = nullptr;

    stats(const char* name) noexcept : name(name) {}
  };

  inline heap(const char* name) : stats_(name) {}

  void* malloc(size_t) noexcept;
  void free(void*) noexcept;
  bool resize(void*, size_t, size_t*) noexcept;

 private:
  void* malloc_result(void*, size_t) noexcept;
  bool resize_result(bool, void*, size_t, size_t) noexcept;

  stats stats_;
};


}} /* namespace __cxxabiv1::ext */

#endif /* _ABI_HEAP_H_ */
