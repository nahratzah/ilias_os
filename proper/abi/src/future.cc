#include <future>
#include <sstream>

_namespace_begin(std)
namespace {


class future_error_category : public error_category {
  const char* name() const noexcept override;
  string message(int) const override;
};

const char* future_error_category::name() const noexcept {
  return "future";
}

string future_error_category::message(int code) const {
  switch (code) {
  case 0:
    return "no error";
  case static_cast<int>(future_errc::broken_promise):
    return "broken promise";
  case static_cast<int>(future_errc::future_already_retrieved):
    return "future already retrieved";
  case static_cast<int>(future_errc::promise_already_satisfied):
    return "promise already satisfied";
  case static_cast<int>(future_errc::no_state):
    return "no state";
  }

  ostringstream rv;
  rv << name() << ":" << code;
  return rv.str();
}

const future_error_category future_category_impl{};


} /* namespace std::<unnamed> */


const error_category& future_category() noexcept {
  return future_category_impl;
}


future_error::future_error(error_code ec)
: logic_error(ec.message()),
  ec_(ec)
{}

future_error::~future_error() noexcept {}

void future_error::__throw(future_errc ec) {
  throw future_error(make_error_code(ec));
}


promise<void>::promise()
: promise(allocator_arg, allocator<void>())
{}

auto promise<void>::get_future() -> future<void> {
  if (!state_)  // no state
    future_error::__throw(future_errc::no_state);
  if (!state_->mark_shared())  // already called
    future_error::__throw(future_errc::future_already_retrieved);

  impl::future_state_base::acquire(state_);
  return future<void>(state_);
}

auto promise<void>::set_value() -> void {
  if (!state_)  // no state
    future_error::__throw(future_errc::no_state);

  state_->set_value();
}

auto promise<void>::set_exception(exception_ptr exc) -> void {
  if (!state_)  // no state
    future_error::__throw(future_errc::no_state);

  state_->set_exc(move(exc));
}

auto promise<void>::set_value_at_thread_exit() -> void {
  if (!state_)  // no state
    future_error::__throw(future_errc::no_state);

  state_->init_value();
}

auto promise<void>::set_exception_at_thread_exit(exception_ptr exc) ->
    void {
  if (!state_)  // no state
    future_error::__throw(future_errc::no_state);

  state_->init_exc(move(exc));
}


auto future<void>::get() -> void {
  if (!state_)  // no state
    future_error::__throw(future_errc::no_state);

  state_->get();
  impl::future_state_base::release(exchange(state_, nullptr));
}

auto future<void>::wait() const -> void {
  if (!state_)  // no state
    future_error::__throw(future_errc::no_state);
  state_->wait();
}


auto shared_future<void>::get() const -> void {
  if (!state_)  // no state
    future_error::__throw(future_errc::no_state);

  state_->get();
}

auto shared_future<void>::wait() const -> void {
  if (!state_)  // no state
    future_error::__throw(future_errc::no_state);
  state_->wait();
}


_namespace_end(std)
