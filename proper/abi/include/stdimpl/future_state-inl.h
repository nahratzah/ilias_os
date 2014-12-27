#ifndef _STDIMPL_FUTURE_STATE_INL_H_
#define _STDIMPL_FUTURE_STATE_INL_H_

#include <cdecl.h>
#include <stdimpl/future_state.h>
#include <cassert>
#include <memory>
#include <thread>
#include <utility>
#include <stdimpl/alloc_deleter.h>
#include <stdimpl/invoke.h>

_namespace_begin(std)
namespace impl {


inline auto future_state_base::acquire(future_state_base* p) noexcept -> void {
  if (p) {
    auto old_refcnt = p->refcnt_.fetch_add(1U, memory_order_acquire);
    assert(old_refcnt + 1U != 0U);
  }
}

inline auto future_state_base::release(future_state_base* p) noexcept -> void {
  if (p) {
    auto old_refcnt = p->refcnt_.fetch_sub(1U, memory_order_release);
    assert(old_refcnt != 0U);

    if (old_refcnt == 1U) {
      callback_t destructor = p->destructor_;
      (*destructor)(p);
    }
  }
}

inline future_state_base::future_state_base(callback_t destructor)
    noexcept
: state_(state_t::uninitialized),
  destructor_(destructor)
{
  assert(destructor_ != nullptr);
}

inline future_state_base::future_state_base(callback_t destructor,
                                            callback_t deferred_fn)
    noexcept
: state_(state_t::uninitialized_deferred),
  destructor_(destructor),
  deferred_fn_(deferred_fn)
{
  assert(destructor_ != nullptr);
  assert(deferred_fn_ != nullptr);
}

inline auto future_state_base::get_state() const noexcept -> state_t {
  return state_.load(memory_order_relaxed);
}

inline auto future_state_base::mark_shared() noexcept -> bool {
  return !shared_.exchange(true, memory_order_acquire);
}

inline auto future_state_base::wait() -> state_t {
  start_deferred();

  state_t s;
  for (s = get_state();
       _predict_false(s != state_t::ready_exc &&
                      s != state_t::ready_value);
       s = get_state()) {
    this_thread::yield();
  }
  return s;
}

template<typename Clock, typename Duration>
auto future_state_base::wait_until(
    const chrono::time_point<Clock, Duration>& tp) -> state_t {
  using clock = typename chrono::time_point<Clock, Duration>::clock;

  state_t s;
  for (s = get_state();
       _predict_false(s != state_t::ready_exc &&
                      s != state_t::ready_value &&
                      s != state_t::uninitialized_deferred &&
                      clock::now() < tp);
       s = get_state()) {
    this_thread::yield();
  }
  return s;
}

inline auto future_state_base::start_deferred() noexcept -> void {
  state_t expect = state_t::uninitialized_deferred;
  if (state_.compare_exchange_strong(expect, state_t::uninitialized,
                                     memory_order_acquire,
                                     memory_order_relaxed))
    deferred_fn_(this);
}

inline auto future_state_base::unlock() noexcept -> void {
  bool old_lck = lck_.exchange(false, memory_order_release);
  assert(old_lck == true);
}

inline auto future_state_base::set_ready_val() noexcept -> void {
  auto old_state = state_.exchange(state_t::ready_value, memory_order_release);
  assert(old_state == state_t::uninitialized);
}

inline auto future_state_base::set_ready_exc() noexcept -> void {
  auto old_state = state_.exchange(state_t::ready_exc, memory_order_release);
  assert(old_state == state_t::uninitialized);
}

inline auto future_state_base::set_initialized_val() noexcept -> void {
  auto old_state = state_.exchange(state_t::initialized_value,
                                   memory_order_release);
  assert(old_state == state_t::uninitialized);
}

inline auto future_state_base::set_initialized_exc() noexcept -> void {
  auto old_state = state_.exchange(state_t::initialized_exc,
                                   memory_order_release);
  assert(old_state == state_t::uninitialized);
}


template<typename T>
future_state<T>::future_state(callback_t destructor) noexcept
: future_state_base(destructor)
{}

template<typename T>
future_state<T>::future_state(callback_t destructor, callback_t deferred_fn)
    noexcept
: future_state_base(destructor, deferred_fn)
{}

template<typename T>
future_state<T>::~future_state() noexcept {
  void* storage_ptr = &storage_;

  switch (this->get_state()) {
  case state_t::uninitialized:
  case state_t::uninitialized_deferred:
    break;
  case state_t::initialized_value:
  case state_t::ready_value:
    static_cast<T_*>(storage_ptr)->~T_();
    break;
  case state_t::initialized_exc:
  case state_t::ready_exc:
    static_cast<exception_ptr*>(storage_ptr)->~exception_ptr();
    break;
  }
}

template<typename T>
auto future_state<T>::get() -> T* {
  void* storage_ptr = &storage_;

  switch (this->wait()) {
  case state_t::ready_value:
    return static_cast<T_*>(storage_ptr);
  case state_t::ready_exc:
    rethrow_exception(*static_cast<exception_ptr*>(storage_ptr));
    break;
  default:
    __builtin_unreachable();
  }

  assert_msg(false, "Unreachable code reached.");
  for (;;);
}

template<typename T>
template<typename... Args>
auto future_state<T>::set_value(Args&&... args) -> void {
  auto lck = assign_value_(forward<Args>(args)...);
  set_ready_val();
}

template<typename T>
template<typename... Args>
auto future_state<T>::init_value(Args&&... args) -> void {
  auto lck = assign_value_(forward<Args>(args)...);
  set_initialized_val();
}

template<typename T>
auto future_state<T>::set_exc(exception_ptr ptr) -> void {
  auto lck = assign_exc_(move(ptr));
  set_ready_exc();
}

template<typename T>
auto future_state<T>::init_exc(exception_ptr ptr) -> void {
  auto lck = assign_exc_(move(ptr));
  set_initialized_exc();
}

template<typename T>
template<typename... Args>
auto future_state<T>::assign_value_(Args&&... args) ->
    unique_lock<future_state_base> {
  auto lck = unique_lock<future_state_base>(*this);
  this->ensure_uninitialized();

  void* storage_ptr = &storage_;
  new (storage_ptr) T_(forward<Args>(args)...);
  return lck;
}

template<typename T>
auto future_state<T>::assign_exc_(exception_ptr ptr) ->
    unique_lock<future_state_base> {
  auto lck = unique_lock<future_state_base>(*this);
  this->ensure_uninitialized();

  void* storage_ptr = &storage_;
  new (storage_ptr) exception_ptr(move(ptr));
  return lck;
}


template<typename T, typename Alloc, typename Fn, typename... Args>
template<typename... Init>
future_state_fn<T, Alloc, Fn, Args...>::future_state_fn(const Alloc& alloc,
                                                        Init&&... init)
: future_state<T>(&invoke_destructor, &invoke_deferred),
  data_(alloc, forward<Init>(init)...)
{}

template<typename T, typename Alloc, typename Fn, typename... Args>
auto future_state_fn<T, Alloc, Fn, Args...>::invoke_destructor(
    future_state_base* self) noexcept -> void {
  using alloc = typename allocator_traits<Alloc>::
      template rebind_alloc<future_state_fn>;
  using alloc_traits = typename allocator_traits<Alloc>::
      template rebind_traits<future_state_fn>;

  future_state_fn* self_impl = static_cast<future_state_fn*>(self);

  alloc impl = move_if_noexcept(_namespace(std)::get<0>(self_impl->data_));
  alloc_traits::destroy(impl, self_impl);
  alloc_traits::deallocate(impl, self_impl, 1U);
}

template<typename T, typename Alloc, typename Fn, typename... Args>
auto future_state_fn<T, Alloc, Fn, Args...>::invoke_deferred(
    future_state_base* self) noexcept -> void {
  invoke_deferred_(self, args_indices());
}

template<typename T, typename Alloc, typename Fn, typename... Args>
template<size_t... Idx>
auto future_state_fn<T, Alloc, Fn, Args...>::invoke_deferred_(
    future_state_base* self, index_sequence<Idx...>) noexcept -> void {
  future_state_fn* self_impl = static_cast<future_state_fn*>(self);

  try {
    self_impl->set_value(invoke(
        move(_namespace(std)::get<1 + Idx>(self_impl->data_))...));
  } catch (...) {
    self_impl->set_exc(current_exception());
  }
}


template<typename Alloc, typename Fn, typename... Args>
template<typename... Init>
future_state_fn<void, Alloc, Fn, Args...>::future_state_fn(const Alloc& alloc,
                                                           Init&&... init)
: future_state<void>(&invoke_destructor, &invoke_deferred),
  data_(alloc, forward<Init>(init)...)
{}

template<typename Alloc, typename Fn, typename... Args>
auto future_state_fn<void, Alloc, Fn, Args...>::invoke_destructor(
    future_state_base* self) noexcept -> void {
  using alloc = typename allocator_traits<Alloc>::
      template rebind_alloc<future_state_fn>;
  using alloc_traits = typename allocator_traits<Alloc>::
      template rebind_traits<future_state_fn>;

  future_state_fn* self_impl = static_cast<future_state_fn*>(self);

  alloc impl = move_if_noexcept(_namespace(std)::get<0>(self_impl->data_));
  alloc_traits::destroy(impl, self_impl);
  alloc_traits::deallocate(impl, self_impl, 1U);
}

template<typename Alloc, typename Fn, typename... Args>
auto future_state_fn<void, Alloc, Fn, Args...>::invoke_deferred(
    future_state_base* self) noexcept -> void {
  invoke_deferred_(self, args_indices());
}

template<typename Alloc, typename Fn, typename... Args>
template<size_t... Idx>
auto future_state_fn<void, Alloc, Fn, Args...>::invoke_deferred_(
    future_state_base* self, index_sequence<Idx...>) noexcept -> void {
  future_state_fn* self_impl = static_cast<future_state_fn*>(self);

  try {
    invoke(move(_namespace(std)::get<1 + Idx>(self_impl->data_))...);
    self_impl->set_value();
  } catch (...) {
    self_impl->set_exc(current_exception());
  }
}


template<typename T, typename Alloc>
future_state_nofn<T, Alloc>::future_state_nofn(const Alloc& alloc)
: future_state<T>(&invoke_destructor),
  alloc_(alloc)
{}

template<typename T, typename Alloc>
auto future_state_nofn<T, Alloc>::invoke_destructor(future_state_base* self)
    noexcept -> void {
  using alloc = typename allocator_traits<Alloc>::
      template rebind_alloc<future_state_nofn>;
  using alloc_traits = typename allocator_traits<Alloc>::
      template rebind_traits<future_state_nofn>;

  future_state_nofn* self_impl = static_cast<future_state_nofn*>(self);

  alloc impl = move_if_noexcept(self_impl->alloc_);
  alloc_traits::destroy(impl, self_impl);
  alloc_traits::deallocate(impl, self_impl, 1U);
}


template<typename T, typename... Args>
future_state_task<T(Args...)>::future_state_task(callback_t destructor,
                                                 operation_t op)
: future_state<T>(destructor),
  operation_(op)
{}

template<typename T, typename... Args>
auto future_state_task<T(Args...)>::set_invoke(Args... args) -> void {
  invoke(operation_, this, false, forward<Args>(args)...);
}

template<typename T, typename... Args>
auto future_state_task<T(Args...)>::init_invoke(Args... args) -> void {
  invoke(operation_, this, true, forward<Args>(args)...);
}


template<typename T, typename Alloc, typename Fn, typename... Args>
future_state_task_impl<T(Args...), Alloc, Fn>::future_state_task_impl(
    const Alloc& alloc, Fn&& fn)
: future_state_task<T(Args...)>(&invoke_destructor, &invoke_operation),
  data_(alloc, forward<Fn>(fn))
{}

template<typename T, typename Alloc, typename Fn, typename... Args>
auto future_state_task_impl<T(Args...), Alloc, Fn>::invoke_destructor(
    future_state_base* self) noexcept -> void {
  using alloc = typename allocator_traits<Alloc>::
      template rebind_alloc<future_state_task_impl>;
  using alloc_traits = typename allocator_traits<Alloc>::
      template rebind_traits<future_state_task_impl>;

  future_state_task_impl* self_impl =
      static_cast<future_state_task_impl*>(self);

  alloc impl = move_if_noexcept(_namespace(std)::get<0>(self_impl->data_));
  alloc_traits::destroy(impl, self_impl);
  alloc_traits::deallocate(impl, self_impl, 1U);
}

template<typename T, typename Alloc, typename Fn, typename... Args>
auto future_state_task_impl<T(Args...), Alloc, Fn>::invoke_operation(
    future_state_base* self, bool at_thread_exit, Args... args) noexcept ->
    void {
  future_state_task_impl* self_impl =
      static_cast<future_state_task_impl*>(self);

  if (at_thread_exit) {
    try {
      self_impl->init_value(invoke(_namespace(std)::get<1>(self_impl->data_),
                                   forward<Args>(args)...));
    } catch (...) {
      self_impl->init_exc(current_exception());
    }
  } else {
    try {
      self_impl->set_value(invoke(_namespace(std)::get<1>(self_impl->data_),
                                  forward<Args>(args)...));
    } catch (...) {
      self_impl->set_exc(current_exception());
    }
  }
}


template<typename T, typename Alloc>
auto allocate_future_state(const Alloc& alloc) -> future_state<T>* {
  using void_alloc = typename allocator_traits<Alloc>::
      template rebind_alloc<void>;
  using type = future_state_nofn<T, void_alloc>;
  using alloc_type = typename allocator_traits<Alloc>::
      template rebind_alloc<type>;
  using alloc_traits = typename allocator_traits<Alloc>::
      template rebind_traits<type>;

  alloc_type alloc_impl = alloc;
  return new_alloc_deleter<type>(alloc_impl, nullptr, alloc).release();
}


template<typename T, typename Alloc, typename Fn, typename... Args>
auto allocate_future_state(const Alloc& alloc, Fn&& fn, Args&&... args) ->
    future_state<T>* {
  using void_alloc = typename allocator_traits<Alloc>::
      template rebind_alloc<void>;
  using type = future_state_fn<T, void_alloc, decay_t<Fn>, decay_t<Args>...>;
  using alloc_type = typename allocator_traits<Alloc>::
      template rebind_alloc<type>;
  using alloc_traits = typename allocator_traits<Alloc>::
      template rebind_traits<type>;

  alloc_type alloc_impl = alloc;
  return new_alloc_deleter<type>(alloc_impl, nullptr, alloc,
                                 forward<Fn>(fn),
                                 forward<Args>(args)...).release();
}


template<typename T, typename Alloc, typename Fn>
auto allocate_future_state_task(const Alloc& alloc, Fn&& fn) ->
    future_state_task<T>* {
  using void_alloc = typename allocator_traits<Alloc>::
      template rebind_alloc<void>;
  using type = future_state_task_impl<T, void_alloc, decay_t<Fn>>;
  using alloc_type = typename allocator_traits<Alloc>::
      template rebind_alloc<type>;
  using alloc_traits = typename allocator_traits<Alloc>::
      template rebind_traits<type>;

  alloc_type alloc_impl = alloc;
  return new_alloc_deleter<type>(alloc_impl, nullptr, alloc,
                                 forward<Fn>(fn)).release();
}


} /* namespace std::impl */
_namespace_end(std)

#endif /* _STDIMPL_FUTURE_STATE_INL_H_ */
