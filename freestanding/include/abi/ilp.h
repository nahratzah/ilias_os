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
#elif defined(__ILP32__)
using ilp = ilp32;
#endif

using ilp::size_t;
using ilp::ptrdiff_t;
using ilp::uintptr_t;
using ilp::intptr_t;


} /* namespace abi */


#endif /* ABI_ILP_H */