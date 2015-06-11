#include <ilias/test/registry.h>
#include <thread>
#include <utility>
#include <mutex>
#include <ilias/test/json.h>

namespace ilias {
namespace test {


registry::registry() noexcept {}

auto registry::singleton() noexcept -> registry& {
  static once_flag guard;
  static aligned_storage_t<sizeof(registry),
                           alignof(registry)> data;

  void* data_ptr = reinterpret_cast<void*>(&data);
  call_once(guard,
            [](void* ptr) {
              new (ptr) registry();
            },
            data_ptr);
  return *static_cast<registry*>(data_ptr);
}

auto operator<<(ostream& out, const registry& r) ->
    ostream& {
  auto arr_out = out << begin_json << begin_json_array;

  for (auto m : r.modules_) {
    arr_out << m.first;// << ": " << m.second;
  }
  return arr_out << end_json_array << end_json;
}


auto module::instance(string_ref name) -> module& {
  return registry::singleton().modules_[name];
}

auto module::operator<<(ostream& out, const module& m) noexcept ->
    ostream& {
}


}} /* namespace ilias::test */
