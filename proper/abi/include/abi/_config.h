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
#ifdef _LOADER
# include <loader/abi.h>
# include <loader/panic.h>
#endif
#include <tuple>

namespace __cxxabiv1 {


namespace ext {
class heap;  // From abi/ext/heap.h
} /* namespace __cxxabiv1::ext */


namespace _config {


/*
 * Heap_malloc: allocate heap memory.
 *
 * This function is allowed to fail, by returning nullptr.
 * This function must be thread-safe.
 * If the function returns non-null, the size_t argument must
 * be changed to reflect the amount of memory allocated.
 * Memory returned by this function must be aligned to
 * alignof(max_align_t).
 *
 * The code path used in this function may not use exceptions
 * (no try-catch, for instance).
 */
#ifdef _LOADER
using loader::heap_malloc;
#else
inline _namespace(std)::tuple<void*, size_t> heap_malloc(size_t) noexcept {
  return _namespace(std)::make_tuple(nullptr, 0);
}
#endif
/*
 * End of heap_malloc.
 */

/*
 * Heap_free: release memory previously allocated with malloc.
 *
 * This function may fail, by returning false.
 * This function must be thread-safe.
 *
 * The code path used in this function may not use exceptions
 * (no try-catch, for instance).
 */
#ifdef _LOADER
using loader::heap_free;
#else
inline bool heap_free(void* p, size_t sz) noexcept {
  return false;
}
#endif
/*
 * End of heap_free.
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
 * This function may not return.
 * This function may not use exceptions.
 *
 * The arguments are a printf-style message.
 *
 * In userspace, you probably want to invoke abi::_config::abort() eventually.
 */
#ifdef _LOADER
using loader::panic;
#else
inline void panic(const char* fmt, ...) noexcept {
  abort();
}
#endif
/*
 * End of panic.
 */


/*
 * Struct big_heap: an implementation of heap, but with bells, whistles etc.
 * Feel free to go totally crazy with exceptions, TLS, complex constructors...
 * But please keep the destructor trivial/non-existant.
 *
 * Must provide the member functions:
 * - malloc: allocates sz bytes
 * - free: releases storage previously acquired via malloc.
 */
using big_heap = ext::heap;
/*
 * End of struct big_heap.
 */


}} /* namespace __cxxabiv1::_config */

#endif /* _ABI_CONFIG_H_ */
