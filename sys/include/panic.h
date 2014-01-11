#ifndef SYS_PANIC_H
#define SYS_PANIC_H

namespace kernel {


extern "C" void panic(const char*, ...) noexcept __attribute__((__noreturn__));
extern "C" void warn(const char*, ...) noexcept;


}

#endif /* SYS_PANIC_H */
