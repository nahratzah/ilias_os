#include <ilias/vm/page_owner.h>
#include <ilias/vm/page.h>

namespace ilias {
namespace vm {


page_owner::~page_owner() noexcept {}

auto page_owner::release_urgent(offset_type, page&) -> page_refptr {
  return nullptr;
}

auto page_owner::release_async(offset_type, page&) -> void {
  return;
}

auto page_owner::undirty_async(offset_type, page&) -> void {
  return;
}


}} /* namespace ilias::vm */
