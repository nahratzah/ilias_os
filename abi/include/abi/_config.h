#ifndef _ABI_CONFIG_H_
#define _ABI_CONFIG_H_
/*
 * This header file is used to integrate the abi code with your system.
 *
 * The functions here are to be implemented by whomever handles the
 * integration.
 *
 * Each function has a comment section before it, describing what is required
 * of the implementation.  A comment section after the function, is used to
 * make merging in changes to this file easier.
 */

#include <abi/abi.h>

namespace __cxxabiv1 {
namespace _config {


/*
 * Allocation code.
 *
 * Contains member functions:
 * - malloc: allocate heap memory;
 * - free: release memory allocated by malloc.
 *
 * Note that the heap instance must be initialized globally,
 * may not use thread-local storage and must have a trivial
 * destructor.
 *
 * The code paths in this function may not use exceptions
 * (no try-catch, for instance), as it is used by the
 * exception handling code.
 *
 * See __cxxabiv1::heap for a way to override this function
 * if you want to provide a more powerful interface.
 */
struct heap {
  constexpr heap(const char* name) : name(name) {}
  heap(const heap&) = delete;
  inline void* malloc(size_t) noexcept;
  inline void free(void*) noexcept;

  const char* name;
};


/*
 * Malloc: allocate heap memory.
 *
 * This function is allowed to fail.
 * This function must be thread-safe.
 *
 * The code path used in this function may not use exceptions
 * (no try-catch, for instance).
 */
inline void* heap::malloc(size_t) noexcept {
  return nullptr;
}
/*
 * End of heap::malloc.
 */

/*
 * Free: release memory previously allocated with malloc.
 *
 * This function may never fail.
 * This function must be thread-safe.
 *
 * The code path used in this function may not use exceptions
 * (no try-catch, for instance).
 */
inline void heap::free(void*) noexcept {
  return;
}
/*
 * End of heap::free.
 */

/*
 * Abort: stop execution of the program.
 *
 * This function may not return.
 * This function may not use exceptions.
 * This function must be thread-safe.
 */
inline void abort() noexcept {
  for (;;);
}
/*
 * End of abort.
 */

/*
 * Exit: program requests to stop with given error code.
 *
 * This function may not return.
 * This function may not use exceptions.
 * This function must be thread-safe.
 */
inline void exit(int) noexcept {
  for (;;);
}
/*
 * End of exit.
 */

/*
 * Panic: a function that is invoked when the abi gives up.
 *
 * This function is not allowed to return.
 * This function may not use exceptions.
 *
 * The arguments are a printf-style message.
 *
 * In userspace, you probably want to invoke abi::_config::abort() eventually.
 */
inline void panic(const char*, ...) noexcept {
  abort();
}
/*
 * End of panic.
 */


}} /* namespace __cxxabiv1::_config */

#endif /* _ABI_CONFIG_H_ */
