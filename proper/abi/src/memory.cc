#include <memory>
#include <array>
#include <mutex>
#include <stdimpl/stats.h>
#include <ilias/stats.h>
#include <abi/ext/log2.h>
#include <abi/memory.h>

using _namespace(ilias)::global_stats_group;
using _namespace(ilias)::stats_counter;

_namespace_begin(std)


void* align(size_t alignment, size_t size, void*& ptr, size_t& space) {
  if (ptr == nullptr) return nullptr;
  if (size == 0) size = 1;
  if (alignment == 0) alignment = 1;
  if (!abi::ext::is_pow2(alignment)) return nullptr;
  if (size > space) return nullptr;

  const uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
  const uintptr_t aligned_addr = (addr + alignment - 1U) &
                                 ~uintptr_t(alignment);
  if (aligned_addr - addr > space - size) return nullptr;

  space -= size + (aligned_addr - addr);
  ptr = reinterpret_cast<void*>(aligned_addr + size);
  return reinterpret_cast<void*>(aligned_addr);
}


bad_weak_ptr::bad_weak_ptr() noexcept {}
bad_weak_ptr::~bad_weak_ptr() noexcept {}
const char* bad_weak_ptr::what() const noexcept { return "bad_weak_ptr"; }


namespace impl {
namespace {

global_stats_group cache_group{ &std_stats, "temporary_buffer", {}, {} };

#if defined(_LOADER)
constexpr size_t N_temporary_storage_cache = 2;
#elif defined(_KERNEL)
constexpr size_t N_temporary_storage_cache = 16;
#else
constexpr size_t N_temporary_storage_cache = 64;
#endif
#if defined(_LOADER) || defined(_KERNEL)
constexpr size_t N_assigned_cache = 8 * N_temporary_storage_cache;
#else
constexpr size_t N_assigned_cache = 64 * N_temporary_storage_cache;
#endif

abi::big_heap& get_temporary_heap() noexcept {
  using abi::big_heap;

  static once_flag guard;
  static aligned_storage_t<sizeof(big_heap), alignof(big_heap)> data;

  void* data_ptr = &data;
  call_once(guard,
            [](void* ptr) { new (ptr) big_heap("temporary_buffer"); },
            data_ptr);
  return *static_cast<big_heap*>(data_ptr);
}


struct temporary_buffer_release {
  void operator()(const void* p) noexcept {
    get_temporary_heap().free(p);
  }
};

using temporary_buffer_ptr = unique_ptr<void, temporary_buffer_release>;

/* Cache a few recently released blocks of memory. */
using temporary_cache = array<pair<temporary_buffer_ptr, size_t>,
                              N_temporary_storage_cache>;
temporary_cache& cache() noexcept {
  static thread_local temporary_cache impl;
  return impl;
}

/* Remember a couple of recent assignments. */
using temporary_assigned = array<pair<void*, size_t>, N_assigned_cache>;
thread_local temporary_assigned assigned;

bool satisfies_constraints(temporary_cache::reference item,
                           size_t size, size_t align, bool try_realloc) {
  if (!item.first) return false;

  uintptr_t addr = reinterpret_cast<uintptr_t>(item.first.get());
  if (align == 0 || (addr & (align - 1U)) == 0) {
    if (item.second >= size) return true;
    if (try_realloc &&
        get<0>(get_temporary_heap().resize(item.first.get(), size))) {
      item.second = size;
      return true;
    }
  }
  return false;
}

} /* namespace std::impl::<unnamed> */

pair<void*, size_t> temporary_buffer_allocate(size_t size, size_t align) {
  using placeholders::_1;

  static stats_counter cache_hit{ cache_group, "hit" };
  static stats_counter cache_hit2{ cache_group, "hit_after_resize" };
  static stats_counter cache_miss{ cache_group, "miss" };
  static stats_counter cache_alloc{ cache_group, "allocated" };

  assert(abi::ext::is_pow2(align));

  temporary_cache::iterator match;  // Cache hit.
  pair<void*, size_t> assign;  // Assignment.

  /* Search cache for an allocation that matches the request. */
  match = find_if(cache().begin(), cache().end(),
                  bind(&satisfies_constraints, _1, size, align, false));
  if (match != cache().end()) {
    cache_hit.add();
  } else {
    match = find_if(cache().begin(), cache().end(),
                    bind(&satisfies_constraints, _1, size, align, true));
    if (match != cache().end())
      cache_hit2.add();
    else
      cache_miss.add();
  }

  if (match != cache().end()) {
    /* Use found element from cache. */
    assign = make_pair(match->first.release(), match->second);
    if (next(match) != cache().end())
      rotate(match, next(match), cache().end());
  } else {
    /* Allocate from heap. */
    void* addr = get_temporary_heap().malloc(size, align);
    if (_predict_false(!addr)) return make_pair(nullptr, 0);
    assign = make_pair(addr, size);
  }

  /* Replace longest autstanding record. */
  cache_alloc.add();
  move_backward(assigned.begin(), prev(assigned.end()), assigned.end());
  assigned.front() = assign;
  return assign;
}

bool temporary_buffer_resize(const void* p, size_t new_size) noexcept {
  using placeholders::_1;

  if (p == nullptr) return false;
  auto assign = find_if(
      assigned.begin(), assigned.end(),
      bind(equal_to<const void*>(),
           bind<const void*>(&temporary_assigned::value_type::first, _1),
           p));
  if (assign != assigned.end() && assign->second >= new_size) return true;

  bool succes;
  tie(succes, ignore) = get_temporary_heap().resize(p, new_size);
  if (succes && assign != assigned.end())
    assign->second = new_size;

  return succes;
}

void temporary_buffer_deallocate(const void* p) {
  using placeholders::_1;
  using placeholders::_2;

  static stats_counter cache_alloc{ cache_group, "deallocated" };

  if (p == nullptr) return;
  temporary_buffer_ptr ptr{ const_cast<void*>(p) };
  cache_alloc.add();

  auto assign = find_if(
      assigned.begin(), assigned.end(),
      bind(equal_to<const void*>(),
           bind<const void*>(&temporary_assigned::value_type::first, _1),
           p));
  if (assign != assigned.end()) {
    /* Place assignment at the front of the cache. */
    move(cache().begin(), prev(cache().end()), next(cache().begin()));
    cache().front() = make_pair(move(ptr), assign->second);

    /* Remove assignment. */
    move(next(assign), assigned.end(), assign);
    assigned.back() = make_pair(nullptr, 0);
  }
}


} /* namespace std::impl */


_namespace_end(std)
