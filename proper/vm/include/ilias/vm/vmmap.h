#ifndef _ILIAS_VM_VMMAP_H_
#define _ILIAS_VM_VMMAP_H_

#include <ilias/arch.h>
#include <ilias/pmap/pmap.h>
#include <ilias/pmap/page.h>

namespace ilias {
namespace vm {


using vm_permission = unsigned char;
constexpr vm_permission vm_perm_read = 0x01;
constexpr vm_permission vm_perm_write = 0x02;
constexpr vm_permission vm_perm_exec = 0x03;
constexpr unsigned int vm_permission_bits = 3;

template<arch Arch>
class vmmap {
 public:
  using range = std::tuple<pmap::vpage_no<Arch>, pmap::page_count<Arch>;

  class entry {
   public:
    range get_range_used() const noexcept;
    range get_range_free() const noexcept;
    vm_permission get_permission() const noexcept;

   private:
    std::tuple<vpage_no<Arch>, page_count<Arch>, page_count<Arch>> range_;
    vm_permission perm_ : vm_permission_bits;
  };

 private:
  pmap<Arch> pmap_;
  std::list entries_;
};


}} /* namespace ilias::vm */

#endif /* _ILIAS_VM_VMMAP_H_ */
