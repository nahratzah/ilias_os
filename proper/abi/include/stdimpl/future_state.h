#ifndef _STDIMPL_FUTURE_STATE_H_
#define _STDIMPL_FUTURE_STATE_H_

#include <cdecl.h>
#include <atomic>
#include <cstdint>
#include <exception>
#include <mutex>
#include <type_traits>
#include <utility>

_namespace_begin(std)
namespace impl {


class future_state_base
{
 public:
  using callback_t = void (*)(future_state_base*);

  enum class state_t : unsigned char {
    uninitialized,
    uninitialized_deferred,
    initialized_value,
    initialized_exc,
    ready_value,
    ready_exc
  };

  static void acquire(future_state_base*) noexcept;
  static void release(future_state_base*) noexcept;

 protected:
  future_state_base() noexcept = delete;
  future_state_base(const future_state_base&) = delete;
  future_state_base(future_state_base&&) = delete;
  future_state_base& operator=(const future_state_base&) = delete;
  future_state_base& operator=(future_state_base&&) = delete;
  explicit future_state_base(callback_t) noexcept;
  future_state_base(callback_t, callback_t) noexcept;
  ~future_state_base() noexcept = default;

 public:
  state_t get_state() const noexcept;
  bool mark_shared() noexcept;  // Mark fsb shared between promise and future.
  state_t wait();
  template<typename Clock, typename Duration>
  state_t wait_until(const chrono::time_point<Clock, Duration>&);

 protected:
  void start_deferred() noexcept;

 public:
  void lock() noexcept;
  void unlock() noexcept;
  void ensure_uninitialized() const;

 protected:
  void set_ready_val() noexcept;
  void set_ready_exc() noexcept;
  void set_initialized_val() noexcept;
  void set_initialized_exc() noexcept;

 private:
  atomic<state_t> state_;
  atomic<bool> lck_;
  atomic<uintptr_t> refcnt_{ 1U };
  atomic<bool> shared_;

  const callback_t destructor_;
  const callback_t deferred_fn_ = nullptr;
};

struct future_state_ignored_value {};

template<typename T>
class future_state
: public future_state_base
{
 private:
  using T_ = conditional_t<is_void<T>::value, future_state_ignored_value, T>;

 public:
  using state_t = future_state_base::state_t;
  using callback_t = future_state_base::callback_t;

  future_state() noexcept = delete;
  future_state(const future_state&) = delete;
  future_state(future_state&&) = delete;
  future_state& operator=(const future_state&) = delete;
  future_state& operator=(future_state&&) = delete;
  explicit future_state(callback_t) noexcept;
  future_state(callback_t, callback_t) noexcept;
  ~future_state() noexcept;

  T* get();

  template<typename... Args> void set_value(Args&&...);
  template<typename... Args> void init_value(Args&&...);
  void set_exc(exception_ptr ptr);
  void init_exc(exception_ptr ptr);

 private:
  template<typename... Args>
  unique_lock<future_state_base> assign_value_(Args&&...);
  unique_lock<future_state_base> assign_exc_(exception_ptr);

  aligned_union_t<0, T_, exception_ptr> storage_;
};


template<typename T, typename Alloc, typename Fn, typename... Args>
class future_state_fn final
: public future_state<T>
{
  using args_indices = make_index_sequence<1U + sizeof...(Args)>;

 public:
  template<typename... Init>
  future_state_fn(const Alloc&, Init&&...);

 private:
  static void invoke_destructor(future_state_base*) noexcept;
  static void invoke_deferred(future_state_base*) noexcept;
  template<size_t... Idx>
  static void invoke_deferred_(future_state_base*,
                               index_sequence<Idx...>) noexcept;

  std::tuple<Alloc, Fn, Args...> data_;
};

template<typename Alloc, typename Fn, typename... Args>
class future_state_fn<void, Alloc, Fn, Args...> final
: public future_state<void>
{
  using args_indices = make_index_sequence<1U + sizeof...(Args)>;

 public:
  template<typename... Init>
  future_state_fn(const Alloc&, Init&&...);

 private:
  static void invoke_destructor(future_state_base* self) noexcept;
  static void invoke_deferred(future_state_base* self) noexcept;
  template<size_t... Idx>
  static void invoke_deferred_(future_state_base* self,
                               index_sequence<Idx...>) noexcept;

  std::tuple<Alloc, Fn, Args...> data_;
};

template<typename T, typename Alloc>
class future_state_nofn final
: public future_state<T>
{
 public:
  future_state_nofn(const Alloc&);

 private:
  static void invoke_destructor(future_state_base* self) noexcept;

  Alloc alloc_;
};

template<typename>
class future_state_task;

template<typename T, typename... Args>
class future_state_task<T(Args...)>
: public future_state<T>
{
 public:
  using callback_t = future_state_base::callback_t;
  using operation_t = void (*)(future_state_base*, bool, decay_t<Args>...);

  future_state_task(callback_t, operation_t);
  void set_invoke(Args...);
  void init_invoke(Args...);

 private:
  operation_t operation_;
};

template<typename, typename, typename> class future_state_task_impl;

template<typename T, typename Alloc, typename Fn, typename... Args>
class future_state_task_impl<T(Args...), Alloc, Fn> final
: public future_state_task<T(Args...)>
{
 public:
  future_state_task_impl(const Alloc&, Fn&&);

 private:
  static void invoke_destructor(future_state_base*) noexcept;
  static void invoke_operation(future_state_base*, bool, Args...) noexcept;

  tuple<Alloc, Fn> data_;
};


template<typename T, typename Alloc>
future_state<T>* allocate_future_state(const Alloc&);
template<typename T, typename Alloc, typename Fn, typename... Args>
future_state<T>* allocate_future_state(const Alloc&, Fn&&, Args&&...);
template<typename T, typename Alloc, typename Fn>
future_state_task<T>* allocate_future_state_task(const Alloc&, Fn&&);


} /* namespace std::impl */
_namespace_end(std)

#include <stdimpl/future_state-inl.h>

#endif /* _STDIMPL_FUTURE_STATE_H_ */
