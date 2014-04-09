#include <functional>

_namespace_begin(std)


bad_function_call::bad_function_call() noexcept {}
bad_function_call::~bad_function_call() noexcept {}

const char* bad_function_call::what() const noexcept {
  return "std::bad_function_call";
}


_namespace_end(std)
