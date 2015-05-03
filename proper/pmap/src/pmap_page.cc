#include <ilias/pmap/pmap_page.h>
#include <ilias/pmap/pmap.h>
#include <ilias/pmap/pmap_i386.h>
#include <ilias/pmap/pmap_amd64.h>

namespace ilias {
namespace pmap {


pmap_page::pmap_page(page_no<native_arch> pgno) noexcept
: address(pgno)
{}

pmap_page::~pmap_page() noexcept {}

auto pmap_page::linked() const noexcept -> bool {
  std::lock_guard<std::mutex> lck{ guard_ };
  return linked_();
}

auto pmap_page::reduce_permissions(bool reduce_kernel, permission perm)
    noexcept -> void {
  std::lock_guard<std::mutex> lck{ guard_ };
  reduce_permissions_(reduce_kernel, perm);
}

auto pmap_page::unmap(bool unmap_kernel) noexcept -> void {
  std::lock_guard<std::mutex> lck{ guard_ };
  unmap_(unmap_kernel);
}


}} /* namespace ilias::pmap */
