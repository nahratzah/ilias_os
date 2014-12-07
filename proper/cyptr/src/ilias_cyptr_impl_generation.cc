#include <ilias/cyptr/impl/generation.h>

namespace ilias {
namespace cyptr {
namespace impl {


refpointer<generation> generation::new_generation() throw (std::bad_alloc) {
  return make_refpointer<generation>();
}


}}} /* namespace ilias::cyptr::impl */
