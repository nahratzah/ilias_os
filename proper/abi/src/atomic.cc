#define _ATOMIC_INLINE	/* Compile here. */
#include <atomic>
#include <cdecl.h>
#include <abi/abi.h>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <array>
#include <functional>
#include <type_traits>
#include <utility>
#include <abi/ext/atomic.h>

/*
 * Fallback for non-lock-free atomics.
 */
#pragma redefine_extname __atomic_load_c __atomic_load
#pragma redefine_extname __atomic_store_c __atomic_store
#pragma redefine_extname __atomic_exchange_c __atomic_exchange
#pragma redefine_extname __atomic_compare_exchange_c __atomic_compare_exchange
#pragma redefine_extname __atomic_is_lock_free_c __atomic_is_lock_free

using _namespace(std)::memory_order_acquire;
using _namespace(std)::memory_order_release;
using _namespace(std)::memory_order_relaxed;
using _namespace(std)::memory_order_seq_cst;
using _namespace(std)::enable_if_t;
using _namespace(std)::forward;

namespace {

constexpr _namespace(std)::size_t N_SPL = 512;
using lock_t = _Atomic(_namespace(std)::uintptr_t);

/* Align locks to minimize the number of TLB entries required to access it. */
struct alignas(sizeof(lock_t) * N_SPL) locks_array
: _namespace(std)::array<lock_t, N_SPL>
{};

/* These locks are in .BSS segment, so they are initially zeroed. */
locks_array locks;

lock_t& allocate_lock(const void* p) noexcept {
  _namespace(std)::size_t h =
      _namespace(std)::hash<_namespace(std)::uintptr_t>()(
          reinterpret_cast<_namespace(std)::uintptr_t>(p) / 16U);
  return locks[h % locks.size()];
}

void lock(lock_t& l) noexcept {
  _namespace(std)::uintptr_t expect = 0;
  while (!__c11_atomic_compare_exchange_weak(&l, &expect, 1,
                                             memory_order_acquire,
                                             memory_order_relaxed)) {
    expect = 0;
    abi::ext::pause();
  }
}

void unlock(lock_t& l) noexcept {
  __c11_atomic_store(&l, 0, memory_order_release);
}

template<_namespace(std)::size_t sz>
struct is_lock_free_t
: _namespace(std)::false_type
{};

template<>
struct is_lock_free_t<1>
: _namespace(std)::integral_constant<bool, __c11_atomic_is_lock_free(1)>
{};

template<>
struct is_lock_free_t<2>
: _namespace(std)::integral_constant<bool, __c11_atomic_is_lock_free(2)>
{};

template<>
struct is_lock_free_t<4>
: _namespace(std)::integral_constant<bool, __c11_atomic_is_lock_free(4)>
{};

template<>
struct is_lock_free_t<8>
: _namespace(std)::integral_constant<bool, __c11_atomic_is_lock_free(8)>
{};

#if _USE_INT128 && defined(__amd64__) || defined(__x86_64__)
template<>
struct is_lock_free_t<16> {
  bool operator()() const noexcept {
    uint32_t c;
    asm("cpuid"
    :   "=c"(c)
    :   "a"(1), "c"(0)
    :   "eax", "ebx", "edx");
    return (c & (1U << 13));  // CPUID feature: cmpxchg16b.
  }

  operator bool() const noexcept {
    return (*this)();
  }
};
#endif  // _USE_INT128 && defined(__amd64__) || defined(__x86_64__)


/* Fallback case for maybe_lockfree. */
template<typename T, typename = int>
struct maybe_lockfree {
  static constexpr bool load(_namespace(std)::size_t, const void*, void*, int)
      noexcept {
    return false;
  }

  static constexpr bool store(_namespace(std)::size_t, void*, const void*, int)
      noexcept {
    return false;
  }

  static bool constexpr exchange(_namespace(std)::size_t, void*,
      const void*, void*, int)
      noexcept {
    return false;
  }

  static bool constexpr cmp_exchange(_namespace(std)::size_t, void*,
                                     void*, const void*,
                                     int, int, bool*) noexcept {
    return false;
  }

  static constexpr bool fetch_add(T*, T, int, T*) noexcept {
    return false;
  }

  static constexpr bool fetch_sub(T*, T, int, T*) noexcept {
    return false;
  }

  static constexpr bool fetch_and(T*, T, int, T*) noexcept {
    return false;
  }

  static constexpr bool fetch_or(T*, T, int, T*) noexcept {
    return false;
  }

  static constexpr bool fetch_xor(T*, T, int, T*) noexcept {
    return false;
  }

  static constexpr bool is_lock_free(_namespace(std)::size_t, const void*)
      noexcept {
    return false;
  }
};

/* uint128 specialization for maybe_lockfree. */
#if _USE_INT128 && (defined(__amd64__) || defined(__x86_64__))
# define MAYBE_OP_128(invocation)					\
  (maybe_lockfree<uint128_t>::invocation)

template<>
struct maybe_lockfree<uint128_t, int> {
  static bool load(_namespace(std)::size_t sz, const void* atom, void* dst,
                   int) noexcept {
    if (!is_lock_free(sz, atom)) return false;

    uint128_t* dst_ = static_cast<uint128_t*>(dst);

    uint64_t rdx = 0;
    uint64_t rax = 0;
    asm("mov %%rax, %%rbx\n"
      "\tmov %%rdx, %%rcx\n"
      "\tlock cmpxchg16b %2"
    : "=d"(rdx), "=a"(rax)
    : "m"(*static_cast<const uint128_t*>(atom))
    : "cc", "rcx", "rbx");
    *dst_ = ((uint128_t(rdx) << 64) | rax);
    return true;
  }

  static bool store(_namespace(std)::size_t sz, void* atom, const void* src,
                    int) noexcept {
    if (!is_lock_free(sz, atom)) return false;

    const uint128_t* src_ = static_cast<const uint128_t*>(src);

    uint64_t rcx = *src_ >> 64;
    uint64_t rbx = *src_;
    asm volatile("\n"
      "0:\n"
      "\tlock cmpxchg16b %0\n"
      "\tjne 0b\n"
    :
    : "m"(*static_cast<uint128_t*>(atom)), "c"(rcx), "b"(rbx)
    : "cc", "rax", "rdx", "memory");
    return true;
  }

  static bool exchange(_namespace(std)::size_t sz, void* atom,
                       const void* src, void* dst,
                       int) noexcept {
    if (!is_lock_free(sz, atom)) return false;

    const uint128_t* src_ = static_cast<const uint128_t*>(src);
    uint128_t* dst_ = static_cast<uint128_t*>(dst);

    uint64_t rdx;
    uint64_t rax;
    uint64_t rcx = *src_ >> 64;
    uint64_t rbx = *src_;
    asm volatile("\n"
      "0:\n"
      "\tlock cmpxchg16b %2\n"
      "\tjne 0b\n"
    : "=d"(rdx), "=a"(rax)
    : "m"(*static_cast<uint128_t*>(atom)), "c"(rcx), "b"(rbx)
    : "cc", "memory");
    *dst_ = ((uint128_t(rdx) << 64) | rax);
    return true;
  }

  static bool cmp_exchange(_namespace(std)::size_t sz, void* atom,
                           void* expect, const void* desired,
                           int, int, bool* rv) noexcept {
    if (!is_lock_free(sz, atom)) return false;

    uint128_t* expect_ = static_cast<uint128_t*>(expect);
    const uint128_t* desired_ = static_cast<const uint128_t*>(desired);
    uint64_t rdx = *expect_ >> 64;
    uint64_t rax = *expect_;
    uint64_t rcx = *desired_ >> 64;
    uint64_t rbx = *desired_;
    static_assert(sizeof(bool) == sizeof(char),
                  "Assembler below depends on this, to set *rv.");
    asm volatile(
        "lock cmpxchg16b %3\n"  // set ZF if equal, clear ZF if not equal
      "\tsete %2\n"
    : "=d"(rdx), "=a"(rax), "=X"(*rv)
    : "m"(*static_cast<uint128_t*>(atom)),
      "d"(rdx), "a"(rax), "c"(rcx), "b"(rbx)
    : "cc", "memory");
    if (!*rv)
      *expect_ = ((uint128_t(rdx) << 64) | rax);
    return true;
  }

  static bool fetch_add(uint128_t* atom, uint128_t src, int,
                        uint128_t* dst) noexcept {
    if (!is_lock_free(sizeof(*atom), atom)) return false;

    uint64_t rdx;
    uint64_t rax;
    uint64_t rcx = src >> 64;
    uint64_t rbx = src;
    asm volatile("\n"
      "0:\n"
      "\tmov %%rdx, %%rcx\n"
      "\tmov %%rax, %%rbx\n"
      "\tadd %4, %%rbx\n"
      "\tadc %3, %%rcx\n"
      "\tcmpxchg16b %2\n"
      "\tjne 0b"
    : "=a"(rax), "=d"(rdx)
    : "m"(*atom), "X"(rcx), "X"(rbx)
    : "cc", "memory", "rcx", "rbx");
    *dst = ((uint128_t(rdx) << 64) | rax);
    return true;
  }

  static bool fetch_sub(uint128_t* atom, uint128_t src, int,
                        uint128_t* dst) noexcept {
    if (!is_lock_free(sizeof(*atom), atom)) return false;

    uint64_t rdx;
    uint64_t rax;
    uint64_t rcx = src >> 64;
    uint64_t rbx = src;
    asm volatile("\n"
      "0:\n"
      "\tmov %%rdx, %%rcx\n"
      "\tmov %%rax, %%rbx\n"
      "\tsub %4, %%rbx\n"
      "\tsbb %3, %%rcx\n"
      "\tcmpxchg16b %2\n"
      "\tjne 0b"
    : "=a"(rax), "=d"(rdx)
    : "m"(*atom), "X"(rcx), "X"(rbx)
    : "cc", "memory", "rcx", "rbx");
    *dst = ((uint128_t(rdx) << 64) | rax);
    return true;
  }

  static bool fetch_and(uint128_t* atom, uint128_t src, int,
                        uint128_t* dst) noexcept {
    if (!is_lock_free(sizeof(*atom), atom)) return false;

    uint64_t rdx;
    uint64_t rax;
    uint64_t rcx = src >> 64;
    uint64_t rbx = src;
    asm volatile("\n"
      "0:\n"
      "\tmov %3, %%rcx\n"
      "\tmov %4, %%rbx\n"
      "\tand %%rdx, %%rcx\n"
      "\tand %%rax, %%rbx\n"
      "\tcmpxchg16b %2\n"
      "\tjne 0b"
    : "=a"(rax), "=d"(rdx)
    : "m"(*atom), "X"(rcx), "X"(rbx)
    : "cc", "memory", "rcx", "rbx");
    *dst = ((uint128_t(rdx) << 64) | rax);
    return true;
  }

  static bool fetch_or(uint128_t* atom, uint128_t src, int,
                       uint128_t* dst) noexcept {
    if (!is_lock_free(sizeof(*atom), atom)) return false;

    uint64_t rdx;
    uint64_t rax;
    uint64_t rcx = src >> 64;
    uint64_t rbx = src;
    asm volatile("\n"
      "0:\n"
      "\tmov %3, %%rcx\n"
      "\tmov %4, %%rbx\n"
      "\tor %%rdx, %%rcx\n"
      "\tor %%rax, %%rbx\n"
      "\tcmpxchg16b %2\n"
      "\tjne 0b"
    : "=a"(rax), "=d"(rdx)
    : "m"(*atom), "X"(rcx), "X"(rbx)
    : "cc", "memory", "rcx", "rbx");
    *dst = ((uint128_t(rdx) << 64) | rax);
    return true;
  }

  static bool fetch_xor(uint128_t* atom, uint128_t src, int,
                        uint128_t* dst) noexcept {
    if (!is_lock_free(sizeof(*atom), atom)) return false;

    uint64_t rdx;
    uint64_t rax;
    uint64_t rcx = src >> 64;
    uint64_t rbx = src;
    asm volatile("\n"
      "0:\n"
      "\tmov %3, %%rcx\n"
      "\tmov %4, %%rbx\n"
      "\txor %%rdx, %%rcx\n"
      "\txor %%rax, %%rbx\n"
      "\tcmpxchg16b %2\n"
      "\tjne 0b"
    : "=a"(rax), "=d"(rdx)
    : "m"(*atom), "X"(rcx), "X"(rbx)
    : "cc", "memory", "rcx", "rbx");
    *dst = ((uint128_t(rdx) << 64) | rax);
    return true;
  }

  static bool is_lock_free(_namespace(std)::size_t sz, const void* atom)
      noexcept {
    return is_lock_free_t<sizeof(uint128_t)>() &&
           sz == sizeof(uint128_t) &&
           reinterpret_cast<_namespace(std)::uintptr_t>(atom) % 16 == 0;
  }
};
#else // _USE_INT128
# define MAYBE_OP_128(invocation)  (false)
#endif

/* Implementation for guaranteed lockfree code. */
template<typename T>
struct maybe_lockfree<T, enable_if_t<is_lock_free_t<sizeof(T)>::value, int>> {
  static bool load(_namespace(std)::size_t sz, const void* atom, void* dst,
                   int model) noexcept {
    if (sz != sizeof(T)) return false;

    T* dst_ = reinterpret_cast<T*>(dst);
    _Atomic(T)* atom_ = reinterpret_cast<_Atomic(T)*>(const_cast<void*>(atom));

    *dst_ = __c11_atomic_load(atom_, model);
    return true;
  }

  static bool store(_namespace(std)::size_t sz, void* atom, const void* src,
                    int model) noexcept {
    if (sz != sizeof(T)) return false;

    const T* src_ = reinterpret_cast<const T*>(src);
    _Atomic(T)* atom_ = reinterpret_cast<_Atomic(T)*>(atom);

    __c11_atomic_store(atom_, *src_, model);
    return true;
  }

  static bool exchange(_namespace(std)::size_t sz, void* atom,
                       const void* src, void* dst,
                       int model) noexcept {
    if (sz != sizeof(T)) return false;

    const T* src_ = reinterpret_cast<const T*>(src);
    T* dst_ = reinterpret_cast<T*>(dst);
    _Atomic(T)* atom_ = reinterpret_cast<_Atomic(T)*>(atom);

    *dst_ = __c11_atomic_exchange(atom_, *src_, model);
    return true;
  }

  static bool cmp_exchange(_namespace(std)::size_t sz, void* atom,
                           void* expect, const void* desired,
                           int succes, int fail, bool* rv) noexcept {
    if (sz != sizeof(T)) return false;

    const T* desired_ = reinterpret_cast<const T*>(desired);
    T* expect_ = reinterpret_cast<T*>(expect);
    _Atomic(T)* atom_ = reinterpret_cast<_Atomic(T)*>(atom);

    *rv = __c11_atomic_compare_exchange_strong(atom_, expect_, *desired_,
                                               succes, fail);
    return true;
  }

  static bool fetch_add(T* atom, T src, int model, T* dst) noexcept {
    _Atomic(T)* atom_ = reinterpret_cast<_Atomic(T)*>(atom);

    *dst = __c11_atomic_fetch_add(atom_, src, model);
    return true;
  }

  static bool fetch_sub(T* atom, T src, int model, T* dst) noexcept {
    _Atomic(T)* atom_ = reinterpret_cast<_Atomic(T)*>(atom);

    *dst = __c11_atomic_fetch_sub(atom_, src, model);
    return true;
  }

  static bool fetch_and(T* atom, T src, int model, T* dst) noexcept {
    _Atomic(T)* atom_ = reinterpret_cast<_Atomic(T)*>(atom);

    *dst = __c11_atomic_fetch_and(atom_, src, model);
    return true;
  }

  static bool fetch_or(T* atom, T src, int model, T* dst) noexcept {
    _Atomic(T)* atom_ = reinterpret_cast<_Atomic(T)*>(atom);

    *dst = __c11_atomic_fetch_or(atom_, src, model);
    return true;
  }

  static bool fetch_xor(T* atom, T src, int model, T* dst) noexcept {
    _Atomic(T)* atom_ = reinterpret_cast<_Atomic(T)*>(atom);

    *dst = __c11_atomic_fetch_xor(atom_, src, model);
    return true;
  }

  static bool is_lock_free(_namespace(std)::size_t sz, const void*) noexcept {
    return (sz == sizeof(T));
  }
};


/*
 * Tries to invoke invocation.
 *
 * Evaluates to true if succesful, false otherwise.
 */
#define MAYBE_OP(invocation)						\
  (									\
    (maybe_lockfree<uint8_t>::invocation) ||				\
    (maybe_lockfree<uint16_t>::invocation) ||				\
    (maybe_lockfree<uint32_t>::invocation) ||				\
    (maybe_lockfree<uint64_t>::invocation) ||				\
    (MAYBE_OP_128(invocation))						\
  )


/* Handle code requiring locking. */
template<typename Fn, typename... Args>
void do_locked(const void* atom, Fn fn, Args&&... args) noexcept {
  auto& lck = allocate_lock(atom);
  lock(lck);
  fn(forward<Args>(args)...);
  unlock(lck);
}


template<typename T>
T fetch_add(T* atom, T src, int model) noexcept {
  T rv;
  if (!maybe_lockfree<T>::fetch_add(atom, src, model, &rv)) {
    do_locked(atom,
              [](T* atom, T src, T* rv) {
                *rv = *atom;
                *atom = *rv + src;
              },
              atom, src, &rv);
  }
  return rv;
}

template<typename T>
T fetch_sub(T* atom, T src, int model) noexcept {
  T rv;
  if (!maybe_lockfree<T>::fetch_sub(atom, src, model, &rv)) {
    do_locked(atom,
              [](T* atom, T src, T* rv) {
                *rv = *atom;
                *atom = *rv - src;
              },
              atom, src, &rv);
  }
  return rv;
}

template<typename T>
T fetch_and(T* atom, T src, int model) noexcept {
  T rv;
  if (!maybe_lockfree<T>::fetch_and(atom, src, model, &rv)) {
    do_locked(atom,
              [](T* atom, T src, T* rv) {
                *rv = *atom;
                *atom = (*rv & src);
              },
              atom, src, &rv);
  }
  return rv;
}

template<typename T>
T fetch_or(T* atom, T src, int model) noexcept {
  T rv;
  if (!maybe_lockfree<T>::fetch_or(atom, src, model, &rv)) {
    do_locked(atom,
              [](T* atom, T src, T* rv) {
                *rv = *atom;
                *atom = (*rv | src);
              },
              atom, src, &rv);
  }
  return rv;
}

template<typename T>
T fetch_xor(T* atom, T src, int model) noexcept {
  T rv;
  if (!maybe_lockfree<T>::fetch_xor(atom, src, model, &rv)) {
    do_locked(atom,
              [](T* atom, T src, T* rv) {
                *rv = *atom;
                *atom = (*rv ^ src);
              },
              atom, src, &rv);
  }
  return rv;
}


} /* namespace <unnamed> */


_cdecl_begin

void __atomic_load_c(_namespace(std)::size_t sz, const void* atom, void* dst,
                     int model) noexcept {
  if (!MAYBE_OP(load(sz, atom, dst, model)))
    do_locked(atom, &memcpy, dst, atom, sz);
}

uint8_t __atomic_load_1(const uint8_t* atom, int model) noexcept {
  uint8_t dst;
  if (!maybe_lockfree<uint8_t>::load(sizeof(*atom), atom, &dst, model)) {
    do_locked(atom,
              [](uint8_t* dst, const uint8_t* src) {
                *dst = *src;
              },
              &dst, atom);
  }
  return dst;
}
uint16_t __atomic_load_2(const uint16_t* atom, int model) noexcept {
  uint16_t dst;
  if (!maybe_lockfree<uint16_t>::load(sizeof(*atom), atom, &dst, model)) {
    do_locked(atom,
              [](uint16_t* dst, const uint16_t* src) {
                *dst = *src;
              },
              &dst, atom);
  }
  return dst;
}
uint32_t __atomic_load_4(const uint32_t* atom, int model) noexcept {
  uint32_t dst;
  if (!maybe_lockfree<uint32_t>::load(sizeof(*atom), atom, &dst, model)) {
    do_locked(atom,
              [](uint32_t* dst, const uint32_t* src) {
                *dst = *src;
              },
              &dst, atom);
  }
  return dst;
}
uint64_t __atomic_load_8(const uint64_t* atom, int model) noexcept {
  uint64_t dst;
  if (!maybe_lockfree<uint64_t>::load(sizeof(*atom), atom, &dst, model)) {
    do_locked(atom,
              [](uint64_t* dst, const uint64_t* src) {
                *dst = *src;
              },
              &dst, atom);
  }
  return dst;
}
#if _USE_INT128
uint128_t __atomic_load_16(const uint128_t* atom, int model) noexcept {
  uint128_t dst;
  if (!maybe_lockfree<uint128_t>::load(sizeof(*atom), atom, &dst, model)) {
    do_locked(atom,
              [](uint128_t* dst, const uint128_t* src) {
                *dst = *src;
              },
              &dst, atom);
  }
  return dst;
}
#endif


void __atomic_store_c(_namespace(std)::size_t sz, void* atom, const void* src,
                      int model)
    noexcept {
  if (!MAYBE_OP(store(sz, atom, src, model)))
    do_locked(atom, &memcpy, atom, src, sz);
}

void __atomic_store_1(uint8_t* atom, const uint8_t src, int model) noexcept {
  if (!maybe_lockfree<uint8_t>::store(sizeof(*atom), atom, &src, model)) {
    do_locked(atom,
              [](uint8_t* dst, const uint8_t* src) {
                *dst = *src;
              },
              atom, &src);
  }
}
void __atomic_store_2(uint16_t* atom, const uint16_t src, int model) noexcept {
  if (!maybe_lockfree<uint16_t>::store(sizeof(*atom), atom, &src, model)) {
    do_locked(atom,
              [](uint16_t* dst, const uint16_t* src) {
                *dst = *src;
              },
              atom, &src);
  }
}
void __atomic_store_4(uint32_t* atom, const uint32_t src, int model) noexcept {
  if (!maybe_lockfree<uint32_t>::store(sizeof(*atom), atom, &src, model)) {
    do_locked(atom,
              [](uint32_t* dst, const uint32_t* src) {
                *dst = *src;
              },
              atom, &src);
  }
}
void __atomic_store_8(uint64_t* atom, const uint64_t src, int model) noexcept {
  if (!maybe_lockfree<uint64_t>::store(sizeof(*atom), atom, &src, model)) {
    do_locked(atom,
              [](uint64_t* dst, const uint64_t* src) {
                *dst = *src;
              },
              atom, &src);
  }
}
#if _USE_INT128
void __atomic_store_16(uint128_t* atom, const uint128_t src, int model)
    noexcept {
  if (!maybe_lockfree<uint128_t>::store(sizeof(*atom), atom, &src, model)) {
    do_locked(atom,
              [](uint128_t* dst, const uint128_t* src) {
                *dst = *src;
              },
              atom, &src);
  }
}
#endif


void __atomic_exchange_c(_namespace(std)::size_t sz, void* atom,
                         const void* src, void* dst,
                         int model) noexcept {
  if (!MAYBE_OP(exchange(sz, atom, src, dst, model))) {
    do_locked(atom,
              [](void* atom, const void* src, void* dst,
                 _namespace(std)::size_t sz) {
                memcpy(dst, atom, sz);
                memcpy(atom, src, sz);
              },
              atom, src, dst, sz);
  }
}

uint8_t __atomic_exchange_1(uint8_t* atom, uint8_t src, int model) noexcept {
  uint8_t rv;
  if (!maybe_lockfree<uint8_t>::exchange(sizeof(*atom), atom, &src, &rv, model)) {
    do_locked(atom,
              [](uint8_t* atom, const uint8_t* src, uint8_t* dst) {
                *dst = *atom;
                *atom = *src;
              },
              atom, &src, &rv);
  }
  return rv;
}
uint16_t __atomic_exchange_2(uint16_t* atom, uint16_t src, int model)
    noexcept {
  uint16_t rv;
  if (!maybe_lockfree<uint16_t>::exchange(sizeof(*atom), atom, &src, &rv, model)) {
    do_locked(atom,
              [](uint16_t* atom, const uint16_t* src, uint16_t* dst) {
                *dst = *atom;
                *atom = *src;
              },
              atom, &src, &rv);
  }
  return rv;
}
uint32_t __atomic_exchange_4(uint32_t* atom, uint32_t src, int model)
    noexcept {
  uint32_t rv;
  if (!maybe_lockfree<uint32_t>::exchange(sizeof(*atom), atom, &src, &rv, model)) {
    do_locked(atom,
              [](uint32_t* atom, const uint32_t* src, uint32_t* dst) {
                *dst = *atom;
                *atom = *src;
              },
              atom, &src, &rv);
  }
  return rv;
}
uint64_t __atomic_exchange_8(uint64_t* atom, uint64_t src, int model)
    noexcept {
  uint64_t rv;
  if (!maybe_lockfree<uint64_t>::exchange(sizeof(*atom), atom, &src, &rv, model)) {
    do_locked(atom,
              [](uint64_t* atom, const uint64_t* src, uint64_t* dst) {
                *dst = *atom;
                *atom = *src;
              },
              atom, &src, &rv);
  }
  return rv;
}
#if _USE_INT128
uint128_t __atomic_exchange_16(uint128_t* atom, uint128_t src, int model)
    noexcept {
  uint128_t rv;
  if (!maybe_lockfree<uint128_t>::exchange(sizeof(*atom), atom, &src, &rv, model)) {
    do_locked(atom,
              [](uint128_t* atom, const uint128_t* src, uint128_t* dst) {
                *dst = *atom;
                *atom = *src;
              },
              atom, &src, &rv);
  }
  return rv;
}
#endif


int __atomic_compare_exchange_c(_namespace(std)::size_t sz, void* atom,
                                void* expect, const void* desired,
                                int succes, int fail) noexcept {
  bool rv;
  if (!MAYBE_OP(cmp_exchange(sz, atom, expect, desired, succes, fail, &rv))) {
    do_locked(atom,
              [](void* atom, void* expect, const void* desired, int sz,
                 bool* rv) {
                *rv = (memcmp(atom, expect, sz) == 0);
                if (*rv)
                  memcpy(atom, desired, sz);
                else
                  memcpy(expect, atom, sz);
              },
              atom, expect, desired, sz, &rv);
  }

  return (rv ? 1 : 0);
}

int __atomic_compare_exchange_1(uint8_t* atom,
                                uint8_t* expect, uint8_t desired,
                                int succes, int fail) noexcept {
  bool rv;
  if (!maybe_lockfree<uint8_t>::cmp_exchange(sizeof(*atom), atom,
                                             expect, &desired, succes, fail,
                                             &rv)) {
    do_locked(atom,
              [](uint8_t* atom, uint8_t* expect, const uint8_t* desired,
                 bool* rv) {
                *rv = (atom == expect);
                if (*rv)
                  *atom = *desired;
                else
                  *expect = *atom;
              },
              atom, expect, &desired, &rv);
  }
  return (rv ? 1 : 0);
}
int __atomic_compare_exchange_2(uint16_t* atom,
                                uint16_t* expect, uint16_t desired,
                                int succes, int fail) noexcept {
  bool rv;
  if (!maybe_lockfree<uint16_t>::cmp_exchange(sizeof(*atom), atom,
                                              expect, &desired, succes, fail,
                                              &rv)) {
    do_locked(atom,
              [](uint16_t* atom, uint16_t* expect, const uint16_t* desired,
                 bool* rv) {
                *rv = (atom == expect);
                if (*rv)
                  *atom = *desired;
                else
                  *expect = *atom;
              },
              atom, expect, &desired, &rv);
  }
  return (rv ? 1 : 0);
}
int __atomic_compare_exchange_4(uint32_t* atom,
                                uint32_t* expect, uint32_t desired,
                                int succes, int fail) noexcept {
  bool rv;
  if (!maybe_lockfree<uint32_t>::cmp_exchange(sizeof(*atom), atom,
                                              expect, &desired, succes, fail,
                                              &rv)) {
    do_locked(atom,
              [](uint32_t* atom, uint32_t* expect, const uint32_t* desired,
                 bool* rv) {
                *rv = (atom == expect);
                if (*rv)
                  *atom = *desired;
                else
                  *expect = *atom;
              },
              atom, expect, &desired, &rv);
  }
  return (rv ? 1 : 0);
}
int __atomic_compare_exchange_8(uint64_t* atom,
                                uint64_t* expect, uint64_t desired,
                                int succes, int fail) noexcept {
  bool rv;
  if (!maybe_lockfree<uint64_t>::cmp_exchange(sizeof(*atom), atom,
                                              expect, &desired, succes, fail,
                                              &rv)) {
    do_locked(atom,
              [](uint64_t* atom, uint64_t* expect, const uint64_t* desired,
                 bool* rv) {
                *rv = (atom == expect);
                if (*rv)
                  *atom = *desired;
                else
                  *expect = *atom;
              },
              atom, expect, &desired, &rv);
  }
  return (rv ? 1 : 0);
}
#if _USE_INT128
int __atomic_compare_exchange_16(uint128_t* atom,
                                 uint128_t* expect, uint128_t desired,
                                 int succes, int fail) noexcept {
  bool rv;
  if (!maybe_lockfree<uint128_t>::cmp_exchange(sizeof(*atom), atom,
                                               expect, &desired, succes, fail,
                                               &rv)) {
    do_locked(atom,
              [](uint128_t* atom, uint128_t* expect, const uint128_t* desired,
                 bool* rv) {
                *rv = (atom == expect);
                if (*rv)
                  *atom = *desired;
                else
                  *expect = *atom;
              },
              atom, expect, &desired, &rv);
  }
  return (rv ? 1 : 0);
}
#endif


uint8_t __atomic_fetch_add_1(uint8_t* atom, uint8_t src, int model) {
  return fetch_add(atom, src, model);
}
uint16_t __atomic_fetch_add_2(uint16_t* atom, uint16_t src, int model) {
  return fetch_add(atom, src, model);
}
uint32_t __atomic_fetch_add_4(uint32_t* atom, uint32_t src, int model) {
  return fetch_add(atom, src, model);
}
uint64_t __atomic_fetch_add_8(uint64_t* atom, uint64_t src, int model) {
  return fetch_add(atom, src, model);
}
#if _USE_INT128
uint128_t __atomic_fetch_add_16(uint128_t* atom, uint128_t src, int model) {
  return fetch_add(atom, src, model);
}
#endif

uint8_t __atomic_fetch_sub_1(uint8_t* atom, uint8_t src, int model) {
  return fetch_sub(atom, src, model);
}
uint16_t __atomic_fetch_sub_2(uint16_t* atom, uint16_t src, int model) {
  return fetch_sub(atom, src, model);
}
uint32_t __atomic_fetch_sub_4(uint32_t* atom, uint32_t src, int model) {
  return fetch_sub(atom, src, model);
}
uint64_t __atomic_fetch_sub_8(uint64_t* atom, uint64_t src, int model) {
  return fetch_sub(atom, src, model);
}
#if _USE_INT128
uint128_t __atomic_fetch_sub_16(uint128_t* atom, uint128_t src, int model) {
  return fetch_sub(atom, src, model);
}
#endif

uint8_t __atomic_fetch_and_1(uint8_t* atom, uint8_t src, int model) {
  return fetch_and(atom, src, model);
}
uint16_t __atomic_fetch_and_2(uint16_t* atom, uint16_t src, int model) {
  return fetch_and(atom, src, model);
}
uint32_t __atomic_fetch_and_4(uint32_t* atom, uint32_t src, int model) {
  return fetch_and(atom, src, model);
}
uint64_t __atomic_fetch_and_8(uint64_t* atom, uint64_t src, int model) {
  return fetch_and(atom, src, model);
}
#if _USE_INT128
uint128_t __atomic_fetch_and_16(uint128_t* atom, uint128_t src, int model) {
  return fetch_and(atom, src, model);
}
#endif

uint8_t __atomic_fetch_or_1(uint8_t* atom, uint8_t src, int model) {
  return fetch_or(atom, src, model);
}
uint16_t __atomic_fetch_or_2(uint16_t* atom, uint16_t src, int model) {
  return fetch_or(atom, src, model);
}
uint32_t __atomic_fetch_or_4(uint32_t* atom, uint32_t src, int model) {
  return fetch_or(atom, src, model);
}
uint64_t __atomic_fetch_or_8(uint64_t* atom, uint64_t src, int model) {
  return fetch_or(atom, src, model);
}
#if _USE_INT128
uint128_t __atomic_fetch_or_16(uint128_t* atom, uint128_t src, int model) {
  return fetch_or(atom, src, model);
}
#endif

uint8_t __atomic_fetch_xor_1(uint8_t* atom, uint8_t src, int model) {
  return fetch_xor(atom, src, model);
}
uint16_t __atomic_fetch_xor_2(uint16_t* atom, uint16_t src, int model) {
  return fetch_xor(atom, src, model);
}
uint32_t __atomic_fetch_xor_4(uint32_t* atom, uint32_t src, int model) {
  return fetch_xor(atom, src, model);
}
uint64_t __atomic_fetch_xor_8(uint64_t* atom, uint64_t src, int model) {
  return fetch_xor(atom, src, model);
}
#if _USE_INT128
uint128_t __atomic_fetch_xor_16(uint128_t* atom, uint128_t src, int model) {
  return fetch_xor(atom, src, model);
}
#endif

int __atomic_is_lock_free_c(_namespace(std)::size_t sz, const void* atom)
    noexcept {
  return (MAYBE_OP(is_lock_free(sz, atom)) ? 1 : 0);
}


#if _USE_INT128
#pragma redefine_extname __sync_fetch_and_add_16_c __sync_fetch_and_add_16
uint128_t __sync_fetch_and_add_16_c(uint128_t* atom, uint128_t src, ...) {
  return __atomic_fetch_add_16(atom, src, memory_order_seq_cst);
}

#pragma redefine_extname __sync_fetch_and_sub_16_c __sync_fetch_and_sub_16
uint128_t __sync_fetch_and_sub_16_c(uint128_t* atom, uint128_t src, ...) {
  return __atomic_fetch_sub_16(atom, src, memory_order_seq_cst);
}

#pragma redefine_extname __sync_fetch_and_and_16_c __sync_fetch_and_and_16
uint128_t __sync_fetch_and_and_16_c(uint128_t* atom, uint128_t src, ...) {
  return __atomic_fetch_and_16(atom, src, memory_order_seq_cst);
}

#pragma redefine_extname __sync_fetch_and_or_16_c __sync_fetch_and_or_16
uint128_t __sync_fetch_and_or_16_c(uint128_t* atom, uint128_t src, ...) {
  return __atomic_fetch_or_16(atom, src, memory_order_seq_cst);
}

#pragma redefine_extname __sync_fetch_and_xor_16_c __sync_fetch_and_xor_16
uint128_t __sync_fetch_and_xor_16_c(uint128_t* atom, uint128_t src, ...) {
  return __atomic_fetch_xor_16(atom, src, memory_order_seq_cst);
}

#pragma redefine_extname __sync_lock_test_and_set_16_c __sync_lock_test_and_set_16
uint128_t __sync_lock_test_and_set_16_c(uint128_t* atom, uint128_t src, ...) {
  return __atomic_exchange_16(atom, src, memory_order_acquire);
}

#pragma redefine_extname __sync_lock_release_16_c __sync_lock_release_16
void __sync_lock_release_16_c(uint128_t* atom, ...) {
  __atomic_store_16(atom, 0, memory_order_release);
}

#pragma redefine_extname __sync_val_compare_and_swap_16_c __sync_val_compare_and_swap_16
uint128_t __sync_val_compare_and_swap_16_c(uint128_t* atom,
                                           uint128_t oldval, uint128_t newval,
                                           ...) {
  __atomic_compare_exchange_16(atom, &oldval, newval,
                               memory_order_seq_cst, memory_order_seq_cst);
  return oldval;
}

#pragma redefine_extname __sync_bool_compare_and_swap_16_c __sync_bool_compare_and_swap_16
bool __sync_bool_compare_and_swap_16_c(uint128_t* atom,
                                       uint128_t oldval, uint128_t newval,
                                      ...) {
  return __atomic_compare_exchange_16(atom, &oldval, newval,
                                      memory_order_seq_cst,
                                      memory_order_seq_cst);
}
#endif


_cdecl_end
