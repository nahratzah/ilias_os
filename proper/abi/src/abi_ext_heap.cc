#include <abi/ext/heap.h>
#include <abi/ext/hash_set.h>
#include <abi/ext/list.h>
#include <abi/ext/elide_destructor.h>
#include <abi/semaphore.h>
#include <abi/hashcode.h>
#include <abi/ext/log2.h>
#include <abi/panic.h>
#include <new>

namespace __cxxabiv1 {
namespace ext {
namespace {


void atom_inc(std::atomic<uintmax_t>& atom, uintmax_t n = 1) noexcept {
  atom.fetch_add(n, std::memory_order_relaxed);
}

void atom_dec(std::atomic<uintmax_t>& atom, uintmax_t n = 1) noexcept {
  atom.fetch_sub(n, std::memory_order_relaxed);
}

semaphore mlock{ 1U };


/* Round size up to the next multiple of alignof(max_align_t). */
constexpr size_t fix_size(size_t sz) {
  return ((sz == 0 ? 1U : sz) + alignof(max_align_t) - 1U) /
         alignof(max_align_t);
}


constexpr unsigned int n_buckets = 256;
constexpr size_t allocsz_bits = 31;
struct free_tag {};
struct addr_tag {};
struct lin_tag {};

struct meta
: list_elem<addr_tag>,
  list_elem<free_tag>,
  list_elem<lin_tag>
{
  size_t used_ : allocsz_bits;
  bool root_ : 1;
  size_t free_;

  meta(size_t free, bool root = false) : used_(0), root_(root), free_(free) {}
  inline void* get_used_ptr() const noexcept;
  inline void* get_free_ptr() const noexcept;
  inline void* get_end_ptr() const noexcept;
  inline unsigned int freelist_slot() const noexcept;
};

using abi::hash_code;

inline size_t hash_code(const meta& m) noexcept {
  return hash_code(m.get_used_ptr());
}

constexpr size_t meta_sz = fix_size(sizeof(meta));
constexpr size_t min_allocsz = 2U * (size_t(1U) << log2_up(meta_sz));
constexpr size_t max_allocsz = size_t(1U) << allocsz_bits;
constexpr unsigned int alloc_sz_log2 = log2_up(max_allocsz / min_allocsz);

using freelist_buckets = list<meta, free_tag>[alloc_sz_log2 + 1U];

hash_set<meta, n_buckets, addr_tag>& addrset() noexcept {
  static elide_destructor<hash_set<meta, n_buckets, addr_tag>> impl;
  return *impl;
}

freelist_buckets& freelist() {
  static elide_destructor<freelist_buckets> impl;
  return *impl;
}

list<meta, lin_tag>& linlist() noexcept {
  static elide_destructor<list<meta, lin_tag>> impl;
  return *impl;
}


constexpr unsigned int freelist_slot(size_t sz) {
  return (log2_up(sz / min_allocsz) > alloc_sz_log2 ?
          alloc_sz_log2 :
          log2_up(sz / min_allocsz));
}

inline unsigned int meta::freelist_slot() const noexcept {
  using __cxxabiv1::ext::freelist_slot;

  return freelist_slot(free_);
}

inline void* meta::get_used_ptr() const noexcept {
  const void* self = this;
  const auto addr = reinterpret_cast<uintptr_t>(self) + meta_sz;
  return reinterpret_cast<void*>(addr);
}

inline void* meta::get_free_ptr() const noexcept {
  const void* self = get_used_ptr();
  const auto addr = reinterpret_cast<uintptr_t>(self) + used_;
  return reinterpret_cast<void*>(addr);
}

inline void* meta::get_end_ptr() const noexcept {
  const void* self = get_free_ptr();
  const auto addr = reinterpret_cast<uintptr_t>(self) + free_;
  return reinterpret_cast<void*>(addr);
}


void add_memory(void* p, size_t sz) noexcept {
  meta* root = new (p) meta(sz - meta_sz, true);
  freelist()[root->freelist_slot()].link_front(root);
  addrset().link_front(root);
  linlist().link_front(root);
}

bool ask_for_memory(semlock& lock, size_t sz) noexcept {
  void*const p = lock.do_unlocked([&]() {
      return _config::heap_malloc(&sz, min_allocsz);
    });
  if (p) add_memory(p, sz);
  return bool(p);
}

meta* alloc_meta(semlock& lock, size_t sz) noexcept {
  sz = fix_size(sz);
  const size_t alloc_sz = sz + meta_sz;
  const unsigned int f_start = freelist_slot(alloc_sz);
  if (f_start >= alloc_sz_log2) return nullptr;

  do {
    for (unsigned int i = f_start; i <= alloc_sz_log2; ++i) {
      if (freelist()[i].empty()) continue;

      meta* m = &*freelist()[i].begin();
      freelist()[i].unlink(m);

      /*
       * Create a new meta, in the free space of m.
       * Since the actual allocation code uses the m pointer,
       * store the new meta in variable m.
       */
      if (m->used_ != 0) {
        meta* nwm = new (m->get_free_ptr()) meta(m->free_ - meta_sz);
        m->free_ = 0;
        freelist()[m->freelist_slot()].link_back(m);
        addrset().link_front(nwm);
        linlist().link_after(nwm, linlist().iterator_to(m));
        m = nwm;
      }

      m->used_ = sz;
      m->free_ -= sz;
      freelist()[m->freelist_slot()].link_front(m);
      return m;
    }
  } while (ask_for_memory(lock, alloc_sz));
  return nullptr;
}


} /* namespace __cxxabiv1::ext::<unnamed> */


using _config::heap_malloc;
using _config::heap_free;

void* heap::malloc_result(void* rv, size_t sz) noexcept {
  atom_inc(stats_.malloc_calls);
  if (rv)
    atom_inc(stats_.malloc_bytes, fix_size(sz));
  else
    atom_inc(stats_.malloc_fail);
  return rv;
}

void* heap::malloc(size_t sz) noexcept {
  semlock lock{ mlock };

  meta* s = alloc_meta(lock, sz);
  if (s == nullptr) return malloc_result(nullptr, sz);
  return malloc_result(s->get_used_ptr(), sz);
}

bool heap::resize_result(bool rv, void* addr, size_t nsz, size_t osz)
    noexcept {
  atom_inc(stats_.realloc_calls);
  if (rv) {
    if (nsz > osz)
      atom_inc(stats_.resize_bytes_up, nsz - osz);
    else if (nsz < osz)
      atom_inc(stats_.resize_bytes_down, osz - nsz);
  } else {
    atom_inc(stats_.resize_fail);
  }
  return rv;
}

bool heap::resize(void* addr, size_t nsz, size_t* osz) noexcept {
  nsz = fix_size(nsz);
  semlock lock{ mlock };

  auto bucket =
      addrset().get_bucket(addrset().hashcode_2_bucket(hash_code(addr)));
  for (auto& i : bucket) {
    if (i.get_used_ptr() == addr) {
      *osz = i.used_;
      if (*osz == nsz) return resize_result(true, addr, nsz, *osz);

      if (nsz < *osz) {
        freelist()[i.freelist_slot()].unlink(&i);
        i.free_ += *osz - nsz;
        i.used_ -= *osz - nsz;
        freelist()[i.freelist_slot()].link_front(&i);
        return resize_result(true, addr, nsz, *osz);
      }

      const auto delta = nsz - *osz;
      if (i.free_ < delta) return resize_result(false, addr, nsz, *osz);
      freelist()[i.freelist_slot()].unlink(&i);
      i.free_ -= delta;
      i.used_ += delta;
      freelist()[i.freelist_slot()].link_front(&i);
      return resize_result(true, addr, nsz, *osz);
    }
  }

  panic("heap::resize(%p) -- address not in allocation list", addr);
  for (;;);
}

void heap::free(void* addr) noexcept {
  semlock lock{ mlock };

  auto bucket =
      addrset().get_bucket(addrset().hashcode_2_bucket(hash_code(addr)));
  for (auto& i : bucket) {
    if (i.get_used_ptr() == addr) {
      atom_inc(stats_.free_bytes, i.used_);
      freelist()[i.freelist_slot()].unlink(&i);
      i.free_ -= i.used_;
      i.used_ = 0;
      freelist()[i.freelist_slot()].link_front(&i);

      // Merge with pred.
      if (!i.root_) {
        auto pred = --linlist().iterator_to(&i);
        if (&i == pred->get_end_ptr()) {
          pred->free_ += meta_sz + i.free_;

          linlist().unlink(&i);
          addrset().unlink(&i);
          freelist()[i.freelist_slot()].unlink(&i);
        }
      }

      atom_inc(stats_.free_calls);
      return;
    }
  }

  panic("heap::free(%p) -- address not in allocation list", addr);
}


}} /* namespace __cxxabiv1::ext */
