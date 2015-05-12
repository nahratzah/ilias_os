#ifndef _ILIAS_VM_VMMAP_H_
#define _ILIAS_VM_VMMAP_H_

#include <ilias/arch.h>
#include <ilias/pmap/pmap.h>
#include <ilias/pmap/page.h>
#include <ilias/vm/page.h>
#include <ilias/linked_set.h>
#include <ilias/linked_list.h>
#include <vector>
#include <utility>
#include <algorithm>
#include <vector>
#include <ilias/stats-fwd.h>
#include <ilias/future.h>
#include <ilias/workq.h>
#include <ilias/refcnt.h>
#include <ilias/monitor.h>
#include <ilias/pmap/pmap_i386.h>
#include <ilias/pmap/pmap_amd64.h>

namespace ilias {
namespace vm {
namespace stats {

extern global_stats_group vmmap_group;
extern stats_counter vmmap_contention;

} /* namespace ilias::vm::stats */


using namespace std;
using namespace ilias::pmap;


struct vm_permission {
 private:
  enum class perm_type : unsigned char {
    none  = 0x00,
    read  = 0x01,
    write = 0x02,
    exec  = 0x04,
  };

 public:
  constexpr vm_permission(perm_type perm) noexcept : perm_(perm) {}
  constexpr vm_permission() = default;
  constexpr vm_permission(const vm_permission&) = default;
  vm_permission& operator=(const vm_permission&) = default;

  constexpr bool operator==(const vm_permission&) const noexcept;
  constexpr bool operator!=(const vm_permission&) const noexcept;

  constexpr vm_permission operator&(const vm_permission& y) const noexcept {
    return static_cast<perm_type>(static_cast<unsigned char>(perm_) &
                                  static_cast<unsigned char>(y.perm_));
  }

  constexpr vm_permission operator|(const vm_permission& y) const noexcept {
    return static_cast<perm_type>(static_cast<unsigned char>(perm_) |
                                  static_cast<unsigned char>(y.perm_));
  }

  constexpr vm_permission operator^(const vm_permission& y) const noexcept {
    return static_cast<perm_type>(static_cast<unsigned char>(perm_) ^
                                  static_cast<unsigned char>(y.perm_));
  }

  constexpr vm_permission operator~() const noexcept {
    constexpr auto mask = static_cast<unsigned char>(perm_type::read) |
                          static_cast<unsigned char>(perm_type::write) |
                          static_cast<unsigned char>(perm_type::exec);
    return static_cast<perm_type>(~static_cast<unsigned char>(perm_) & mask);
  }

  vm_permission& operator&=(const vm_permission&) noexcept;
  vm_permission& operator|=(const vm_permission&) noexcept;
  vm_permission& operator^=(const vm_permission&) noexcept;

  explicit constexpr operator bool() const noexcept;

  static constexpr perm_type perm_none_ = perm_type::none;
  static constexpr perm_type perm_read_ = perm_type::read;
  static constexpr perm_type perm_write_ = perm_type::write;
  static constexpr perm_type perm_exec_ = perm_type::exec;

 private:
  perm_type perm_ = perm_type::none;
};

enum class fork_style : unsigned char {
  copy,
  share
};

constexpr vm_permission vm_perm_none{ vm_permission::perm_none_ };
constexpr vm_permission vm_perm_read{ vm_permission::perm_read_ };
constexpr vm_permission vm_perm_write{ vm_permission::perm_write_ };
constexpr vm_permission vm_perm_exec{ vm_permission::perm_exec_ };
constexpr vm_permission vm_perm_r = vm_perm_read;
constexpr vm_permission vm_perm_w = vm_perm_write;
constexpr vm_permission vm_perm_x = vm_perm_exec;
constexpr vm_permission vm_perm_rw = vm_perm_r | vm_perm_w;
constexpr vm_permission vm_perm_rx = vm_perm_r | vm_perm_x;
constexpr vm_permission vm_perm_rwx = vm_perm_r | vm_perm_w | vm_perm_x;


class vmmap_entry;
using vmmap_entry_ptr = refpointer<vmmap_entry>;

class vmmap_entry
: public refcount_base<vmmap_entry>
{
 protected:
  vmmap_entry(workq_ptr wq) noexcept : wq_(move(wq)) {}
  vmmap_entry(const vmmap_entry& o) noexcept : wq_(move(o.wq_)) {}
  vmmap_entry(const vmmap_entry&, workq_ptr wq) noexcept : wq_(move(wq)) {}
  vmmap_entry(vmmap_entry&& o) noexcept : wq_(move(o.wq_)) {}
  vmmap_entry(vmmap_entry&&, workq_ptr wq) noexcept : wq_(move(wq)) {}
  vmmap_entry& operator=(const vmmap_entry&) noexcept { return *this; }
  vmmap_entry& operator=(vmmap_entry&&) noexcept { return *this; }

 public:
  virtual ~vmmap_entry() noexcept;

  virtual cb_future<tuple<page_ptr, monitor_token>> fault_read(
      monitor_token, shared_ptr<page_alloc>, page_count<native_arch>) = 0;
  virtual cb_future<tuple<page_ptr, monitor_token>> fault_write(
      monitor_token, shared_ptr<page_alloc>, page_count<native_arch>) = 0;

  virtual vmmap_entry_ptr clone() const = 0;
  virtual pair<vmmap_entry_ptr, vmmap_entry_ptr> split(page_count<native_arch>)
      const = 0;

  virtual vector<bool> mincore() const = 0;

  workq_ptr get_workq() const noexcept { return wq_; }

 private:
  workq_ptr wq_;
};

template<typename Impl, typename... Args>
vmmap_entry_ptr make_vmmap_entry(Args&&...);


template<arch Arch>
class vmmap_shard {
 public:
  using range = tuple<vpage_no<Arch>, page_count<Arch>>;

 private:
  struct addr_tag {};
  struct free_tag {};
  struct free_list_tag {};

  class entry
  : public linked_set_element<entry, addr_tag>,
    public linked_set_element<entry, free_tag>,
    public linked_list_element<free_list_tag>
  {
   public:
    entry() = default;
    entry(const entry&);
    entry(entry&&) noexcept;
    entry& operator=(const entry&);
    entry& operator=(entry&&) noexcept;
    entry(range, vpage_no<Arch>, vm_permission, vmmap_entry_ptr&&,
          fork_style = fork_style::copy) noexcept;

    range get_range_used() const noexcept;
    range get_range_free() const noexcept;
    vm_permission get_permission() const noexcept;

    vpage_no<Arch> get_addr_used() const noexcept;
    vpage_no<Arch> get_addr_free() const noexcept;
    vpage_no<Arch> get_addr_end() const noexcept;
    void update_end(vpage_no<Arch>) noexcept;
    bool unused() const noexcept;
    pair<unique_ptr<entry>, unique_ptr<entry>> split(vpage_no<Arch>) const;

    vmmap_entry& data() const noexcept;
    friend void swap(entry& x, entry& y) noexcept { swap(x.data_, y.data_); }

    cb_future<tuple<page_ptr, monitor_token>> fault_read(
	monitor_token, shared_ptr<page_alloc>, vpage_no<Arch>) noexcept;
    cb_future<tuple<page_ptr, monitor_token>> fault_write(
	monitor_token, shared_ptr<page_alloc>, vpage_no<Arch>) noexcept;

    fork_style get_fork_style() const noexcept;
    fork_style set_fork_style(fork_style s) noexcept;

    vector<bool> mincore() const;

   private:
    tuple<vpage_no<Arch>, page_count<Arch>, page_count<Arch>,
          vm_permission, vmmap_entry_ptr, fork_style> data_;
  };

  static void entry_deletor_(entry* e) noexcept { delete e; }

  struct isect_ {
    isect_(vpage_no<Arch> b, vpage_no<Arch> e) noexcept : begin(b), end(e) {}
    isect_(range r) noexcept : isect_(get<0>(r), get<0>(r) + get<1>(r)) {}

    vpage_no<Arch> begin;
    vpage_no<Arch> end;
  };

  struct entry_before_ {
    bool operator()(const entry&, const entry&) const noexcept;
    bool operator()(const entry&, const isect_&) const noexcept;
    bool operator()(const isect_&, const entry&) const noexcept;
  };

  struct free_before_ {
   private:
    static page_count<Arch> unwrap_(const entry&) noexcept;
    static const page_count<Arch>& unwrap_(const page_count<Arch>&)
        noexcept;

   public:
    template<typename T, typename U> bool operator()(const T&, const U&)
        const noexcept;
  };

  using entries_type = linked_set<entry, addr_tag, entry_before_>;
  using free_type = linked_set<entry, free_tag, free_before_>;
  using free_list = linked_list<entry, free_list_tag>;

 public:
  vmmap_shard() = default;
  vmmap_shard(const vmmap_shard&);
  vmmap_shard(vmmap_shard&&) noexcept;
  explicit vmmap_shard(range);
  vmmap_shard(vpage_no<Arch>, page_count<Arch>);
  vmmap_shard(vpage_no<Arch>, vpage_no<Arch>);
  ~vmmap_shard() noexcept;

  vmmap_shard& operator=(const vmmap_shard&);
  vmmap_shard& operator=(vmmap_shard&&) noexcept;
  template<arch A> friend void swap(vmmap_shard<A>&, vmmap_shard<A>&) noexcept;

  range get_range() const noexcept;

  void manage(range);
  void manage(vpage_no<Arch>, page_count<Arch>);
  void manage(vpage_no<Arch>, vpage_no<Arch>);

  static range intersect(range, range);

  void map(range, vm_permission, vmmap_entry_ptr&&);

  page_count<Arch> free_size() const noexcept { return npg_free_; }
  page_count<Arch> largest_free_size() const noexcept;
  vector<bool> mincore(vpage_no<Arch>, vpage_no<Arch>) const;

  void merge(vmmap_shard&&) noexcept;
  template<typename Iter> void fanout(Iter, Iter) noexcept;

  cb_future<tuple<page_ptr, monitor_token>> fault_read(
      monitor_token, shared_ptr<page_alloc>, vpage_no<Arch>);
  cb_future<tuple<page_ptr, monitor_token>> fault_write(
      monitor_token, shared_ptr<page_alloc>, vpage_no<Arch>);

 private:
  void map_link_(unique_ptr<entry>&&);
  typename entries_type::iterator link_(unique_ptr<entry>&&) noexcept;
  unique_ptr<entry> unlink_(entry*) noexcept;
  unique_ptr<entry> unlink_(typename entries_type::const_iterator) noexcept;
  template<typename Fn, typename... Args> void free_update_(entry&, Fn,
                                                            Args&&...)
      noexcept;

  void ensure_no_gaps_(tuple<typename entries_type::iterator,
                             typename entries_type::iterator>) const;
  pair<typename entries_type::iterator, typename entries_type::iterator>
      split_(range);

  entry* find_entry_for_addr_(vpage_no<Arch>) noexcept;

 public:
  template<typename T>
  static cb_future<T> efault_future_(vpage_no<Arch>);

 private:
  entries_type entries_;
  free_type free_;
  free_list free_list_;
  page_count<Arch> npg_free_ = page_count<Arch>(0);
};


template<arch Arch>
class vmmap {
 private:
  using shard_list = vector<vmmap_shard<Arch>>;

 public:
  explicit vmmap(pmap_support<Arch>&, shared_ptr<page_alloc>, workq_service&);
  vmmap(pmap_support<Arch>&, shared_ptr<page_alloc>, workq_service&,
        vpage_no<Arch>, vpage_no<Arch>);
  ~vmmap() noexcept = default;

  cb_future<void> reshard(size_t, size_t);

  cb_future<void> fault_read(vpage_no<Arch>);
  cb_future<void> fault_write(vpage_no<Arch>);
  cb_future<vector<bool>> mincore(vpage_no<Arch>, vpage_no<Arch>) const;

 private:
  typename shard_list::iterator find_shard_locked_(vpage_no<Arch>) noexcept;
  static bool shard_free_less_(const vmmap_shard<Arch>&,
                               const vmmap_shard<Arch>&) noexcept;

  typename shard_list::iterator heap_begin() noexcept;
  typename shard_list::const_iterator heap_begin() const noexcept;
  typename shard_list::iterator heap_end() noexcept;
  typename shard_list::const_iterator heap_end() const noexcept;
  bool heap_empty() const noexcept;

  cb_future<void> swap_slot_(size_t) noexcept;  // With lock held.
  void reshard_(monitor_token, size_t, size_t);

  pmap<Arch> pmap_;
  mutable monitor avail_guard_;
  shard_list avail_;
  typename shard_list::size_type in_use_ = 0;

  shared_ptr<page_alloc> pga_;
  workq_ptr wq_;
};


#if defined(__i386__) || defined(__amd64__) || defined(__x86_64__)
extern template class vmmap<arch::i386>;
#endif
#if defined(__amd64__) || defined(__x86_64__)
extern template class vmmap<arch::amd64>;
#endif


}} /* namespace ilias::vm */

#include <ilias/vm/vmmap-inl.h>

#endif /* _ILIAS_VM_VMMAP_H_ */
