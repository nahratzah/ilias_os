#ifndef _ILIAS_VM_ANON_H_
#define _ILIAS_VM_ANON_H_

#include <ilias/vm/vmmap.h>
#include <memory>
#include <vector>

namespace ilias {
namespace vm {

using namespace std;


class anon_vme
: public vmmap_entry
{
 private:
  class entry {
   public:
    entry() = default;
    entry(const entry&) = delete;
    entry(entry&&) = delete;
    entry& operator=(const entry&) = delete;
    entry& operator=(entry&&) = delete;

    mutable atomic<uintptr_t> refcnt_{ 1U };

    future<page_ptr> fault();

   private:
    page_ptr page_ = nullptr;
  };

  struct entry_deleter_ {
    void operator()(const entry* e) const noexcept;
  };

  using entry_ptr = unique_ptr<entry, entry_deleter_>;
  using data_type = vector<entry_ptr>;

 public:
  anon_vme() = delete;
  anon_vme(const anon_vme&);
  anon_vme(anon_vme&&) noexcept;
  anon_vme(page_count<native_arch> npg);
  template<typename Iter> anon_vme(Iter, Iter);
  ~anon_vme() noexcept override;

  bool empty() const noexcept;

  bool present(page_count<native_arch>) const noexcept;
  future<page_ptr> fault_read(page_count<native_arch>) override;
  future<page_ptr> fault_write(page_count<native_arch>) override;

  vmmap_entry_ptr clone() const override;
  pair<vmmap_entry_ptr, vmmap_entry_ptr> split(
      page_count<native_arch>) const override;
  pair<anon_vme, anon_vme> split_no_alloc(page_count<native_arch>) const;

 private:
  static entry_ptr copy_entry_(const entry_ptr&) noexcept;

  data_type data_;
};


}} /* namespace ilias::vm */

#include <ilias/vm/anon-inl.h>

#endif /* _ILIAS_VM_ANON_H_ */
