#ifndef _ATOMIC_INLINE
# define _ATOMIC_INLINE extern inline
#endif

_namespace_begin(std)


#ifdef __cplusplus
template<typename T> T kill_dependancy(T y) noexcept { return y; }

namespace impl {

constexpr bool is_weaker(memory_order a, memory_order b) {
  return int(a) < int(b);
}
constexpr bool suitable_for_write(memory_order mo) {
  return mo != memory_order_consume &&
         mo != memory_order_acquire &&
         mo != memory_order_acq_rel;
}
constexpr bool suitable_for_read(memory_order mo) {
  return mo != memory_order_release &&
         mo != memory_order_acq_rel;
}
constexpr bool suitable_for_rw(memory_order mo) {
  return true;
}
constexpr memory_order derive_from_succes(memory_order mo) {
  memory_order rv = mo;
  if (mo == memory_order_acq_rel) mo = memory_order_acquire;
  if (mo == memory_order_release) mo = memory_order_relaxed;
  return mo;
}

} /* namespace std::impl */


/* Instantiate all generic methods on atomic. */
#define _ATOMIC_GENERIC(Template, T)					\
Template								\
constexpr atomic<T>::atomic(T v) noexcept : v_(v) {}			\
									\
Template								\
auto atomic<T>::is_lock_free() const noexcept -> bool {			\
  return __c11_atomic_is_lock_free(sizeof(v_));				\
}									\
Template								\
auto atomic<T>::is_lock_free() const volatile noexcept -> bool {	\
  return __c11_atomic_is_lock_free(sizeof(v_));				\
}									\
									\
Template								\
auto atomic<T>::store(T v, memory_order mo) volatile noexcept -> void {	\
  assert(impl::suitable_for_write(mo));					\
  __c11_atomic_store(&v_, v, mo);					\
}									\
Template								\
auto atomic<T>::store(T v, memory_order mo) noexcept -> void {		\
  assert(impl::suitable_for_write(mo));					\
  __c11_atomic_store(&v_, v, mo);					\
}									\
									\
Template								\
auto atomic<T>::operator=(T v) volatile noexcept -> T {			\
  store(v);								\
  return v;								\
}									\
Template								\
auto atomic<T>::operator=(T v) noexcept -> T {				\
  store(v);								\
  return v;								\
}									\
									\
Template								\
auto atomic<T>::load(memory_order mo) const volatile noexcept -> T {	\
  assert(impl::suitable_for_read(mo));					\
  return __c11_atomic_load(&v_, mo);					\
}									\
Template								\
auto atomic<T>::load(memory_order mo) const noexcept -> T {		\
  assert(impl::suitable_for_read(mo));					\
  return __c11_atomic_load(&v_, mo);					\
}									\
									\
Template								\
atomic<T>::operator T () const volatile noexcept {			\
  return load();							\
}									\
Template								\
atomic<T>::operator T () const noexcept {				\
  return load();							\
}									\
									\
Template								\
auto atomic<T>::exchange(T v, memory_order mo) volatile noexcept -> T {	\
  assert(impl::suitable_for_rw(mo));					\
  return __c11_atomic_exchange(&v_, v, mo);				\
}									\
Template								\
auto atomic<T>::exchange(T v, memory_order mo) noexcept -> T {		\
  assert(impl::suitable_for_rw(mo));					\
  return __c11_atomic_exchange(&v_, v, mo);				\
}									\
									\
Template								\
auto atomic<T>::compare_exchange_weak(T& expect, T v,			\
                                      memory_order succes,		\
                                      memory_order fail)		\
    volatile noexcept -> bool {						\
  assert(impl::suitable_for_rw(succes));				\
  assert(impl::suitable_for_read(fail));				\
  assert(!impl::is_weaker(succes, fail));				\
  return __c11_atomic_compare_exchange_weak(&v_, &expect, v,		\
                                            succes, fail);		\
}									\
Template								\
auto atomic<T>::compare_exchange_weak(T& expect, T v,			\
                                      memory_order succes,		\
                                      memory_order fail)		\
    noexcept -> bool {							\
  assert(impl::suitable_for_rw(succes));				\
  assert(impl::suitable_for_read(fail));				\
  assert(!impl::is_weaker(succes, fail));				\
  return __c11_atomic_compare_exchange_weak(&v_, &expect, v,		\
                                            succes, fail);		\
}									\
									\
Template								\
auto atomic<T>::compare_exchange_strong(T& expect, T v,			\
                                        memory_order succes,		\
                                        memory_order fail)		\
    volatile noexcept -> bool {						\
  assert(impl::suitable_for_rw(succes));				\
  assert(impl::suitable_for_read(fail));				\
  assert(!impl::is_weaker(succes, fail));				\
  return __c11_atomic_compare_exchange_strong(&v_, &expect, v,		\
                                              succes, fail);		\
}									\
Template								\
auto atomic<T>::compare_exchange_strong(T& expect, T v,			\
                                        memory_order succes,		\
                                        memory_order fail)		\
    noexcept -> bool {							\
  assert(impl::suitable_for_rw(succes));				\
  assert(impl::suitable_for_read(fail));				\
  assert(!impl::is_weaker(succes, fail));				\
  return __c11_atomic_compare_exchange_strong(&v_, &expect, v,		\
                                              succes, fail);		\
}									\
									\
Template								\
auto atomic<T>::compare_exchange_weak(T& expect, T v,			\
                                      memory_order succes)		\
    volatile noexcept -> bool {						\
  memory_order fail = impl::derive_from_succes(succes);			\
  return compare_exchange_weak(expect, move(v), succes, fail);		\
}									\
Template								\
auto atomic<T>::compare_exchange_weak(T& expect, T v,			\
                                      memory_order succes)		\
    noexcept -> bool {							\
  memory_order fail = impl::derive_from_succes(succes);			\
  return compare_exchange_weak(expect, move(v), succes, fail);		\
}									\
									\
Template								\
auto atomic<T>::compare_exchange_strong(T& expect, T v,			\
                                        memory_order succes)		\
    volatile noexcept -> bool {						\
  memory_order fail = impl::derive_from_succes(succes);			\
  return compare_exchange_strong(expect, move(v), succes, fail);	\
}									\
Template								\
auto atomic<T>::compare_exchange_strong(T& expect, T v,			\
                                        memory_order succes)		\
    noexcept -> bool {							\
  memory_order fail = impl::derive_from_succes(succes);			\
  return compare_exchange_strong(expect, move(v), succes, fail);	\
}									\

/* All arithmatic operations for atomic. */
#define _ATOMIC_ARITHMATIC(Template, T)					\
Template								\
auto atomic<T>::fetch_add(__diff v, memory_order mo)			\
    volatile noexcept -> T {						\
  assert(impl::suitable_for_rw(mo));					\
  return __c11_atomic_fetch_add(&v_, v, mo);				\
}									\
Template								\
auto atomic<T>::fetch_add(__diff v, memory_order mo) noexcept -> T {	\
  assert(impl::suitable_for_rw(mo));					\
  return __c11_atomic_fetch_add(&v_, v, mo);				\
}									\
									\
Template								\
auto atomic<T>::fetch_sub(__diff v, memory_order mo)			\
    volatile noexcept -> T {						\
  assert(impl::suitable_for_rw(mo));					\
  return __c11_atomic_fetch_sub(&v_, v, mo);				\
}									\
Template								\
auto atomic<T>::fetch_sub(__diff v, memory_order mo) noexcept -> T {	\
  assert(impl::suitable_for_rw(mo));					\
  return __c11_atomic_fetch_sub(&v_, v, mo);				\
}									\
									\
Template								\
auto atomic<T>::operator++(int) volatile noexcept -> T {		\
  return fetch_add(1);							\
}									\
Template								\
auto atomic<T>::operator++(int) noexcept -> T {				\
  return fetch_add(1);							\
}									\
									\
Template								\
auto atomic<T>::operator--(int) volatile noexcept -> T {		\
  return fetch_sub(1);							\
}									\
Template								\
auto atomic<T>::operator--(int) noexcept -> T {				\
  return fetch_sub(1);							\
}									\
									\
Template								\
auto atomic<T>::operator++() volatile noexcept -> T {			\
  auto rv = fetch_add(1);						\
  return ++rv;								\
}									\
Template								\
auto atomic<T>::operator++() noexcept -> T {				\
  auto rv = fetch_add(1);						\
  return ++rv;								\
}									\
									\
Template								\
auto atomic<T>::operator--() volatile noexcept -> T {			\
  auto rv = fetch_sub(1);						\
  return --rv;								\
}									\
Template								\
auto atomic<T>::operator--() noexcept -> T {				\
  auto rv = fetch_sub(1);						\
  return --rv;								\
}									\
									\
Template								\
auto atomic<T>::operator+=(__diff v) volatile noexcept -> T {		\
  return fetch_add(v);							\
}									\
Template								\
auto atomic<T>::operator+=(__diff v) noexcept -> T {			\
  return fetch_add(v);							\
}									\
									\
Template								\
auto atomic<T>::operator-=(__diff v) volatile noexcept -> T {		\
  return fetch_sub(v);							\
}									\
Template								\
auto atomic<T>::operator-=(__diff v) noexcept -> T {			\
  return fetch_sub(v);							\
}									\

/* All logic operations for atomic. */
#define _ATOMIC_LOGIC(Template, T)					\
Template								\
auto atomic<T>::fetch_and(T v, memory_order mo)				\
    volatile noexcept -> T {						\
  assert(impl::suitable_for_rw(mo));					\
  return __c11_atomic_fetch_and(&v_, v, mo);				\
}									\
Template								\
auto atomic<T>::fetch_and(T v, memory_order mo) noexcept -> T {		\
  assert(impl::suitable_for_rw(mo));					\
  return __c11_atomic_fetch_and(&v_, v, mo);				\
}									\
									\
Template								\
auto atomic<T>::fetch_or(T v, memory_order mo)				\
    volatile noexcept -> T {						\
  assert(impl::suitable_for_rw(mo));					\
  return __c11_atomic_fetch_or(&v_, v, mo);				\
}									\
Template								\
auto atomic<T>::fetch_or(T v, memory_order mo) noexcept -> T {		\
  assert(impl::suitable_for_rw(mo));					\
  return __c11_atomic_fetch_or(&v_, v, mo);				\
}									\
									\
Template								\
auto atomic<T>::fetch_xor(T v, memory_order mo)				\
    volatile noexcept -> T {						\
  assert(impl::suitable_for_rw(mo));					\
  return __c11_atomic_fetch_xor(&v_, v, mo);				\
}									\
Template								\
auto atomic<T>::fetch_xor(T v, memory_order mo) noexcept -> T {		\
  assert(impl::suitable_for_rw(mo));					\
  return __c11_atomic_fetch_xor(&v_, v, mo);				\
}									\
									\
Template								\
auto atomic<T>::operator&=(T v) volatile noexcept -> T {		\
  return fetch_and(v);							\
}									\
Template								\
auto atomic<T>::operator&=(T v) noexcept -> T {				\
  return fetch_and(v);							\
}									\
									\
Template								\
auto atomic<T>::operator|=(T v) volatile noexcept -> T {		\
  return fetch_or(v);							\
}									\
Template								\
auto atomic<T>::operator|=(T v) noexcept -> T {				\
  return fetch_or(v);							\
}									\
									\
Template								\
auto atomic<T>::operator^=(T v) volatile noexcept -> T {		\
  return fetch_xor(v);							\
}									\
Template								\
auto atomic<T>::operator^=(T v) noexcept -> T {				\
  return fetch_xor(v);							\
}									\


/* Implement generic atomic:  template<typename T> std::atomic */
_ATOMIC_GENERIC(template<typename T>, T)

/* Implement pointer atomic:  template<typename T> std::atomic<T*> */
_ATOMIC_GENERIC(template<typename T>, T*)
_ATOMIC_ARITHMATIC(template<typename T>, T*)

/* Generate integral atomic:  template<> std::atomic< integral-type > */
#define _ATOMIC_INTEGRAL(Type)						\
	_ATOMIC_GENERIC(_ATOMIC_INLINE, Type)				\
	_ATOMIC_ARITHMATIC(_ATOMIC_INLINE, Type)			\
	_ATOMIC_LOGIC(_ATOMIC_INLINE, Type)

_ATOMIC_INTEGRAL(char)
_ATOMIC_INTEGRAL(signed char)
_ATOMIC_INTEGRAL(unsigned char)
_ATOMIC_INTEGRAL(wchar_t)
_ATOMIC_INTEGRAL(short)
_ATOMIC_INTEGRAL(unsigned short)
_ATOMIC_INTEGRAL(int)
_ATOMIC_INTEGRAL(unsigned int)
_ATOMIC_INTEGRAL(long)
_ATOMIC_INTEGRAL(unsigned long)
_ATOMIC_INTEGRAL(long long)
_ATOMIC_INTEGRAL(unsigned long long)

#undef _ATOMIC_GENERIC
#undef _ATOMIC_ARITHMATIC
#undef _ATOMIC_LOGIC
#undef _ATOMIC_INTEGRAL


_ATOMIC_INLINE void atomic_thread_fence(memory_order mo) noexcept {
  __c11_atomic_thread_fence(mo);
}

_ATOMIC_INLINE void atomic_signal_fence(memory_order mo) noexcept {
  __c11_atomic_signal_fence(mo);
}

template<typename T> void atomic_init(volatile std::atomic<T>* object,
                                      T desired) noexcept {
  __c11_atomic_init(&object->v_, desired);
}
template<typename T> void atomic_init(std::atomic<T>* object,
                                      T desired) noexcept {
  __c11_atomic_init(&object->v_, desired);
}

#endif /* __cplusplus */

_namespace_end(std)

#undef _ATOMIC_INLINE
