#include <ilias/vm/page_owner.h>

namespace ilias {
namespace vm {


page_owner::~page_owner() noexcept {}

auto page_owner::release_urgent(offset_type, page&) -> bool {
  return false;
}

auto page_owner::release_async(offset_type, page&) -> void {
  return;
}

auto page_owner::undirty_async(offset_type, page&) -> void {
  return;
}


}} /* namespace ilias::vm */
