#include <ilias/linked_list.h>

_namespace_begin(ilias)

auto basic_linked_list::reverse() noexcept -> void {
  element* e = root_.succ_;
  do {
    e = e->pred_ = _namespace(std)::exchange(e->succ_, e->pred_);
  } while (e != &root_);
}

_namespace_end(ilias)
