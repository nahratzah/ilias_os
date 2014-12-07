#include <ilias/cyptr/impl/basic_obj.h>
#include <cassert>

namespace ilias {
namespace cyptr {
namespace impl {


basic_obj::~basic_obj() noexcept {
  std::lock_guard<std::mutex> guard{ mtx_ };
  assert(edge_list_.empty());
}


}}} /* namespace ilias::cyptr::impl */
