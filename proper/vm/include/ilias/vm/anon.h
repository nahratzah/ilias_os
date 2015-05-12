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

    cb_future<tuple<page_ptr, monitor_token>> fault(monitor_token,
                                                    shared_ptr<page_alloc>,
                                                    workq_ptr);
    bool present() const noexcept;
    cb_future<tuple<page_ptr, monitor_token>> assign(monitor_token,
                                                     workq_ptr, page_ptr);

   private:
    tuple<page_ptr, monitor_token> allocation_callback_(monitor_token,
                                                        page_ptr) noexcept;

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

  cb_future<tuple<page_ptr, monitor_token>> fault_read(
      monitor_token, shared_ptr<page_alloc>, page_count<native_arch>) override;
  cb_future<tuple<page_ptr, monitor_token>> fault_write(
      monitor_token, shared_ptr<page_alloc>, page_count<native_arch>) override;
  cb_future<tuple<page_ptr, monitor_token>> fault_assign(
      monitor_token, page_count<native_arch>, page_ptr);
  vector<bool> mincore() const override;

  vmmap_entry_ptr clone() const override;
  pair<vmmap_entry_ptr, vmmap_entry_ptr> split(
      page_count<native_arch>) const override;
  pair<anon_vme, anon_vme> split_no_alloc(page_count<native_arch>) const;

 private:
  cb_future<tuple<page_ptr, monitor_token>> fault_rw_(monitor_token,
                                                      shared_ptr<page_alloc>,
                                                      page_count<native_arch>);

  data_type data_;
};


}} /* namespace ilias::vm */

#include <ilias/vm/anon-inl.h>

#endif /* _ILIAS_VM_ANON_H_ */
