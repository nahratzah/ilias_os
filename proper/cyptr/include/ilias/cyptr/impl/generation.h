#ifndef _ILIAS_CYPTR_IMPL_GENERATION_H_
#define _ILIAS_CYPTR_IMPL_GENERATION_H_

#include <mutex>
#include <new>
#include <ilias/linked_list.h>
#include <ilias/refcnt.h>
#include <ilias/cyptr/impl/fwd.h>
#include <ilias/cyptr/impl/tags.h>
#include <ilias/cyptr/impl/tstamp.h>

namespace ilias {
namespace cyptr {
namespace impl {


class generation
: public refcount_base<generation>
{
 public:
  using obj_list = linked_list<basic_obj, basic_obj_gen_linktag>;

  generation() noexcept = default;
  generation(const generation&) = delete;
  generation(generation&&) = delete;
  generation& operator=(const generation&) = delete;
  generation& operator=(generation&&) = delete;

  bool before(const generation&) const noexcept;
  static refpointer<generation> new_generation() throw (std::bad_alloc);

 private:
  std::mutex mtx_;
  obj_list obj_;
  const tstamp tstamp_ = tstamp::now();
};


}}} /* namespace ilias::cyptr::impl */

#include <ilias/cyptr/impl/generation-inl.h>

#endif /* _ILIAS_CYPTR_IMPL_GENERATION_H_ */
