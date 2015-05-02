#ifndef _ILIAS_PMAP_PMAP_PAGE_H_
#define _ILIAS_PMAP_PMAP_PAGE_H_

#include <mutex>
#include <ilias/arch.h>
#include <ilias/pmap/page.h>
#include <ilias/pmap/pmap.h>
#include <ilias/pmap/rmap.h>
#include <tuple>

namespace ilias {
namespace pmap {


struct rmap_page_tag {};
struct rmap_pmap_tag {};

template<arch, typename> class pmap_page;  // Undefined

template<arch PhysArch, arch... VirtArch>
class pmap_page<PhysArch, arch_set<VirtArch...>> {
 private:
  using rmap_list = std::tuple<rmap<PhysArch, VirtArch>...>;

 public:
  pmap_page() = delete;
  pmap_page(const pmap_page&) = delete;
  pmap_page& operator=(const pmap_page&) = delete;

 protected:
  pmap_page(page_no<PhysArch>) noexcept;
  ~pmap_page() noexcept = default;

 public:
  bool linked() const noexcept;
  void reduce_permissions(bool, permission) noexcept;
  void unmap(bool) noexcept;

  const page_no<PhysArch> address;

 private:
  bool linked_() const noexcept;
  void reduce_permissions_(bool, permission) noexcept;
  void unmap_(bool) noexcept;

  mutable std::mutex guard_;  // XXX: Should be a shared mutex.
  rmap_list rmap_;
};


}} /* namespace ilias::pmap */

#include "pmap_page-inl.h"

#endif /* _ILIAS_PMAP_PMAP_PAGE_H_ */
