#ifndef _ILIAS_PMAP_PMAP_PAGE_H_
#define _ILIAS_PMAP_PMAP_PAGE_H_

#include <mutex>
#include <ilias/arch.h>
#include <ilias/pmap/page.h>
#include <ilias/pmap/pmap.h>
#include <ilias/linked_list.h>

namespace ilias {
namespace pmap {


struct rmap_page_tag {};
struct rmap_pmap_tag {};

enum class reduce_permission_result {
  OK,
  UNMAPPED
};

template<arch Arch> class rmap_entry
: public list_element<rmap_page_tag>,
  public list_element<rmap_pmap_tag>
{
 public:
  constexpr rmap_entry(pmap<Arch>&, vaddr<Arch>) noexcept;
  rmap_entry(const rmap_entry&) = delete;
  rmap_entry& operator=(const rmap_entry&) = delete;

  // XXX: pool based operator new/delete

  pmap<Arch>*const pmap_;
  const vaddr<Arch> va_;
};

template<arch Arch> class pmap_page {
  friend pmap<Arch>;

 private:
  using rmap_list = linked_list<rmap_entry>;

 public:
  pmap_page(page_no<Arch>) noexcept;
  pmap_page(const pmap_page&) = delete;
  pmap_page& operator=(const pmap_page&) = delete;
  ~pmap_page() noexcept;

  bool linked() const noexcept;
  void reduce_permissions(bool, permission) noexcept;
  void unmap(bool) noexcept;

  const page_no<Arch> address;

 private:
  bool linked_() const noexcept;
  void reduce_permissions_(bool, permission) noexcept;
  void unmap_(bool) noexcept;

  mutable std::mutex guard_;  // Should be a shared mutex.
  rmap_list rmap_;
};

pmap_page::~pmap_page() noexcept {
  std::lock_guard<std::mutex> lck{ guard_ };
  rmap_.clear();
}


}} /* namespace ilias::pmap */

#include <ilias/pmap/pmap_page-inl.h>

#endif /* _ILIAS_PMAP_PMAP_PAGE_H_ */
