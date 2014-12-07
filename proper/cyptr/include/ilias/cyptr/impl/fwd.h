#ifndef _ILIAS_CYPTR_IMPL_FWD_H_
#define _ILIAS_CYPTR_IMPL_FWD_H_

#include <ilias/refcnt.h>

namespace ilias {
namespace cyptr {
namespace impl {


class generation;
class basic_obj;
class basic_obj_lock;
class edge;

enum class obj_color : unsigned int {
  unlinked,
  strong_linked,
  gen_linked,
};

using generation_ptr = refpointer<generation>;


}}} /* namespace ilias::cyptr::impl */

#endif /* _ILIAS_CYPTR_IMPL_FWD_H_ */
