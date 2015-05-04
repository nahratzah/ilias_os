#ifndef _ILIAS_PMAP_PMAP_PAGE_H_
#define _ILIAS_PMAP_PMAP_PAGE_H_

#include "pmap_page-fwd.h"
#include <mutex>
#include <atomic>
#include <ilias/arch.h>
#include <ilias/pmap/page.h>
#include <ilias/pmap/pmap.h>
#include <ilias/pmap/rmap.h>
#include <tuple>
#include <limits>

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
  void flush_accessed_dirty_() noexcept;

  template<arch VA>
  void pmap_deregister_(pmap<VA>&, vpage_no<VA>) noexcept;

 private:
  rmap_list rmap_;
};

class pmap_page
: private pmap_page_tmpl<native_arch, native_varch>
{
  template<arch> friend class pmap;

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

  page_no<native_arch> address() const noexcept;
  void mark_accessed() noexcept;
  void mark_dirty() noexcept;
  void mark_accessed_and_dirty() noexcept;

 protected:
  std::tuple<bool, bool> get_accessed_dirty() noexcept;
  std::tuple<bool, bool> flush_accessed_dirty(bool = false) noexcept;

 private:
  static constexpr unsigned int pgno_bits =
      std::numeric_limits<page_no<native_arch>::type>::digits - 2;
  static constexpr page_no<native_arch>::type pgno_mask =
      (page_no<native_arch>::type(1) << pgno_bits) - 1U;
  static constexpr page_no<native_arch>::type accessed_mask =
      page_no<native_arch>::type(1) << (pgno_bits + 0U);
  static constexpr page_no<native_arch>::type dirty_mask =
      page_no<native_arch>::type(1) << (pgno_bits + 1U);

  mutable std::mutex guard_;  // XXX: Should be a shared mutex.
  std::atomic<page_no<native_arch>::type> address_and_ad_;
};


}} /* namespace ilias::pmap */

#include "pmap_page-inl.h"

#endif /* _ILIAS_PMAP_PMAP_PAGE_H_ */
