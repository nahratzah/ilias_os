#include <ilias/any.h>

_namespace_begin(ilias)


any_error::~any_error() noexcept {}

auto any_error::__throw() -> void {
  __throw(_namespace(std)::string_ref("ilias::any: no value"));
}

auto any_error::__throw(_namespace(std)::string_ref msg) -> void {
  throw any_error(msg);
}

auto any_error::__throw(const _namespace(std)::string& msg) -> void {
  throw any_error(msg);
}

auto any_error::__throw(const char* msg) -> void {
  throw any_error(msg);
}


_namespace_end(ilias)
