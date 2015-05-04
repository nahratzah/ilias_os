#include <ilias/pmap/pmap_page.h>
#include <ilias/pmap/pmap.h>
#include <ilias/pmap/pmap_i386.h>
#include <ilias/pmap/pmap_amd64.h>
#include <utility>

namespace ilias {
namespace pmap {


constexpr unsigned int pmap_page::pgno_bits;
constexpr page_no<native_arch>::type pmap_page::pgno_mask;
constexpr page_no<native_arch>::type pmap_page::accessed_mask;
constexpr page_no<native_arch>::type pmap_page::dirty_mask;


pmap_page::pmap_page(page_no<native_arch> pgno) noexcept
: address_and_ad_(pgno.get() & pgno_mask)
{}

pmap_page::~pmap_page() noexcept {}

auto pmap_page::linked() const noexcept -> bool {
  std::lock_guard<std::mutex> lck{ guard_ };
  return linked_();
}

auto pmap_page::reduce_permissions(bool reduce_kernel, permission perm,
                                   bool update_ad)
    noexcept -> void {
  std::lock_guard<std::mutex> lck{ guard_ };
  reduce_permissions_(reduce_kernel, perm, update_ad);
}

auto pmap_page::unmap(bool unmap_kernel) noexcept -> void {
  std::lock_guard<std::mutex> lck{ guard_ };
  unmap_(unmap_kernel);
}

auto pmap_page::flush_accessed_dirty(bool clear) noexcept ->
    std::tuple<bool, bool> {
  using std::tie;
  using std::make_tuple;

  std::lock_guard<std::mutex> lck{ guard_ };
  flush_accessed_dirty_();

  page_no<native_arch>::type f;
  if (clear) {
    f = address_and_ad_.fetch_and(~(accessed_mask | dirty_mask),
                                  std::memory_order_relaxed);
  } else {
    f = address_and_ad_.load(std::memory_order_relaxed);
  }
  return make_tuple(f & accessed_mask, f & dirty_mask);
}


}} /* namespace ilias::pmap */
