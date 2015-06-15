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


auto module::instance(string_ref name) -> module& {
  return registry::singleton().modules_[name];
}


}} /* namespace ilias::test */
