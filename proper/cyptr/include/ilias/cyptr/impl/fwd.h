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
class background;

enum class obj_color : unsigned int {
  unlinked,
  linked,
  maybe_dying,
  dying,
  dead,
};

using generation_ptr = refpointer<generation>;


} /* namespace ilias::cyptr::impl */


class node;
template<typename> class edge;
template<typename> class cyptr;


}} /* namespace ilias::cyptr */

#endif /* _ILIAS_CYPTR_IMPL_FWD_H_ */
