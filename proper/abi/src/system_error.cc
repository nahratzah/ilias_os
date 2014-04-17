#include <system_error>
#include <string>
#include <cstdio>
#include <functional>
#include <utility>
#include <new>

_namespace_begin(std)
namespace {


class generic_error_category : public error_category {
  const char* name() const noexcept override;
  string message(int) const override;
};

class system_error_category : public error_category {
  const char* name() const noexcept override;
  string message(int) const override;
  error_condition default_error_condition(int) const noexcept override;
};

const char* generic_error_category::name() const noexcept {
  return "generic";
}

string simple_create_msg(const error_category& ec, int code) {
  constexpr int N = 16;
  char buf[N];
  string rv;

  if (code == 0) {
    rv = "no error";
  } else {
    string_ref bufstr(buf, N - 1);
    int buflen = snprintf(buf, N, ":%d", code);
    if (buflen < 0) {
      switch (abi::errno) {
      case _ABI_ENOMEM:
        throw std::bad_alloc();
      default:
        assert(false);
        throw runtime_error("simple_create_msg: printf failed");
      }
    }

    string_ref name = ec.name();
    bufstr = bufstr.substr(0, buflen);
    rv.reserve(name.length() + bufstr.length());
    rv += name;
    rv += bufstr;
  }
  return rv;
}

string generic_error_category::message(int code) const {
  return simple_create_msg(*this, code);
}


const char* system_error_category::name() const noexcept {
  return "system";
}

string system_error_category::message(int code) const {
  if (code >= 0 && code < abi::sys_nerr) return abi::sys_errlist[code];
  return simple_create_msg(*this, code);
}

error_condition system_error_category::default_error_condition(int ev)
    const noexcept {
  const bool is_posix = (ev >= 1 && ev < _ABI_EPOSIXENDSHERE);
  return error_condition(ev, (is_posix ?
                              generic_category() :
                              system_category()));
}


const generic_error_category generic_category_impl{};
const system_error_category system_category_impl{};


} /* namespace std::<unnamed> */


error_category::~error_category() noexcept {}

error_condition error_category::default_error_condition(int ev)
    const noexcept {
  return error_condition(ev, *this);
}

bool error_category::equivalent(int code, const error_condition& condition)
    const noexcept {
  return default_error_condition(code) == condition;
}

bool error_category::equivalent(const error_code& code, int condition)
    const noexcept {
  return *this == code.category() && code.value() == condition;
}

const error_category& generic_category() noexcept {
  return generic_category_impl;
}

const error_category& system_category() noexcept {
  return system_category_impl;
}


error_condition error_code::default_error_condition() const noexcept {
  return category().default_error_condition(value());
}

string error_code::message() const {
  return category().message(value());
}


string error_condition::message() const {
  return category().message(value());
}


system_error::system_error(error_code ec, const string& msg)
: system_error(ec, string_ref(msg))
{}

system_error::system_error(error_code ec, const string_ref& msg)
: runtime_error(msg + string(":  ") + ec.message()),
  ec_(ec)
{}

system_error::system_error(error_code ec, const char* msg)
: system_error(ec, string_ref(msg))
{}

system_error::system_error(error_code ec)
: runtime_error(ec.message()),
  ec_(ec)
{}

system_error::system_error(int ev, const error_category& cat,
                           const string& msg)
: system_error(error_code(ev, cat), msg)
{}

system_error::system_error(int ev, const error_category& cat,
                           const string_ref& msg)
: system_error(error_code(ev, cat), msg)
{}

system_error::system_error(int ev, const error_category& cat, const char* msg)
: system_error(error_code(ev, cat), msg)
{}

system_error::system_error(int ev, const error_category& cat)
: system_error(error_code(ev, cat))
{}

system_error::~system_error() noexcept {}

const error_code& system_error::code() const noexcept {
  return ec_;
}

size_t hash<error_code>::operator()(error_code ec) const {
  return hash<int>()(ec.value());
}


_namespace_end(std)
