#include <ilias/optional.h>

_namespace_begin(ilias)


optional_error::~optional_error() noexcept {}

auto optional_error::__throw() -> void {
  __throw(_namespace(std)::string_ref("ilias::optional: no value"));
}

auto optional_error::__throw(_namespace(std)::string_ref msg) -> void {
  throw optional_error(msg);
}

auto optional_error::__throw(const _namespace(std)::string& msg) -> void {
  throw optional_error(msg);
}

auto optional_error::__throw(const char* msg) -> void {
  throw optional_error(msg);
}


_namespace_end(ilias)
