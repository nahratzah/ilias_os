#include <chrono>
#include <cdecl.h>
#include <cassert>

_namespace_begin(std)
namespace chrono {

auto system_clock::now() -> time_point {
  assert_msg(false, "std::chrono::system_clock::now(): unimplemented");
  for (;;);
}

auto steady_clock::now() -> time_point {
  assert_msg(false, "std::chrono::steady_clock::now(): unimplemented");
  for (;;);
}

auto high_resolution_clock::now() -> time_point {
  assert_msg(false,
             "std::chrono::high_resolution_clock::now(): unimplemented");
  for (;;);
}

} /* namespace std::chrono */
_namespace_end(std)
