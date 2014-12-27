#ifndef _FUTURE_INL_H_
#define _FUTURE_INL_H_

#include <cdecl.h>
#include <future>
#include <memory>
#include <utility>

_namespace_begin(std)


constexpr launch operator|(launch x, launch y) noexcept {
  using inttype = underlying_type<launch>::type;

  return static_cast<launch>(static_cast<inttype>(x) |
                             static_cast<inttype>(y));
}

constexpr launch operator&(launch x, launch y) noexcept {
  using inttype = underlying_type<launch>::type;

  return static_cast<launch>(static_cast<inttype>(x) &
                             static_cast<inttype>(y));
}

constexpr launch operator^(launch x, launch y) noexcept {
  using inttype = underlying_type<launch>::type;

  return static_cast<launch>(static_cast<inttype>(x) ^
                             static_cast<inttype>(y));
}

constexpr launch operator~(launch x) noexcept {
  using inttype = underlying_type<launch>::type;

  return static_cast<launch>(~static_cast<inttype>(x));
}


inline error_code make_error_code(future_errc e) noexcept {
  return error_code(static_cast<int>(e), future_category());
}

inline error_condition make_error_condition(future_errc e) noexcept {
  return error_condition(static_cast<int>(e), future_category());
}


inline auto future_error::code() const noexcept -> const error_code& {
  return ec_;
}


template<typename R>
promise<R>::promise()
: promise(allocator_arg, allocator<void>())
{}

template<typename R>
template<typename Alloc>
promise<R>::promise(allocator_arg_t, const Alloc& alloc)
: state_(impl::allocate_future_state<R>(alloc))
{}

template<typename R>
promise<R>::promise(promise&& p) noexcept
: state_(exchange(p.state_, nullptr))
{}

template<typename R>
promise<R>::~promise() noexcept {
  impl::future_state_base::release(state_);
}

template<typename R>
auto promise<R>::operator=(promise&& p) noexcept -> promise& {
  promise(std::move(p)).swap(*this);
  return *this;
}

template<typename R>
auto promise<R>::swap(promise& p) noexcept -> void {
  using _namespace(std)::swap;
  swap(state_, p.state_);
}

template<typename R>
auto promise<R>::get_future() -> future<R> {
  if (!state_)  // no state
    future_error::__throw(future_errc::no_state);
  if (!state_->mark_shared())  // already called
    future_error::__throw(future_errc::future_already_retrieved);

  impl::future_state_base::acquire(state_);
  return future<R>(state_);
}

template<typename R>
auto promise<R>::set_value(const R& r) -> void {
  if (!state_)  // no state
    future_error::__throw(future_errc::no_state);

  state_->set_value(r);
}

template<typename R>
auto promise<R>::set_value(R&& r) -> void {
  if (!state_)  // no state
    future_error::__throw(future_errc::no_state);

  state_->set_value(move(r));
}

template<typename R>
auto promise<R>::set_exception(exception_ptr exc) -> void {
  if (!state_)  // no state
    future_error::__throw(future_errc::no_state);

  state_->set_exc(move(exc));
}

template<typename R>
auto promise<R>::set_value_at_thread_exit(const R& r) -> void {
  if (!state_)  // no state
    future_error::__throw(future_errc::no_state);

  state_->init_value(r);
}

template<typename R>
auto promise<R>::set_value_at_thread_exit(R&& r) -> void {
  if (!state_)  // no state
    future_error::__throw(future_errc::no_state);

  state_->init_value(move(r));
}

template<typename R>
auto promise<R>::set_exception_at_thread_exit(exception_ptr exc) -> void {
  if (!state_)  // no state
    future_error::__throw(future_errc::no_state);

  state_->init_exc(move(exc));
}


template<typename R>
promise<R&>::promise()
: promise(allocator_arg, allocator<void>())
{}

template<typename R>
template<typename Alloc>
promise<R&>::promise(allocator_arg_t, const Alloc& alloc)
: state_(impl::allocate_future_state<R*>(alloc))
{}

template<typename R>
promise<R&>::promise(promise&& p) noexcept
: state_(exchange(p.state_))
{}

template<typename R>
promise<R&>::~promise() noexcept {
  impl::future_state_base::release(state_);
}

template<typename R>
auto promise<R&>::operator=(promise&& p) noexcept -> promise& {
  promise(std::move(p)).swap(*this);
  return *this;
}

template<typename R>
auto promise<R&>::swap(promise& p) noexcept -> void {
  using _namespace(std)::swap;
  swap(state_, p.state_);
}

template<typename R>
auto promise<R&>::get_future() -> future<R&> {
  if (!state_)  // no state
    future_error::__throw(future_errc::no_state);
  if (!state_->mark_shared())  // already called
    future_error::__throw(future_errc::future_already_retrieved);

  impl::future_state_base::acquire(state_);
  return future<R&>(state_);
}

template<typename R>
auto promise<R&>::set_value(R& r) -> void {
  if (!state_)  // no state
    future_error::__throw(future_errc::no_state);

  state_->set_value(&r);
}

template<typename R>
auto promise<R&>::set_exception(exception_ptr exc) -> void {
  if (!state_)  // no state
    future_error::__throw(future_errc::no_state);

  state_->set_exc(move(exc));
}

template<typename R>
auto promise<R&>::set_value_at_thread_exit(R& r) -> void {
  if (!state_)  // no state
    future_error::__throw(future_errc::no_state);

  state_->init_value(&r);
}

template<typename R>
auto promise<R&>::set_exception_at_thread_exit(exception_ptr exc) -> void {
  if (!state_)  // no state
    future_error::__throw(future_errc::no_state);

  state_->init_exc(move(exc));
}


template<typename Alloc>
promise<void>::promise(allocator_arg_t, const Alloc& alloc)
: state_(impl::allocate_future_state<void>(alloc))
{}

inline promise<void>::promise(promise&& p) noexcept
: state_(exchange(p.state_, nullptr))
{}

inline promise<void>::~promise() noexcept {
  impl::future_state_base::release(state_);
}

inline auto promise<void>::operator=(promise&& p) noexcept -> promise& {
  promise(std::move(p)).swap(*this);
  return *this;
}

inline auto promise<void>::swap(promise& p) noexcept -> void {
  using _namespace(std)::swap;
  swap(state_, p.state_);
}


template<typename R>
void swap(promise<R>& x, promise<R>& y) noexcept {
  x.swap(y);
}


template<typename R>
future<R>::future() noexcept {}

template<typename R>
future<R>::future(future&& f) noexcept
: state_(exchange(f.state_, nullptr))
{}

template<typename R>
future<R>::~future() noexcept {
  impl::future_state_base::release(state_);
}

template<typename R>
auto future<R>::operator=(future&& f) noexcept -> future& {
  future(std::move(f)).swap(*this);
  return *this;
}

template<typename R>
auto future<R>::swap(future& f) noexcept -> void {
  using _namespace(std)::swap;
  swap(state_, f.state_);
}

template<typename R>
auto future<R>::share() -> shared_future<R> {
  return shared_future<R>(exchange(state_, nullptr));
}

template<typename R>
auto future<R>::get() -> R {
  if (!state_)  // no state
    future_error::__throw(future_errc::no_state);

  R rv = move(*state_->get());
  impl::future_state_base::release(exchange(state_, nullptr));
  return rv;
}

template<typename R>
auto future<R>::valid() const noexcept -> bool {
  return state_ != nullptr;
}

template<typename R>
auto future<R>::wait() const -> void {
  if (!state_)  // no state
    future_error::__throw(future_errc::no_state);
  state_->wait();
}

template<typename R>
template<typename Rep, typename Period>
auto future<R>::wait_for(const chrono::duration<Rep, Period>& d) const ->
    future_status {
  return wait_until(chrono::steady_clock::now() + d);
}

template<typename R>
template<typename Clock, typename Duration>
auto future<R>::wait_until(const chrono::time_point<Clock, Duration>& tp)
    const -> future_status {
  using state_t = impl::future_state_base::state_t;

  if (!state_)  // no state
    future_error::__throw(future_errc::no_state);

  switch (state_->wait_until(tp)) {
  case state_t::uninitialized_deferred:
    return future_status::deferred;
  case state_t::ready_value:
  case state_t::ready_exc:
    return future_status::ready;
  default:
    return future_status::timeout;
  }
}

template<typename R>
future<R>::future(impl::future_state<R>* s) noexcept
: state_(s)
{}


template<typename R>
future<R&>::future() noexcept {}

template<typename R>
future<R&>::future(future&& f) noexcept
: state_(exchange(f.state_, nullptr))
{}

template<typename R>
future<R&>::~future() noexcept {
  impl::future_state_base::release(state_);
}

template<typename R>
auto future<R&>::operator=(future&& f) noexcept -> future& {
  future(std::move(f)).swap(*this);
  return *this;
}

template<typename R>
auto future<R&>::swap(future& f) noexcept -> void {
  using _namespace(std)::swap;
  swap(state_, f.state_);
}

template<typename R>
auto future<R&>::share() -> shared_future<R&> {
  return shared_future<R&>(exchange(state_, nullptr));
}

template<typename R>
auto future<R&>::get() -> R& {
  if (!state_)  // no state
    future_error::__throw(future_errc::no_state);

  R& rv = **state_->get();
  impl::future_state_base::release(exchange(state_, nullptr));
  return rv;
}

template<typename R>
auto future<R&>::valid() const noexcept -> bool {
  return state_ != nullptr;
}

template<typename R>
auto future<R&>::wait() const -> void {
  if (!state_)  // no state
    future_error::__throw(future_errc::no_state);
  state_->wait();
}

template<typename R>
template<typename Rep, typename Period>
auto future<R&>::wait_for(const chrono::duration<Rep, Period>& d) const ->
    future_status {
  return wait_until(chrono::steady_clock::now() + d);
}

template<typename R>
template<typename Clock, typename Duration>
auto future<R&>::wait_until(const chrono::time_point<Clock, Duration>& tp)
    const -> future_status {
  using state_t = impl::future_state_base::state_t;

  if (!state_)  // no state
    future_error::__throw(future_errc::no_state);

  switch (state_->wait_until(tp)) {
  case state_t::uninitialized_deferred:
    return future_status::deferred;
  case state_t::ready_value:
  case state_t::ready_exc:
    return future_status::ready;
  default:
    return future_status::timeout;
  }
}

template<typename R>
future<R&>::future(impl::future_state<R*>* s) noexcept
: state_(s)
{}


inline future<void>::future() noexcept {}

inline future<void>::future(future&& f) noexcept
: state_(exchange(f.state_, nullptr))
{}

inline future<void>::~future() noexcept {
  impl::future_state_base::release(state_);
}

inline auto future<void>::operator=(future&& f) noexcept -> future& {
  future(std::move(f)).swap(*this);
  return *this;
}

inline auto future<void>::swap(future& f) noexcept -> void {
  using _namespace(std)::swap;
  swap(state_, f.state_);
}

inline auto future<void>::share() -> shared_future<void> {
  return shared_future<void>(exchange(state_, nullptr));
}

inline auto future<void>::valid() const noexcept -> bool {
  return state_ != nullptr;
}

template<typename Rep, typename Period>
auto future<void>::wait_for(const chrono::duration<Rep, Period>& d) const ->
    future_status {
  return wait_until(chrono::steady_clock::now() + d);
}

template<typename Clock, typename Duration>
auto future<void>::wait_until(const chrono::time_point<Clock, Duration>& tp)
    const -> future_status {
  using state_t = impl::future_state_base::state_t;

  if (!state_)  // no state
    future_error::__throw(future_errc::no_state);

  switch (state_->wait_until(tp)) {
  case state_t::uninitialized_deferred:
    return future_status::deferred;
  case state_t::ready_value:
  case state_t::ready_exc:
    return future_status::ready;
  default:
    return future_status::timeout;
  }
}

inline future<void>::future(impl::future_state<void>* s) noexcept
: state_(s)
{}


template<typename R>
void swap(future<R>& x, future<R>& y) noexcept {
  x.swap(y);
}


template<typename R>
shared_future<R>::shared_future(const shared_future& f)
: state_(f.state_)
{
  impl::future_state_base::acquire(state_);
}

template<typename R>
shared_future<R>::shared_future(shared_future&& f) noexcept
: state_(exchange(f.state_, nullptr))
{}

template<typename R>
shared_future<R>::shared_future(future<R>&& f) noexcept
: shared_future(f.share())
{}

template<typename R>
shared_future<R>::~shared_future() noexcept {
  impl::future_state_base::release(state_);
}

template<typename R>
auto shared_future<R>::operator=(const shared_future& f) -> shared_future& {
  shared_future(f).swap(*this);
  return *this;
}

template<typename R>
auto shared_future<R>::operator=(shared_future&& f) -> shared_future& {
  shared_future(move(f)).swap(*this);
  return *this;
}

template<typename R>
auto shared_future<R>::swap(shared_future& f) noexcept -> void {
  using _namespace(std)::swap;
  swap(state_, f.state_);
}

template<typename R>
auto shared_future<R>::get() const -> const R& {
  if (!state_)  // no state
    future_error::__throw(future_errc::no_state);

  const R& rv = *state_->get();
  return rv;
}

template<typename R>
auto shared_future<R>::valid() const noexcept -> bool {
  return state_ != nullptr;
}

template<typename R>
auto shared_future<R>::wait() const -> void {
  if (!state_)  // no state
    future_error::__throw(future_errc::no_state);
  state_->wait();
}

template<typename R>
template<typename Rep, typename Period>
auto shared_future<R>::wait_for(const chrono::duration<Rep, Period>& d)
    const -> future_status {
  return wait_until(chrono::steady_clock::now() + d);
}

template<typename R>
template<typename Clock, typename Duration>
auto shared_future<R>::wait_until(
    const chrono::time_point<Clock, Duration>& tp) const -> future_status {
  using state_t = impl::future_state_base::state_t;

  if (!state_)  // no state
    future_error::__throw(future_errc::no_state);

  switch (state_->wait_until(tp)) {
  case state_t::uninitialized_deferred:
    return future_status::deferred;
  case state_t::ready_value:
  case state_t::ready_exc:
    return future_status::ready;
  default:
    return future_status::timeout;
  }
}

template<typename R>
shared_future<R>::shared_future(impl::future_state<R>* s) noexcept
: state_(s)
{}


template<typename R>
shared_future<R&>::shared_future(const shared_future& f)
: state_(f.state_)
{
  impl::future_state_base::acquire(state_);
}

template<typename R>
shared_future<R&>::shared_future(shared_future&& f) noexcept
: state_(exchange(f.state_, nullptr))
{}

template<typename R>
shared_future<R&>::shared_future(future<R&>&& f) noexcept
: shared_future(f.share())
{}

template<typename R>
shared_future<R&>::~shared_future() noexcept {
  impl::future_state_base::release(state_);
}

template<typename R>
auto shared_future<R&>::operator=(const shared_future& f) -> shared_future& {
  shared_future(f).swap(*this);
  return *this;
}

template<typename R>
auto shared_future<R&>::operator=(shared_future&& f) -> shared_future& {
  shared_future(move(f)).swap(*this);
  return *this;
}

template<typename R>
auto shared_future<R&>::swap(shared_future& f) noexcept -> void {
  using _namespace(std)::swap;
  swap(state_, f.state_);
}

template<typename R>
auto shared_future<R&>::get() const -> R& {
  if (!state_)  // no state
    future_error::__throw(future_errc::no_state);

  R& rv = **state_->get();
  return rv;
}

template<typename R>
auto shared_future<R&>::valid() const noexcept -> bool {
  return state_ != nullptr;
}

template<typename R>
auto shared_future<R&>::wait() const -> void {
  if (!state_)  // no state
    future_error::__throw(future_errc::no_state);
  state_->wait();
}

template<typename R>
template<typename Rep, typename Period>
auto shared_future<R&>::wait_for(const chrono::duration<Rep, Period>& d)
    const -> future_status {
  return wait_until(chrono::steady_clock::now() + d);
}

template<typename R>
template<typename Clock, typename Duration>
auto shared_future<R&>::wait_until(
    const chrono::time_point<Clock, Duration>& tp) const -> future_status {
  using state_t = impl::future_state_base::state_t;

  if (!state_)  // no state
    future_error::__throw(future_errc::no_state);

  switch (state_->wait_until(tp)) {
  case state_t::uninitialized_deferred:
    return future_status::deferred;
  case state_t::ready_value:
  case state_t::ready_exc:
    return future_status::ready;
  default:
    return future_status::timeout;
  }
}

template<typename R>
shared_future<R&>::shared_future(impl::future_state<R*>* s) noexcept
: state_(s)
{}


inline shared_future<void>::shared_future(const shared_future& f)
: state_(f.state_)
{
  impl::future_state_base::acquire(state_);
}

inline shared_future<void>::shared_future(shared_future&& f) noexcept
: state_(exchange(f.state_, nullptr))
{}

inline shared_future<void>::shared_future(future<void>&& f) noexcept
: shared_future(f.share())
{}

inline shared_future<void>::~shared_future() noexcept {
  impl::future_state_base::release(state_);
}

inline auto shared_future<void>::operator=(const shared_future& f) ->
    shared_future& {
  shared_future(f).swap(*this);
  return *this;
}

inline auto shared_future<void>::operator=(shared_future&& f) ->
    shared_future& {
  shared_future(move(f)).swap(*this);
  return *this;
}

inline auto shared_future<void>::swap(shared_future& f) noexcept -> void {
  using _namespace(std)::swap;
  swap(state_, f.state_);
}

inline auto shared_future<void>::valid() const noexcept -> bool {
  return state_ != nullptr;
}

template<typename Rep, typename Period>
auto shared_future<void>::wait_for(const chrono::duration<Rep, Period>& d)
    const -> future_status {
  return wait_until(chrono::steady_clock::now() + d);
}

template<typename Clock, typename Duration>
auto shared_future<void>::wait_until(
    const chrono::time_point<Clock, Duration>& tp) const -> future_status {
  using state_t = impl::future_state_base::state_t;

  if (!state_)  // no state
    future_error::__throw(future_errc::no_state);

  switch (state_->wait_until(tp)) {
  case state_t::uninitialized_deferred:
    return future_status::deferred;
  case state_t::ready_value:
  case state_t::ready_exc:
    return future_status::ready;
  default:
    return future_status::timeout;
  }
}

inline shared_future<void>::shared_future(impl::future_state<void>* s) noexcept
: state_(s)
{}


template<typename R, typename... ArgTypes>
packaged_task<R(ArgTypes...)>::packaged_task(packaged_task&& pt) noexcept
: state_(exchange(pt.state_, nullptr))
{}

template<typename R, typename... ArgTypes>
template<typename F>
packaged_task<R(ArgTypes...)>::packaged_task(F&& f)
: packaged_task(allocator_arg, allocator<void>(), forward<F>(f))
{}

template<typename R, typename... ArgTypes>
template<typename F, typename Alloc>
packaged_task<R(ArgTypes...)>::packaged_task(allocator_arg_t,
                                             const Alloc& alloc,
                                             F&& f)
: state_(impl::allocate_future_state_task<R(ArgTypes...)>(alloc,
                                                          forward<F>(f)))
{}

template<typename R, typename... ArgTypes>
packaged_task<R(ArgTypes...)>::~packaged_task() noexcept {
  impl::future_state_base::release(state_);
}

template<typename R, typename... ArgTypes>
auto packaged_task<R(ArgTypes...)>::operator=(packaged_task&& pt) noexcept ->
    packaged_task& {
  packaged_task(std::move(pt)).swap(*this);
  return *this;
}

template<typename R, typename... ArgTypes>
auto packaged_task<R(ArgTypes...)>::swap(packaged_task& pt) noexcept -> void {
  using _namespace(std)::swap;
  swap(state_, pt.state_);
}

template<typename R, typename... ArgTypes>
auto packaged_task<R(ArgTypes...)>::valid() const noexcept ->
    bool {
  return state_ != nullptr;
}

template<typename R, typename... ArgTypes>
auto packaged_task<R(ArgTypes...)>::get_future() -> future<R> {
  if (!state_)  // no state
    future_error::__throw(future_errc::no_state);
  if (!state_->mark_shared())  // already called
    future_error::__throw(future_errc::future_already_retrieved);

  impl::future_state_base::acquire(state_);
  return future<R>(state_);
}

template<typename R, typename... ArgTypes>
auto packaged_task<R(ArgTypes...)>::operator()(ArgTypes... args) -> void {
  if (!state_)  // no state
    future_error::__throw(future_errc::no_state);

  state_->set_invoke(forward<ArgTypes>(args)...);
}

template<typename R, typename... ArgTypes>
auto packaged_task<R(ArgTypes...)>::make_ready_at_thread_exit(
    ArgTypes... args) -> void {
  if (!state_)  // no state
    future_error::__throw(future_errc::no_state);

  state_->init_invoke(forward<ArgTypes>(args)...);
}

template<typename R, typename... ArgTypes>
auto packaged_task<R(ArgTypes...)>::reset() -> void {
  impl::future_state_base::release(exchange(state_, nullptr));
}


template<typename R, typename... ArgTypes>
void swap(packaged_task<R(ArgTypes...)>& x,
          packaged_task<R(ArgTypes...)>& y) noexcept {
  x.swap(y);
}


template<typename F, typename... Args>
auto async(F&& f, Args&&... args) ->
    future<typename enable_if_t<!is_same<launch, decay_t<F>>::value,
                                identity<decltype(declval<F>()(
                                                      declval<Args>()...))>
                               >::type> {
  return async(launch::async | launch::deferred,
               forward<F>(f), forward<Args>(args)...);
}

template<typename F, typename... Args>
auto async(launch l, F&& f, Args&&... args) ->
    future<decltype(declval<F>()(declval<Args>()...))> {
  using rv_type = decltype(declval<F>()(declval<Args>()...));
  using task_type = packaged_task<rv_type(Args...)>;
  using future_type = future<rv_type>;

#ifndef _SINGLE_THREADED
  /* Case for thread. */
  if ((l & launch::async) == launch::async) {
    task_type pt = task_type(forward<F>(f));
    future_type f = pt.get_future();

    thread thr = thread(&task_type::make_ready_at_thread_exit,
                        move(pt), forward<Args>(args)...);
    thr.detach();
    return pt.get_future();
  }
#endif  // ! _SINGLE_THREADED

  /* Case for deferred. */
  if ((l & launch::deferred) == launch::deferred) {
    auto ptr = impl::allocate_future_state<rv_type>(allocator<void>(),
                                                    forward<F>(f),
                                                    forward<Args>(args)...);
    ptr->mark_shared();
    return future_type(ptr);
  }

#ifdef _SINGLE_THREADED
  /* Case for thread, iff threads are not supported. */
  if ((l & launch::async) == launch::async)
    throw system_error(make_error_code(errc::resource_unavailable_try_again));
#endif

  /* Empty/unrecognized launch policy. */
  throw invalid_argument("std::async: launch policy not recognized");
}


_namespace_end(std)

#endif /* _FUTURE_INL_H_ */
