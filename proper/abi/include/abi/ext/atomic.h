#ifndef _ABI_EXT_ATOMIC_H_
#define _ABI_EXT_ATOMIC_H_

namespace __cxxabiv1 {
namespace ext {
namespace {

/*
 * Implement the pause instruction.
 * The pause instruction informs the CPU that the current CPU will block
 * and allows the CPU to switch to a different CPU (which makes sense if
 * you consider that hyperthreading CPUs share the same execution unit).
 */
inline void pause() noexcept {
#if defined(__i386__) || defined(__amd64__) || defined(__x86_64__)
  asm volatile("pause");
#endif
}

}}} /* namespace __cxxabiv1::ext::<unnamed> */

#endif /* _ABI_EXT_ATOMIC_H_ */
