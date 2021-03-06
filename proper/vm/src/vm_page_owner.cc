#include <ilias/vm/page_owner.h>
#include <ilias/vm/page.h>

namespace ilias {
namespace vm {


auto page_owner::release_urgent(offset_type, page&) -> page_ptr {
  return nullptr;
}

auto page_owner::release_async(offset_type, page&) -> void {
  return;
}

auto page_owner::undirty_async(page_busy_lock, offset_type, page&) -> void {
  return;
}


}} /* namespace ilias::vm */
