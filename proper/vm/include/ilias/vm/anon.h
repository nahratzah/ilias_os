#ifndef _ILIAS_VM_ANON_H_
#define _ILIAS_VM_ANON_H_

#include <ilias/vm/vmmap.h>
#include <memory>
#include <vector>
#include <mutex>
#include <ilias/refcnt.h>

namespace ilias {
namespace vm {

using namespace std;


class anon_vme
: public vmmap_entry
{
 private:
  class entry
  : public refcount_base<entry>
  {
   public:
    entry() = default;
    entry(const entry&) = delete;
    entry(entry&&) = delete;
    entry& operator=(const entry&) = delete;
    entry& operator=(entry&&) = delete;

    future<page_refptr> fault(shared_ptr<page_alloc>, workq_ptr);
    bool present() const noexcept;
    future<page_refptr> assign(workq_ptr, future<page_refptr>);

   private:
    future<page_refptr> assign_locked_(workq_ptr, unique_lock<mutex>&&,
                                    future<page_refptr>);
    void allocation_callback_(future<page_refptr>) noexcept;

    mutable mutex guard_;
    page_refptr page_ = nullptr;
    promise<page_refptr> in_progress_;
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

  future<page_refptr> fault_read(shared_ptr<page_alloc>,
                              page_count<native_arch>) override;
  future<page_refptr> fault_write(shared_ptr<page_alloc>,
                               page_count<native_arch>) override;
  future<page_refptr> fault_assign(page_count<native_arch>, future<page_refptr>);

  vmmap_entry_ptr clone() const override;
  pair<vmmap_entry_ptr, vmmap_entry_ptr> split(
      page_count<native_arch>) const override;
  pair<anon_vme, anon_vme> split_no_alloc(page_count<native_arch>) const;

 private:
  data_type data_;
};


}} /* namespace ilias::vm */

#include <ilias/vm/anon-inl.h>

#endif /* _ILIAS_VM_ANON_H_ */
