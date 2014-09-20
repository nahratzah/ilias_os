#ifndef _ILIAS_VM_ANON_H_
#define _ILIAS_VM_ANON_H_

#include <ilias/vm/vmmap.h>
#include <memory>
#include <vector>
#include <mutex>
#include <ilias/refcnt.h>
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

    future<page_ptr> fault(shared_ptr<page_alloc>, workq_ptr);
    bool present() const noexcept;
    future<page_ptr> assign(workq_ptr, future<page_ptr>);

   private:
    future<page_ptr> assign_locked_(workq_ptr, unique_lock<mutex>&&,
                                    future<page_ptr>);
    void allocation_callback_(future<page_ptr>) noexcept;

    page_ptr release_urgent(page_owner::offset_type, page&) override;

    mutable mutex guard_;
    page_ptr page_ = nullptr;
    promise<page_ptr> in_progress_;
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

  future<page_ptr> fault_read(shared_ptr<page_alloc>,
                              page_count<native_arch>) override;
  future<page_ptr> fault_write(shared_ptr<page_alloc>,
                               page_count<native_arch>) override;
  future<page_ptr> fault_assign(page_count<native_arch>, future<page_ptr>);
  vector<bool> mincore() const override;

  vmmap_entry_ptr clone() const override;
  pair<vmmap_entry_ptr, vmmap_entry_ptr> split(
      page_count<native_arch>) const override;
  pair<anon_vme, anon_vme> split_no_alloc(page_count<native_arch>) const;

 private:
  future<page_ptr> fault_rw_(shared_ptr<page_alloc>, page_count<native_arch>);

  data_type data_;
};


}} /* namespace ilias::vm */

#include <ilias/vm/anon-inl.h>

#endif /* _ILIAS_VM_ANON_H_ */
