#include <abi/ext/heap.h>
#include <abi/ext/hash_list.h>
#include <abi/ext/list.h>
#include <abi/semaphore.h>
#include <abi/hashcode.h>
#include <abi/ext/log2.h>

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
constexpr unsigned int alloc_sz_log2 = 20;
struct free_tag {};
struct addr_tag {};

struct meta
: list_elem<addr_tag>
{
  size_t used_ = 0;
  size_t free_ = 0;
};

inline size_t hash_code(const meta& m) noexcept {
  using abi::hash_code;

  const void* p = &m;
  return hash_code(p);
}

hash_set<meta, n_buckets, addr_tag> addr;
list<meta, free_tag> free[alloc_sz_log2];


constexpr size_t meta_sz = fix_size(sizeof(meta));
constexpr size_t min_allocsz = 2U * (size_t(1U) << log2_up(meta_sz));
constexpr size_t max_allocsz = min_allocsz << alloc_sz_log2;

constexpr unsigned int freelist_slot(size_t sz) {
  return log2_up(sz / min_allocsz);
}


void add_memory(void* p, size_t sz) noexcept {
  meta* root = static_cast<meta*>(p);

  while (sz >= min_allocsz) {
    size_t immed = size_t(1) << log2_down(sz);
    meta* m = static_cast<meta*>(p);
    new (m) meta();

    m.root = p;
    m.sz_slot_ = freelist_slot(immed);
    m.addr_chain_ = free_lists[freelist_slot(immed)];
    free_lists[freelist_slot(immed)] = m;

    p = reinterpret_cast<uint8_t*>(p) + immed;
    sz -= immed;
  }
}

bool ask_for_memory(semlock& lock, size_t sz) noexcept {
  void*const p = lock.do_unlocked([&]() {
      return heap_malloc(&sz, min_allocsz);
    });
  if (p) add_memory(p, sz);
  return bool(p);
}

meta* find_free_meta(semlock& lock, size_t alloc_sz) noexcept {
  const unsigned int f_start = freelist_slot(alloc_sz);
  if (f_start >= alloc_sz_log2) return nullptr;

  do {
    for (unsigned int i = f_start; i < alloc_sz_log2; ++i) {
      meta* m = free_lists[i];
      if (!m) continue;

      /* Found available space. */
      if (!m.is_free_) panic("space on free list is not free: %p", m);
      /* Take m from freelist. */
      free_lists[i] = m.free_chain_;
      m.free_chain_ = nullptr;
      m.is_free_ = false;

      return m;
    }
  } while (ask_for_memory(lock, sz));
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

void heap::malloc(size_t sz) noexcept {
  sz = fix_size(sz);
  alloc_sz = sz + meta_sz;

  semlock lock{ mlock };

  meta* s = find_free_meta(lock, alloc_sz);
  if (s == nullptr) return malloc_result(nullptr, sz);
}


}} /* namespace __cxxabiv1::ext */
