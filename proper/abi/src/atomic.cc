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

using std::memory_order_acquire;
using std::memory_order_release;
using std::memory_order_relaxed;
using std::enable_if_t;
using std::forward;

namespace {

constexpr size_t N_SPL = 512;
using lock_t = _Atomic(uintptr_t);

/* Align locks to minimize the number of TLB entries required to access it. */
struct alignas(sizeof(lock_t) * N_SPL) locks_array
: _namespace(std)::array<lock_t, N_SPL>
{};

/* These locks are in .BSS segment, so they are initially zeroed. */
locks_array locks;

lock_t& allocate_lock(const void* p) noexcept {
  size_t h = _namespace(std)::hash<uintptr_t>()(
      reinterpret_cast<uintptr_t>(p) / 16U);
  return locks[h % locks.size()];
}

void lock(lock_t& l) noexcept {
  uintptr_t expect = 0;
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

template<size_t sz>
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
    uint32_t d;
    asm("cpuid"
    :   "=d"(d)
    :   "a"(1), "d"(0)
    :   "eax", "ebx", "ecx");
    return (d & (1U << 8));  // CPUID feature: cx8.
  }

  operator bool() const noexcept {
    return (*this)();
  }
};
#endif  // _USE_INT128 && defined(__amd64__) || defined(__x86_64__)


/* Fallback case for maybe_lockfree. */
template<typename T, typename = int>
struct maybe_lockfree {
  static constexpr bool load(int, const void*, void*, int) noexcept {
    return false;
  }

  static constexpr bool store(int, void*, const void*, int) noexcept {
    return false;
  }

  static bool constexpr exchange(int, void*, const void*, void*, int)
      noexcept {
    return false;
  }

  static bool constexpr cmp_exchange(int, void*, void*, const void*, int, int,
                                     bool*) noexcept {
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
};

/* uint128 specialization for maybe_lockfree. */
#if 0 // _USE_INT128
# define MAYBE_OP_128(invocation)					\
  (maybe_lockfree<uint128_t>::invocation)

template<>
struct maybe_lockfree<uint128_t, int> {
  static bool load(int sz, const void* atom, void* dst, int model) noexcept {
    if (sz != sizeof(uint128_t) || !is_lock_free_t<sizeof(uint128_t)>())
      return false;
    ...
  }

  static bool store(int sz, void* atom, const void* src, int model) noexcept {
    if (sz != sizeof(uint128_t) || !is_lock_free_t<sizeof(uint128_t)>())
      return false;
    ...
  }

  static bool exchange(int sz, void* atom, const void* src, void* dst,
                       int model) noexcept {
    if (sz != sizeof(uint128_t) || !is_lock_free_t<sizeof(uint128_t)>())
      return false;
    ...
  }

  static bool cmp_exchange(int sz, void* atom,
                           void* expect, const void* desired,
                           int succes, int fail, bool* rv) noexcept {
    if (sz != sizeof(uint128_t) || !is_lock_free_t<sizeof(uint128_t)>())
      return false;
    ...
  }

  static bool fetch_add(T* atom, T src, int model, T* dst) noexcept {
    if (!is_lock_free_t<sizeof(uint128_t)>()) return false;
    ...
  }

  static bool fetch_sub(T* atom, T src, int model, T* dst) noexcept {
    if (!is_lock_free_t<sizeof(uint128_t)>()) return false;
    ...
  }

  static bool fetch_and(T* atom, T src, int model, T* dst) noexcept {
    if (!is_lock_free_t<sizeof(uint128_t)>()) return false;
    ...
  }

  static bool fetch_or(T* atom, T src, int model, T* dst) noexcept {
    if (!is_lock_free_t<sizeof(uint128_t)>()) return false;
    ...
  }

  static bool fetch_xor(T* atom, T src, int model, T* dst) noexcept {
    if (!is_lock_free_t<sizeof(uint128_t)>()) return false;
    ...
  }
};
#else // _USE_INT128
# define MAYBE_OP_128(invocation)  (false)
#endif

/* Implementation for guaranteed lockfree code. */
template<typename T>
struct maybe_lockfree<T, enable_if_t<is_lock_free_t<sizeof(T)>::value, int>> {
  static bool load(int sz, const void* atom, void* dst, int model) noexcept {
    if (sz != sizeof(T)) return false;

    T* dst_ = reinterpret_cast<T*>(dst);
    _Atomic(T)* atom_ = reinterpret_cast<_Atomic(T)*>(const_cast<void*>(atom));

    *dst_ = __c11_atomic_load(atom_, model);
    return true;
  }

  static bool store(int sz, void* atom, const void* src, int model) noexcept {
    if (sz != sizeof(T)) return false;

    const T* src_ = reinterpret_cast<const T*>(src);
    _Atomic(T)* atom_ = reinterpret_cast<_Atomic(T)*>(atom);

    __c11_atomic_store(atom_, *src_, model);
    return true;
  }

  static bool exchange(int sz, void* atom, const void* src, void* dst,
                       int model) noexcept {
    if (sz != sizeof(T)) return false;

    const T* src_ = reinterpret_cast<const T*>(src);
    T* dst_ = reinterpret_cast<T*>(dst);
    _Atomic(T)* atom_ = reinterpret_cast<_Atomic(T)*>(atom);

    *dst_ = __c11_atomic_exchange(atom_, *src_, model);
    return true;
  }

  static bool cmp_exchange(int sz, void* atom,
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

void __atomic_load_c(int sz, const void* atom, void* dst, int model) noexcept {
  if (!MAYBE_OP(load(sz, atom, dst, model)))
    do_locked(atom, &memcpy, dst, atom, sz);
}

uint8_t __atomic_load_1(const uint8_t* atom, int model) noexcept {
  uint8_t dst;
  __atomic_load_c(sizeof(*atom), atom, &dst, model);
  return dst;
}
uint16_t __atomic_load_2(const uint16_t* atom, int model) noexcept {
  uint16_t dst;
  __atomic_load_c(sizeof(*atom), atom, &dst, model);
  return dst;
}
uint32_t __atomic_load_4(const uint32_t* atom, int model) noexcept {
  uint32_t dst;
  __atomic_load_c(sizeof(*atom), atom, &dst, model);
  return dst;
}
uint64_t __atomic_load_8(const uint64_t* atom, int model) noexcept {
  uint64_t dst;
  __atomic_load_c(sizeof(*atom), atom, &dst, model);
  return dst;
}
#if _USE_INT128
uint128_t __atomic_load_16(const uint128_t* atom, int model) noexcept {
  uint128_t dst;
  __atomic_load_c(sizeof(*atom), atom, &dst, model);
  return dst;
}
#endif


void __atomic_store_c(int sz, void* atom, const void* src, int model)
    noexcept {
  if (!MAYBE_OP(store(sz, atom, src, model)))
    do_locked(atom, &memcpy, atom, src, sz);
}

void __atomic_store_1(uint8_t* atom, const uint8_t src, int model) noexcept {
  __atomic_store_c(sizeof(*atom), atom, &src, model);
}
void __atomic_store_2(uint16_t* atom, const uint16_t src, int model) noexcept {
  __atomic_store_c(sizeof(*atom), atom, &src, model);
}
void __atomic_store_4(uint32_t* atom, const uint32_t src, int model) noexcept {
  __atomic_store_c(sizeof(*atom), atom, &src, model);
}
void __atomic_store_8(uint64_t* atom, const uint64_t src, int model) noexcept {
  __atomic_store_c(sizeof(*atom), atom, &src, model);
}
#if _USE_INT128
void __atomic_store_16(uint128_t* atom, const uint128_t src, int model)
    noexcept {
  __atomic_store_c(sizeof(*atom), atom, &src, model);
}
#endif


void __atomic_exchange_c(int sz, void* atom, const void* src, void* dst,
                         int model) noexcept {
  if (!MAYBE_OP(exchange(sz, atom, src, dst, model))) {
    do_locked(atom,
              [](void* atom, const void* src, void* dst, size_t sz) {
                memcpy(dst, atom, sz);
                memcpy(atom, src, sz);
              },
              atom, src, dst, sz);
  }
}

uint8_t __atomic_exchange_1(uint8_t* atom, uint8_t src, int model) noexcept {
  uint8_t rv;
  __atomic_exchange_c(sizeof(*atom), atom, &src, &rv, model);
  return rv;
}
uint16_t __atomic_exchange_2(uint16_t* atom, uint16_t src, int model)
    noexcept {
  uint16_t rv;
  __atomic_exchange_c(sizeof(*atom), atom, &src, &rv, model);
  return rv;
}
uint32_t __atomic_exchange_4(uint32_t* atom, uint32_t src, int model)
    noexcept {
  uint32_t rv;
  __atomic_exchange_c(sizeof(*atom), atom, &src, &rv, model);
  return rv;
}
uint64_t __atomic_exchange_8(uint64_t* atom, uint64_t src, int model)
    noexcept {
  uint64_t rv;
  __atomic_exchange_c(sizeof(*atom), atom, &src, &rv, model);
  return rv;
}
#if _USE_INT128
uint128_t __atomic_exchange_16(uint128_t* atom, uint128_t src, int model)
    noexcept {
  uint128_t rv;
  __atomic_exchange_c(sizeof(*atom), atom, &src, &rv, model);
  return rv;
}
#endif


int __atomic_compare_exchange_c(int sz, void* atom,
                                void* expect, const void* desired,
                                int succes, int fail) noexcept {
  bool rv = 0;
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

int __atomic_compare_excange_1(uint8_t* atom, uint8_t* expect, uint8_t desired,
                               int succes, int fail) noexcept {
  return __atomic_compare_exchange_c(sizeof(*atom), atom, expect, &desired,
                                     succes, fail);
}
int __atomic_compare_excange_2(uint16_t* atom,
                               uint16_t* expect, uint16_t desired,
                               int succes, int fail) noexcept {
  return __atomic_compare_exchange_c(sizeof(*atom), atom, expect, &desired,
                                     succes, fail);
}
int __atomic_compare_excange_4(uint32_t* atom,
                               uint32_t* expect, uint32_t desired,
                               int succes, int fail) noexcept {
  return __atomic_compare_exchange_c(sizeof(*atom), atom, expect, &desired,
                                     succes, fail);
}
int __atomic_compare_excange_8(uint64_t* atom,
                               uint64_t* expect, uint64_t desired,
                               int succes, int fail) noexcept {
  return __atomic_compare_exchange_c(sizeof(*atom), atom, expect, &desired,
                                     succes, fail);
}
#if _USE_INT128
int __atomic_compare_excange_16(uint128_t* atom,
                                uint128_t* expect, uint128_t desired,
                                int succes, int fail) noexcept {
  return __atomic_compare_exchange_c(sizeof(*atom), atom, expect, &desired,
                                     succes, fail);
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

_cdecl_end
