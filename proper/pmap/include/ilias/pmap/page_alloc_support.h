#ifndef _ILIAS_PMAP_PAGE_ALLOC_SUPPORT_H_
#define _ILIAS_PMAP_PAGE_ALLOC_SUPPORT_H_

#include <ilias/arch.h>
#include <ilias/pmap/page.h>

namespace ilias {
namespace pmap {


template<arch Arch = native_arch>
class page_ptr {
 public:
  page_ptr() noexcept;
  page_ptr(const page_ptr&) = delete;
  page_ptr(page_ptr&&) noexcept;
  explicit page_ptr(page_no<Arch>) noexcept;
  ~page_ptr() noexcept;

  page_ptr& operator=(const page_ptr&) = delete;
  page_ptr& operator=(page_ptr&&) noexcept;
  void swap(page_ptr& other) noexcept;

  explicit operator bool() const noexcept;
  bool is_allocated() const noexcept;

  page_no<Arch> get() const noexcept;
  page_no<Arch> release() noexcept;

  static page_ptr allocate();

 private:
  page_no<Arch> pgno_;
  bool release_on_destruction_ : 1;
  bool valid_ : 1;
};

template<arch Arch>
void swap(page_ptr<Arch>& a, page_ptr<Arch>& b) noexcept;


}} /* namespace ilias::pmap */

#include <ilias/pmap/page_alloc_support-inl.h>

#endif /* _ILIAS_PMAP_PAGE_ALLOC_SUPPORT_H_ */
