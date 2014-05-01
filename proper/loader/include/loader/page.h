#ifndef _LOADER_PAGE_H_
#define _LOADER_PAGE_H_

#include <ilias/arch.h>
#include <ilias/pmap/page.h>
#include <ilias/pmap/pmap.h>
#include <vector>

namespace loader {

template<ilias::arch Arch>
class page {
 public:
  page() noexcept : pgno_(0) {}
  page(ilias::pmap::page_no<Arch> pgno) noexcept : pgno_(pgno) {}

  ilias::pmap::page_no<Arch> address() const noexcept { return pgno_; }
  bool is_free() const noexcept { return free_; }
  void mark_in_use() noexcept { free_ = false; }
  void mark_as_free() noexcept { free_ = true; }

 private:
  ilias::pmap::page_no<Arch> pgno_;
  bool free_ = true;
};

template<ilias::arch Arch>
class page_allocator
: public ilias::pmap::pmap_support<Arch>
{
 public:
  page_allocator() = default;
  page_allocator(const page_allocator&) = delete;
  page_allocator& operator=(const page_allocator&) = delete;
  ~page_allocator() noexcept override {}

  void add_range(uint64_t phys_addr, uint64_t len);
  void shrink_to_fit() noexcept { data_.shrink_to_fit(); }

  ilias::pmap::page_no<Arch> allocate_page() override;
  void deallocate_page(ilias::pmap::page_no<Arch>) noexcept override;
  bool mark_in_use(ilias::pmap::page_no<Arch>, ilias::pmap::page_no<Arch>)
      noexcept;

  ilias::pmap::vpage_no<Arch> map_page(ilias::pmap::page_no<Arch>) override;
  void unmap_page(ilias::pmap::vpage_no<Arch>) noexcept override;

 private:
  struct comparator;

  std::vector<page<Arch>> data_;
};

#if defined(__i386__) || defined(__amd64__) || defined(__x86_64__)
extern template class page_allocator<ilias::arch::i386>;
extern template class page_allocator<ilias::arch::amd64>;
#endif

} /* namespace loader */

#include <loader/page-inl.h>

#endif /* _LOADER_PAGE_H_ */
