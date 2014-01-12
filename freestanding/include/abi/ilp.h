#ifndef ABI_ILP_H
#define ABI_ILP_H


namespace abi {


struct lp64 {
  using size_t = unsigned long;
  using ptrdiff_t = signed long;
  using uintptr_t = unsigned long long;
  using intptr_t = signed long long;
};

struct ilp32 {
  using size_t = unsigned int;
  using ptrdiff_t = signed int;
  using uintptr_t = unsigned int;
  using intptr_t = signed int;
};


#if defined(__LP64__)
using ilp = lp64;
#else  /* XXX find a better way to determine ilp32 archs. */
using ilp = ilp32;
#endif

typedef ilp::size_t size_t;
typedef ilp::ptrdiff_t ptrdiff_t;
typedef ilp::uintptr_t uintptr_t;
typedef ilp::intptr_t intptr_t;


} /* namespace abi */


#endif /* ABI_ILP_H */
