#ifndef _ILIAS_VM_ANON_H_
#define _ILIAS_VM_ANON_H_

#include <ilias/vm/vmmap.h>
#include <memory>
#include <vector>
#include <mutex>
#include <ilias/refcnt.h>
#include <ilias/monitor.h>
#include <ilias/vm/page_owner.h>

namespace ilias {
namespace vm {

using namespace std;


class anon_vme
: public vmmap_entry
{
 private:
  class entry final
  : public refcount_base<entry>,
    public page_owner
  {
   public:
    entry() = default;
    entry(const entry&) = delete;
    entry(entry&&) = delete;
    entry& operator=(const entry&) = delete;
    entry& operator=(entry&&) = delete;
    ~entry() noexcept;

    void fault(cb_promise<page_ptr>, monitor_token,
               shared_ptr<page_alloc>, workq_ptr);
    bool present() const noexcept;
    void assign(cb_promise<page_ptr>, monitor_token,
                workq_ptr, cb_future<page_ptr>) noexcept;

   private:
    void assign_locked_(cb_promise<page_ptr>, workq_ptr,
                        cb_future<monitor_token>,
                        cb_future<page_ptr>) noexcept;
    page_ptr allocation_callback_(monitor_token, page_ptr) noexcept;

    page_ptr release_urgent(page_owner::offset_type, page&) override;

    mutable monitor guard_;
    page_ptr page_ = nullptr;
  };

  using entry_ptr = refpointer<entry>;
  using data_type = vector<entry_ptr>;

 public:
  anon_vme() = delete;
  anon_vme(const anon_vme&);
  anon_vme(anon_vme&&) noexcept;
  anon_vme(workq_ptr wq, page_count<native_arch> npg);
  template<typename Iter> anon_vme(workq_ptr, Iter, Iter);
  ~anon_vme() noexcept override;

  bool empty() const noexcept;
  bool all_present() const noexcept;
  bool present(page_count<native_arch>) const noexcept;

  void fault_read(cb_promise<page_ptr>, monitor_token,
                  shared_ptr<page_alloc>, page_count<native_arch>)
      noexcept override;
  void fault_write(cb_promise<page_ptr>, monitor_token,
                   shared_ptr<page_alloc>, page_count<native_arch>)
      noexcept override;
  void fault_assign(cb_promise<page_ptr>, monitor_token,
                    page_count<native_arch>, cb_future<page_ptr>) noexcept;
  vector<bool> mincore() const override;

  vmmap_entry_ptr clone() const override;
  pair<vmmap_entry_ptr, vmmap_entry_ptr> split(
      page_count<native_arch>) const override;
  pair<anon_vme, anon_vme> split_no_alloc(page_count<native_arch>) const;

 private:
  void fault_rw_(cb_promise<page_ptr>,
                 monitor_token,
                 shared_ptr<page_alloc>,
                 page_count<native_arch>) noexcept;

  data_type data_;
};


}} /* namespace ilias::vm */

#include <ilias/vm/anon-inl.h>

#endif /* _ILIAS_VM_ANON_H_ */
