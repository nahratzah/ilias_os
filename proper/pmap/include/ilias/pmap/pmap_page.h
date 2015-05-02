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


template<arch, typename> class pmap_page_tmpl;  // Undefined

template<arch PhysArch, arch... VirtArch>
class pmap_page_tmpl<PhysArch, arch_set<VirtArch...>> {
 private:
  using rmap_list = std::tuple<rmap<PhysArch, VirtArch>...>;

 public:
  pmap_page_tmpl(const pmap_page_tmpl&) = delete;
  pmap_page_tmpl& operator=(const pmap_page_tmpl&) = delete;

 protected:
  pmap_page_tmpl() noexcept;
  ~pmap_page_tmpl() noexcept = default;

  bool linked_() const noexcept;
  void reduce_permissions_(bool, permission) noexcept;
  void unmap_(bool) noexcept;

  rmap_list rmap_;
};

class pmap_page
: private pmap_page_tmpl<native_arch, native_varch>
{
 public:
  pmap_page() = delete;
  pmap_page(const pmap_page&) = delete;
  pmap_page& operator=(const pmap_page&) = delete;

 protected:
  pmap_page(page_no<native_arch>) noexcept;
  ~pmap_page() noexcept;

 public:
  bool linked() const noexcept;
  void reduce_permissions(bool, permission) noexcept;
  void unmap(bool) noexcept;

  const page_no<native_arch> address;

 private:
  mutable std::mutex guard_;  // XXX: Should be a shared mutex.
};


}} /* namespace ilias::pmap */

#include "pmap_page-inl.h"

#endif /* _ILIAS_PMAP_PMAP_PAGE_H_ */
