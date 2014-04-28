#ifndef _ILIAS_PMAP_PAGE_ALLOC_SUPPORT_H_
#define _ILIAS_PMAP_PAGE_ALLOC_SUPPORT_H_

#include <ilias/arch.h>
#include <ilias/pmap/page.h>
#include <ilias/pmap/pmap.h>

namespace ilias {
namespace pmap {


template<arch Arch = native_arch>
class page_ptr {
 public:
  page_ptr() noexcept = default;
  page_ptr(const page_ptr&) = delete;
  page_ptr(page_ptr&&) noexcept;
  explicit page_ptr(page_no<Arch>) noexcept;
  ~page_ptr() noexcept;

  page_ptr& operator=(const page_ptr&) = delete;
  page_ptr& operator=(page_ptr&&) noexcept;
  void swap(page_ptr& other) noexcept;

  explicit operator bool() const noexcept;
  bool is_allocated() const noexcept;
  void set_allocated(pmap_support<Arch>&) noexcept;

  page_no<Arch> get() const noexcept;
  page_no<Arch> release() noexcept;

  static page_ptr allocate(pmap_support<Arch>&);

 private:
  page_no<Arch> pgno_;
  pmap_support<Arch>* release_on_destruction_;
  bool valid_ = false;
};

template<arch Arch>
void swap(page_ptr<Arch>& a, page_ptr<Arch>& b) noexcept;


template<typename T, arch Arch> struct unmap_page_deleter;

template<typename T, arch Arch> using pmap_mapped_ptr =
    std::unique_ptr<T, unmap_page_deleter<T, Arch>>;

template<typename T, arch Arch>
auto pmap_map_page(page_no<Arch>, pmap_support<Arch>&) ->
    pmap_mapped_ptr<T, Arch>;


}} /* namespace ilias::pmap */

#include <ilias/pmap/page_alloc_support-inl.h>

#endif /* _ILIAS_PMAP_PAGE_ALLOC_SUPPORT_H_ */
