#include <abi/ext/heap.h>
#include <abi/ext/hash_set.h>
#include <abi/semaphore.h>
#include <abi/hashcode.h>
#include <abi/ext/log2.h>
#include <abi/panic.h>
#include <mutex>
#include <new>

namespace __cxxabiv1 {
namespace ext {
namespace {


class global_heap {
 private:
  struct used_tag {};
  struct chain_tag {};
  struct free_tag {};
  static constexpr unsigned int n_used_buckets = 256;

  global_heap() = default;
  global_heap(const global_heap&) = delete;
  global_heap& operator=(const global_heap&) = delete;

 public:
  using span = _namespace(std)::tuple<const void*, size_t>;

  class memory
  : public list_elem<used_tag>,
    public list_elem<chain_tag>,
    public list_elem<free_tag>
  {
   public:
    memory() = delete;
    memory(const memory&) = delete;
    memory& operator=(const memory&) = delete;

    memory(size_t, bool) noexcept;
    ~memory() noexcept;

    span get_span_space() const noexcept;
    span get_base_space() const noexcept;
    span get_used_space() const noexcept;
    span get_free_space() const noexcept;
    bool has_used() const noexcept { return used_; }
    bool is_root() const noexcept { return root_; }

    bool try_claim(const void*, size_t) noexcept;
    bool try_claim(uintptr_t, size_t) noexcept;
    memory* try_split(const void*, size_t) noexcept;
    memory* try_split(uintptr_t, size_t) noexcept;
    void release() noexcept;
    bool resize(size_t) noexcept;
    static bool pred_merge(memory*, memory*) noexcept;

    static constexpr unsigned int used_bits = 30;
    static constexpr size_t max_alloc_size = (size_t(1) << used_bits) - 1U;

   private:
    const bool root_ : 1;
    bool used_ : 1;  // Keep track of allocation:
                     // bytes_used_ will be 0 for 0-sized allocations.
    size_t bytes_used_ : used_bits;
    size_t bytes_free_ : 32;
  };

  /* Fix alignment of address. */
  static uintptr_t align(uintptr_t, size_t) noexcept;
  static const void* align(const void*, size_t) noexcept;
  static void* align(void*, size_t) noexcept;

  /* Size and alignment of memory unit. */
  static constexpr size_t memory_align = alignof(memory);
  static const size_t memory_alloc_space;

  void* allocate(size_t, size_t) noexcept;
  _namespace(std)::tuple<bool, size_t> free(const void*) noexcept;
  _namespace(std)::tuple<bool, size_t> resize(const void*, size_t) noexcept;

  static global_heap& get_singleton() noexcept;

 private:
  const memory* lookup_used_addr_(const void*) const noexcept;
  memory* lookup_used_addr_(const void*) noexcept;
  const memory* try_claim_(memory*, size_t, size_t) noexcept;
  const memory* allocate_from_free_(size_t, size_t) noexcept;

  using used_set = hash_set<memory, n_used_buckets, used_tag>;
  using chain = list<memory, chain_tag>;
  using free_set = list<memory, free_tag>;

  semaphore lock_{ 1U };
  used_set used_;
  chain chain_;
  free_set free_;
};


inline size_t hash_code(const global_heap::memory& m) noexcept {
  using abi::hash_code;
  using _namespace(std)::get;

  return hash_code(get<0>(m.get_used_space()));
}


global_heap::memory::memory(size_t span_bytes, bool root) noexcept
: root_(root),
  used_(false),
  bytes_used_(0),
  bytes_free_(span_bytes - memory_alloc_space)
{
  assert(span_bytes >= memory_alloc_space);
}

global_heap::memory::~memory() noexcept {
  assert(!has_used());
  assert(bytes_used_ == 0);
  assert(!chain::is_linked(this));
  assert(!used_set::is_linked(this));
  assert(!free_set::is_linked(this));
}

auto global_heap::memory::get_span_space() const noexcept -> span {
  using _namespace(std)::tie;
  using _namespace(std)::ignore;
  using _namespace(std)::make_tuple;

  const void* base_addr;
  size_t base;
  size_t used;
  size_t free;

  tie(base_addr, base) = get_base_space();
  tie(ignore, used) = get_used_space();
  tie(ignore, free) = get_free_space();
  return make_tuple(base_addr, base + used + free);
}

auto global_heap::memory::get_base_space() const noexcept -> span {
  using _namespace(std)::make_tuple;

  return make_tuple(this, memory_alloc_space);
}

auto global_heap::memory::get_used_space() const noexcept -> span {
  using _namespace(std)::tie;
  using _namespace(std)::make_tuple;

  const void* base_addr;
  size_t off;

  tie(base_addr, off) = get_base_space();
  return make_tuple(reinterpret_cast<const uint8_t*>(base_addr) + off,
                    bytes_used_);
}

auto global_heap::memory::get_free_space() const noexcept -> span {
  using _namespace(std)::tie;
  using _namespace(std)::make_tuple;

  const void* base_addr;
  size_t off;

  tie(base_addr, off) = get_used_space();
  return make_tuple(reinterpret_cast<const uint8_t*>(base_addr) + off,
                    bytes_free_);
}

auto global_heap::memory::try_claim(const void* addr, size_t sz) noexcept ->
    bool {
  using _namespace(std)::tie;

  const void* fp_addr;
  size_t fp_size;
  tie(fp_addr, fp_size) = get_free_space();

  if (sz > max_alloc_size) return false;  // Too large.
  if (has_used()) return false;  // This is already keeping track of an alloc.
  if (fp_addr != addr) return false;  // Address mismatch.
  if (fp_size < sz) return false;  // Insufficient space.

  used_ = true;
  bytes_used_ += sz;
  bytes_free_ -= sz;
  return true;
}

auto global_heap::memory::try_claim(uintptr_t addr, size_t sz) noexcept ->
    bool {
  return try_claim(reinterpret_cast<const void*>(addr), sz);
}

auto global_heap::memory::try_split(const void* addr, size_t sz) noexcept ->
    memory* {
  using _namespace(std)::tie;
  using _namespace(std)::get;

  assert(align(addr, memory_align) == addr);

  void* m_addr = reinterpret_cast<void*>(
      reinterpret_cast<uintptr_t>(addr) - memory_alloc_space);
  const void* fp_addr;
  size_t fp_size;
  tie(fp_addr, fp_size) = get_free_space();
  if (!(m_addr >= fp_addr)) return nullptr;
  const size_t off = reinterpret_cast<const uint8_t*>(m_addr) -
                     reinterpret_cast<const uint8_t*>(fp_addr);

  /* Create new memory segment. */
  memory* m = new (m_addr) memory(fp_size - off, false);
  if (m->try_claim(addr, sz)) {
    bytes_free_ -= get<1>(m->get_span_space());
    return m;
  }

  /* Insufficient space in allocation. */
  m->~memory();
  return nullptr;
}

auto global_heap::memory::try_split(uintptr_t addr, size_t sz) noexcept ->
    memory* {
  return try_split(reinterpret_cast<const void*>(addr), sz);
}

auto global_heap::memory::release() noexcept -> void {
  assert(has_used());

  used_ = false;
  bytes_free_ += bytes_used_;
  bytes_used_ = 0;
}

auto global_heap::memory::resize(size_t nsz) noexcept -> bool {
  assert(has_used());

  if (nsz > max_alloc_size) return false;  // Too large.
  if (bytes_used_ + bytes_free_ >= nsz) {
    bytes_free_ = bytes_free_ + bytes_used_ - nsz;
    bytes_used_ = nsz;
    return true;
  }
  return false;
}

auto global_heap::memory::pred_merge(memory* p, memory* s) noexcept -> bool {
  using _namespace(std)::tie;
  using _namespace(std)::ignore;

  assert(s != nullptr);
  assert(p != nullptr || s->is_root());
  if (s->has_used()) return false;
  if (s->is_root()) return false;

  const void* p_addr;
  size_t p_size;
  tie(p_addr, p_size) = p->get_span_space();

  /* Non-root spans must always connect with their predecessor. */
  assert(static_cast<const void*>(reinterpret_cast<const uint8_t*>(p_addr) +
                                  p_size) == s);

  size_t s_size;
  tie(ignore, s_size) = s->get_span_space();
  p->bytes_free_ += s_size;
  return true;
}


auto global_heap::align(uintptr_t addr, size_t alignment) noexcept ->
    uintptr_t {
  assert(is_pow2(alignment));

  if (alignment == 0) alignment = 1;
  return (addr + (alignment - 1U)) & ~uintptr_t(alignment - 1U);
}

auto global_heap::align(const void* addr, size_t alignment) noexcept ->
    const void* {
  return reinterpret_cast<const void*>(
      align(reinterpret_cast<uintptr_t>(addr), alignment));
}

auto global_heap::align(void* addr, size_t alignment) noexcept -> void* {
  return reinterpret_cast<void*>(
      align(reinterpret_cast<uintptr_t>(addr), alignment));
}

auto global_heap::allocate(size_t sz, size_t alignment) noexcept -> void* {
  using _namespace(std)::tie;
  using _namespace(std)::get;

  if (sz > memory::max_alloc_size) return nullptr;  // Too large.

  semlock l{ lock_ };

  for (;;) {
    /* Try to allocate from this arena. */
    {
      const memory* m = allocate_from_free_(sz, alignment);
      if (m) return const_cast<void*>(get<0>(m->get_used_space()));
    }

    /* Ask for more memory from the heap allocator. */
    const size_t request_h_size = align(memory_alloc_space + sz, alignment);
    void* h_addr;
    size_t h_size;
    tie(h_addr, h_size) = l.do_unlocked(&_config::heap_malloc, request_h_size);
    if (!h_addr) return nullptr;  // Nope, can't get more memory to manage.
    if (h_size < request_h_size) {
      panic("abi::_config::heap_malloc yields less space (%zu) "
            "than requested (%zu)", h_size, request_h_size);
    }

    /*
     * Link memory into global heap;
     * note that we link at the front of the free list,
     * so the second iteration of this loop will succeed quickly.
     */
    memory* new_mem = new (h_addr) memory(h_size, true);
    chain_.link_back(new_mem);
    free_.link_front(new_mem);
  }
  /* UNREACHABLE */
}

auto global_heap::free(const void* p) noexcept ->
    _namespace(std)::tuple<bool, size_t> {
  using _namespace(std)::make_tuple;
  using _namespace(std)::tie;

  semlock l{ lock_ };

  /* Lookup memory segment holding the allocation. */
  memory* m = lookup_used_addr_(p);
  if (!m) return make_tuple(false, 0);
  assert(m->has_used());

  /* Check address and size of used space. */
  const void* u_addr;
  size_t u_size;
  tie(u_addr, u_size) = m->get_used_space();
  assert(u_addr == p);

  /* Lookup predecessor of m. */
  auto m_iter = chain_.iterator_to(m);
  auto p_iter = _namespace(std)::prev(m_iter);
  memory* predecessor = (m_iter == chain_.begin() ? nullptr : &*p_iter);

  /* Release memory and try to merge it with its predecessor. */
  m->release();
  if (memory::pred_merge(predecessor, m)) {
    chain_.unlink(m);
    used_.unlink(m);
    free_.unlink(m);
    m->~memory();
    free_.link_front(predecessor);
  } else {
    free_.link_front(m);
  }
  return make_tuple(true, u_size);
}

auto global_heap::resize(const void* p, size_t nsz) noexcept ->
    _namespace(std)::tuple<bool, size_t> {
  using _namespace(std)::tie;
  using _namespace(std)::get;
  using _namespace(std)::make_tuple;

  semlock l{ lock_ };

  /* Lookup memory segment holding the allocation. */
  memory* m = lookup_used_addr_(p);
  if (!m) return make_tuple(false, 0);
  assert(m->has_used());

  /* Check old size (for return value). */
  size_t osz;
  const void* u_addr;
  tie(u_addr, osz) = m->get_used_space();
  assert(u_addr == p);

  /* Try to resize m. */
  if (m->resize(nsz)) {
    if (get<1>(m->get_free_space()) >= memory_alloc_space)
      free_.link_back(m);
    else
      free_.unlink(m);
    return make_tuple(true, osz);
  }
  return make_tuple(false, osz);
}

auto global_heap::get_singleton() noexcept -> global_heap& {
  static _namespace(std)::once_flag guard;
  static _namespace(std)::aligned_storage_t<sizeof(global_heap),
                                            alignof(global_heap)> data;

  void* data_ptr = reinterpret_cast<void*>(&data);
  _namespace(std)::call_once(guard,
                             [](void* ptr) { new (ptr) global_heap; },
                             data_ptr);
  return *static_cast<global_heap*>(data_ptr);
}

auto global_heap::lookup_used_addr_(const void* p) const noexcept ->
    const memory* {
  using abi::hash_code;
  using _namespace(std)::get;

  for (const auto& i :
       used_.get_bucket(used_set::hashcode_2_bucket(hash_code(p))))
    if (get<0>(i.get_used_space()) == p) return &i;
  return nullptr;
}

auto global_heap::lookup_used_addr_(const void* p) noexcept ->
    memory* {
  const global_heap& self = *this;  // To force calling of const member.
  return const_cast<memory*>(self.lookup_used_addr_(p));
}

auto global_heap::try_claim_(memory* m, size_t sz, size_t alignment)
    noexcept -> const memory* {
  using _namespace(std)::tie;
  using _namespace(std)::get;

  if (alignment <= 0) alignment = 1;
  assert(is_pow2(alignment));

  uintptr_t fp_addr;
  size_t fp_size;

  /* Load free space in m. */
  {
    const void* fp;
    tie(fp, fp_size) = m->get_free_space();
    fp_addr = reinterpret_cast<uintptr_t>(fp);
  }

  /* Align allocation address. */
  uintptr_t alloc_addr = align(fp_addr, alignment);

  /* Try to make memory argument handle the space directly. */
  if (m->try_claim(alloc_addr, sz)) {
    used_.link_front(m);
    if (get<1>(m->get_free_space()) <= memory_alloc_space) free_.unlink(m);
    return m;
  }

  /*
   * Split m, creating a second memory segment that will keep track of this
   * allocation.
   *
   * Layout:
   * - free space in m
   * - new memory segment  --  mm
   * - allocated data  --  alloc_addr
   */
  alloc_addr = align(align(fp_addr, memory_align) + memory_alloc_space,
                     alignment);
  memory* mm = m->try_split(alloc_addr, sz);
  if (mm) {
    /* Link mm into the maintained lists. */
    assert(get<0>(mm->get_used_space()) ==
           reinterpret_cast<const void*>(alloc_addr));
    chain_.link_after(mm, chain_.iterator_to(m));
    used_.link_front(mm);
    if (get<1>(mm->get_free_space()) >= memory_alloc_space)
      free_.link_front(mm);

    /* Unlink m from free list if it is no longer suitable for allocation. */
    if (get<1>(m->get_free_space()) < memory_alloc_space ||
        (m->has_used() && get<1>(m->get_free_space()) == memory_alloc_space))
      free_.unlink(m);
  }
  return mm;
}

auto global_heap::allocate_from_free_(size_t sz, size_t alignment) noexcept ->
    const memory* {
  for (auto& m : free_) {
    const memory* mm = try_claim_(&m, sz, alignment);
    if (mm) return mm;
  }
  return nullptr;
}

const size_t global_heap::memory_alloc_space =
    global_heap::align(sizeof(global_heap::memory), global_heap::memory_align);


} /* namespace __cxxabiv1::ext::<unnamed> */


struct heap::all_stats {
 private:
  all_stats() = default;

 public:
  static all_stats& get_singleton() noexcept;
  void do_register(stats_data&) noexcept;
  void do_deregister(stats_data&) noexcept;
  stats_collection get_all(stats_collection = stats_collection()) const;

 private:
  mutable semaphore lock_{ 1U };
  list<heap::stats_data, void> data_;
};

auto heap::all_stats::get_singleton() noexcept -> heap::all_stats& {
  _namespace(std)::once_flag guard;
  _namespace(std)::aligned_storage_t<sizeof(all_stats),
                                     alignof(all_stats)> data;

  void* data_ptr = reinterpret_cast<void*>(&data);
  _namespace(std)::call_once(guard,
                             [](void* ptr) { new (ptr) all_stats; },
                             data_ptr);
  return *static_cast<all_stats*>(data_ptr);
}

auto heap::all_stats::do_register(stats_data& sd) noexcept -> void {
  semlock l = semlock(lock_);
  data_.link_back(&sd);
}

auto heap::all_stats::do_deregister(stats_data& sd) noexcept -> void {
  semlock l = semlock(lock_);
  data_.unlink(&sd);
}

auto heap::all_stats::get_all(stats_collection c) const -> stats_collection {
  c.clear();

  semlock l = semlock(lock_);
  for (const auto& i : data_) c.emplace_back(i);
  return c;
}


heap::heap(_namespace(std)::string_ref name) noexcept
: stats_(name)
{
  all_stats::get_singleton().do_register(stats_);
}

heap::~heap() noexcept {
  all_stats::get_singleton().do_deregister(stats_);
}

auto heap::get_stats(stats_collection c) -> stats_collection {
  using _namespace(std)::move;

  return all_stats::get_singleton().get_all(move(c));
}

auto heap::malloc(size_t sz, size_t align) noexcept -> void* {
  using _namespace(std)::min;

  const size_t args = sz;
  void*const rv = global_heap::get_singleton().allocate(sz, align);
  return malloc_result(rv, args);
}

auto heap::malloc(size_t sz) noexcept -> void* {
  size_t align = std::min(size_t(1) << log2_down(sz), alignof(max_align_t));
  return malloc(sz, align);
}

auto heap::free(const void* p) noexcept -> void {
  using _namespace(std)::tie;

  const void* args = p;
  if (_predict_false(p == nullptr)) {
    free_result(0, args);
    return;
  }

  bool succes;
  size_t size;
  tie(succes, size) = global_heap::get_singleton().free(p);
  if (!succes) panic("heap::free for %p, which is not allocated.", args);
  free_result(size, args);
}

auto heap::resize(const void* p, size_t nsz) noexcept ->
    _namespace(std)::tuple<bool, size_t> {
  using _namespace(std)::make_tuple;

  const auto args = make_tuple(p, nsz);
  return resize_result(global_heap::get_singleton().resize(p, nsz), args);
}

auto heap::get_stats() -> stats_collection {
  return all_stats::get_singleton().get_all();
}

auto heap::malloc_result(void* p, size_t sz) noexcept -> void* {
  using _namespace(std)::memory_order_relaxed;

  stats_.malloc_calls_.fetch_add(1U, memory_order_relaxed);
  if (p)
    stats_.malloc_bytes_.fetch_add(sz, memory_order_relaxed);
  else
    stats_.malloc_fail_.fetch_add(1U, memory_order_relaxed);
  return p;
}

auto heap::resize_result(_namespace(std)::tuple<bool, size_t> rv,
                         _namespace(std)::tuple<const void*, size_t> args)
    noexcept -> _namespace(std)::tuple<bool, size_t> {
  using _namespace(std)::get;
  using _namespace(std)::memory_order_relaxed;

  auto& old_sz = get<1>(rv);
  auto& new_sz = get<1>(args);

  stats_.resize_calls_.fetch_add(1U, memory_order_relaxed);
  if (!get<0>(rv))
    stats_.resize_fail_.fetch_add(1U, memory_order_relaxed);
  else if (new_sz > old_sz)
    stats_.resize_bytes_up_.fetch_add(new_sz - old_sz, memory_order_relaxed);
  else if (new_sz < old_sz)
    stats_.resize_bytes_down_.fetch_add(old_sz - new_sz, memory_order_relaxed);
  return rv;
}

auto heap::free_result(size_t sz, const void* arg) noexcept -> void {
  using _namespace(std)::memory_order_relaxed;

  stats_.free_calls_.fetch_add(1U, memory_order_relaxed);
  if (arg)
    stats_.free_bytes_.fetch_add(sz, memory_order_relaxed);
}


}} /* namespace __cxxabiv1::ext */
