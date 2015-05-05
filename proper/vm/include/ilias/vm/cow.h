#ifndef _ILIAS_VM_COW_H_
#define _ILIAS_VM_COW_H_

#include <ilias/vm/vmmap.h>
#include <ilias/vm/anon.h>

namespace ilias {
namespace vm {


class cow_vme
: public anon_vme
{
 public:
  cow_vme(anon_vme&&, vmmap_entry_ptr&&) noexcept;
  cow_vme(workq_ptr, page_count<native_arch>, vmmap_entry_ptr&&);
  cow_vme(const cow_vme&);
  cow_vme(cow_vme&&) noexcept;
  ~cow_vme() noexcept override;

  void fault_read(cb_promise<page_ptr>, monitor_token,
                  shared_ptr<page_alloc>, page_count<native_arch>)
      noexcept override;
  void fault_write(cb_promise<page_ptr>, monitor_token,
                   shared_ptr<page_alloc>, page_count<native_arch>)
      noexcept override;
  vector<bool> mincore() const override;

  vmmap_entry_ptr clone() const override;
  pair<vmmap_entry_ptr, vmmap_entry_ptr> split(
      page_count<native_arch>) const override;

 private:
  vmmap_entry_ptr nested_;
};


}} /* namespace ilias::vm */

#endif /* _ILIAS_VM_COW_H_ */
