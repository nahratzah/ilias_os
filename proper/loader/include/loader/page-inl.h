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
auto page_allocator<Arch>::add_range(uint64_t base, uint64_t len) ->
    void {
  using ilias::pmap::round_page_up;
  using ilias::pmap::round_page_down;
  using ilias::pmap::page_no;
  using ilias::pmap::phys_addr;

  auto s = phys_addr<Arch>(round_page_up(base, Arch));
  auto e = phys_addr<Arch>(round_page_down(base + len, Arch));

  bool need_sort = (!data_.empty() && data_.back().address() >= s);
  for (auto i = page_no<Arch>(s); i != e && data_.size() <= 2048; ++i)
    data_.emplace_back(i);
  if (need_sort) std::sort(data_.begin(), data_.end(), comparator());
}

template<ilias::arch Arch>
auto page_allocator<Arch>::allocate_page() -> ilias::pmap::page_no<Arch> {
  using std::placeholders::_1;

  auto found = std::find_if(data_.rbegin(), data_.rend(),
                            [](const page<Arch>& pg) { return pg.is_free(); });
  if (found == data_.rend()) throw std::bad_alloc();
  found->mark_in_use();
  return found->address();
}

template<ilias::arch Arch>
auto page_allocator<Arch>::deallocate_page(ilias::pmap::page_no<Arch> pgno)
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

template<ilias::arch Arch>
auto page_allocator<Arch>::map_page(ilias::pmap::page_no<Arch> pg) ->
    ilias::pmap::vpage_no<Arch> {
  if (pg.get() > (0xffffffff >> ilias::pmap::page_shift(Arch)))
    throw std::range_error("Physical page cannot be mapped.");
  return ilias::pmap::vpage_no<Arch>(pg.get());
}

template<ilias::arch Arch>
auto page_allocator<Arch>::unmap_page(ilias::pmap::vpage_no<Arch>) noexcept ->
    void {
  return;
}


} /* namespace loader */

#endif /* _LOADER_PAGE_INL_H_ */
