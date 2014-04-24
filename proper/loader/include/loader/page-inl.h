#ifndef _LOADER_PAGE_INL_H_
#define _LOADER_PAGE_INL_H_

#include <loader/page.h>
#include <algorithm>
#include <functional>

namespace loader {


template<ilias::arch Arch>
struct page_allocator<Arch>::comparator {
 private:
  const ilias::pmap::page_no<Arch>&
  convert_(const ilias::pmap::page_no<Arch>& x) const noexcept {
    return x;
  }

  ilias::pmap::page_no<Arch>
  convert_(const page<Arch>& pg) const noexcept {
    return pg.address();
  }

 public:
  template<typename A, typename B>
  bool operator()(const A& a, const B& b) const noexcept {
    return this->convert_(a) < this->convert_(b);
  }
};


template<ilias::arch Arch>
auto page_allocator<Arch>::add_range(uint64_t phys_addr, uint64_t len) ->
    void {
  auto off = phys_addr & ilias::pmap::page_mask(Arch);
  if (off != 0) {
    if (len <= off) return;
    len -= off;
    phys_addr += ilias::pmap::page_size(Arch) - off;
  }

  len &= ilias::pmap::page_mask(Arch);
  if (len == 0) return;

  uint64_t pa_end = phys_addr + len;
  for (uint64_t i = phys_addr; i != pa_end; ++i)
    data_.emplace_back(ilias::pmap::phys_addr<Arch>(i));
  std::sort(data_.begin(), data_.end(), comparator());
}

template<ilias::arch Arch>
auto page_allocator<Arch>::allocate() -> ilias::pmap::page_no<Arch> {
  using std::placeholders::_1;

  auto found = std::find_if(data_.rbegin(), data_.rend(),
                            [](const page<Arch>& pg) { return pg.is_free(); });
  if (found == data_.rend()) throw std::bad_alloc();
  found->mark_in_use();
  return found->address();
}

template<ilias::arch Arch>
auto page_allocator<Arch>::deallocate(ilias::pmap::page_no<Arch> pgno)
    noexcept -> void {
  auto found = std::lower_bound(data_.begin(), data_.end(), pgno,
                                comparator());
  assert(found && found->address() == pgno && !found->is_free());
  found->mark_as_free();
}

template<ilias::arch Arch>
auto page_allocator<Arch>::mark_in_use(ilias::pmap::page_no<Arch> b,
                                       ilias::pmap::page_no<Arch> e)
    noexcept -> bool {
  using std::placeholders::_1;

  auto d_b = std::lower_bound(data_.begin(), data_.end(), b,
                              comparator());
  auto d_e = std::lower_bound(d_b, data_.end(), e,
                              comparator());
  std::for_each(d_b, d_e, std::bind(&page<Arch>::mark_in_use, _1));
  return d_b != d_e;
}


} /* namespace loader */

#endif /* _LOADER_PAGE_INL_H_ */
