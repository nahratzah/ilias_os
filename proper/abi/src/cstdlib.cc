#include <cstdlib>
#include <abi/abi.h>
#include <abi/linker.h>
#include <abi/semaphore.h>
#include <abi/memory.h>
#include <abi/panic.h>
#include <cerrno>
#include <cstring>
#include <algorithm>
#include <functional>
#include <iterator>
#include <new>
#include <thread>

namespace std {

using abi::semlock;

/* at{_quick_}exit function implementation. */
namespace {


constexpr _TYPES(size_t) fn_count = 256;
abi::semaphore fn_lock{ 1U };


class fn_type {
 public:
  fn_type() = default;
  fn_type(const fn_type&) = default;
  fn_type& operator=(const fn_type&) = default;
  constexpr fn_type(void (*)(), bool);

  bool resolve(bool) noexcept;
  bool empty() const noexcept;

 private:
  void (*fn_)() = nullptr;
  bool is_quick_ = false;
};

fn_type fn_array[fn_count];

class fn_stack {
 public:
  static bool push(fn_type) noexcept;
  static inline bool empty() noexcept;
  static fn_type pop(bool* = nullptr) noexcept;

 private:
  explicit inline fn_stack(fn_type) noexcept;

  fn_type fn_;
  fn_stack* tail_ = nullptr;
  static fn_stack* head_;
  static abi::heap& heap;
};


constexpr fn_type::fn_type(void (*fn)(), bool is_quick)
: fn_(fn),
  is_quick_(is_quick)
{}

bool fn_type::resolve(bool quick_only) noexcept {
  if (!(fn_ && (is_quick_ == quick_only))) return false;
  (*fn_)();
  return true;
}

bool fn_type::empty() const noexcept {
  return !fn_;
}

inline fn_stack::fn_stack(fn_type fn) noexcept
: fn_(fn)
{}

bool fn_stack::empty() noexcept {
  return head_ == nullptr;
}

bool fn_stack::push(fn_type fn) noexcept {
  fn_stack* e = static_cast<fn_stack*>(heap.malloc(sizeof(fn_stack)));
  if (!e) return false;
  new (e) fn_stack(fn);

  semlock lck{ fn_lock };
  e->tail_ = head_;
  head_ = e;
  return true;
}

fn_type fn_stack::pop(bool* fail) noexcept {
  fn_stack* h = head_;
  if (h == nullptr) {
    if (fail) *fail = true;
    return fn_type();
  }

  head_ = h->tail_;
  fn_type rv = h->fn_;
  heap.free(h);
  if (fail) *fail = false;
  return rv;
}

int push(fn_type fn) noexcept {
  if (fn_stack::push(fn)) return 0;

  semlock lck{ fn_lock };
  if (!fn_stack::empty()) return _ABI_ENOMEM;
  for (fn_type* f = &fn_array[0]; f != &fn_array[fn_count]; ++f) {
    if (f->empty()) {
      *f = fn;
      return 0;
    }
  }
  return _ABI_ENOMEM;
}

void resolve(bool quick_only) noexcept {
  static abi::semaphore highlander{ 1 };
  highlander.decrement();  // Only once ever will this be invoked.

  bool keep_going = true;
  semlock lck{ fn_lock };
  while (keep_going) {
    keep_going = false;
    bool stop;
    do {
      fn_type fn = fn_stack::pop(&stop);
      if (!stop) {
        lck.do_unlocked([&]() {
            fn.resolve(quick_only);
	  });
      }
    } while (!stop);

    for (fn_type* f = &fn_array[fn_count - 1U]; f != &fn_array[0] - 1; --f) {
      fn_type fn = *f;
      *f = fn_type();
      if (lck.do_unlocked([&]() { return fn.resolve(quick_only); })) {
        keep_going = true;
	break;
      }
    }
  }

  if (!quick_only) abi::__cxa_finalize_0();
}


fn_stack* fn_stack::head_ = nullptr;

namespace {

::abi::heap& fn_stack_heap_singleton() {
  using _namespace(std)::aligned_storage_t;
  using ::abi::heap;

  static aligned_storage_t<sizeof(heap), alignof(heap)> data;
  void* data_ptr = &data;
  return *new (data_ptr) heap("abi/atexit");
}

}

::abi::heap& fn_stack::heap = fn_stack_heap_singleton();


abi::big_heap& c_malloc_heap() noexcept {
  using _namespace(std)::once_flag;
  using _namespace(std)::call_once;
  using _namespace(std)::aligned_storage_t;
  using ::abi::big_heap;

  static once_flag guard;
  static aligned_storage_t<sizeof(big_heap), alignof(big_heap)> data;

  void* data_ptr = &data;
  call_once(guard,
            [](void* ptr) { new (ptr) big_heap("abi/malloc"); },
            data_ptr);
  return *static_cast<big_heap*>(data_ptr);
}


} /* namespace std::<unnamed> */


void abort() noexcept {
  ::abi::_config::abort();
  abi::panic("abi::_config::abort() returned");
  for (;;);
}

void quick_exit(int s) noexcept {
  resolve(true);
  abi::_config::exit(s);
  abi::panic("abi::_config::exit() returned");
  for (;;);
}

void exit(int s) noexcept {
  resolve(false);
  abi::_config::exit(s);
  abi::panic("abi::_config::exit() returned");
  for (;;);
}

int atexit(void (*fn)()) noexcept {
  if (fn == nullptr) {
    errno = _ABI_EINVAL;
    return -1;
  }
  int e = push(fn_type(fn, false));
  if (e) {
    errno = e;
    return -1;
  }
  return 0;
}

int at_quick_exit(void (*fn)()) noexcept {
  if (fn == nullptr) {
    errno = _ABI_EINVAL;
    return -1;
  }
  int e = push(fn_type(fn, true));
  if (e) {
    errno = e;
    return -1;
  }
  return 0;
}


void* __attribute__((weak)) malloc(size_t sz) noexcept {
  return c_malloc_heap().malloc(sz);
}

void __attribute__((weak)) free(void* p) noexcept {
  c_malloc_heap().free(p);
}

void* __attribute__((weak)) realloc(void* p, size_t sz) noexcept {
  size_t oldsz;
  bool succes;
  auto& heap = c_malloc_heap();
  tie(succes, oldsz) = heap.resize(p, sz);
  if (succes) return p;

  /* Move memory. */
  if (sz < oldsz) oldsz = sz;
  void* q = heap.malloc(sz);
  if (!q) return nullptr;
  memcpy(q, p, oldsz);
  heap.free(p);
  return q;
}


/*
 * Stride iterator, to help in implementing qsort.
 */
namespace {

class stride_iterator;

class stride_reference {
  friend stride_iterator;

 public:
  const void* get() const noexcept { return addr_; }

  stride_reference() noexcept = default;
  stride_reference(const stride_reference&) noexcept = default;
  stride_reference& operator=(const stride_reference&) noexcept = default;

  stride_reference(void* addr, size_t stride) noexcept
  : addr_(static_cast<uint8_t*>(addr)),
    stride_(stride)
  {}

  stride_iterator operator&() const noexcept;

 private:
  uint8_t* addr_ = nullptr;
  size_t stride_ = 0;
};

void swap(stride_reference&, stride_reference&) = delete;

class stride_iterator
: public iterator<random_access_iterator_tag, stride_reference, ptrdiff_t,
                  stride_iterator, stride_reference>
{
 public:
  stride_iterator(uint8_t* addr, size_t stride) noexcept
  : ref_(addr, stride)
  {}

  stride_iterator() = default;

  stride_iterator(const stride_iterator& other) noexcept
  : ref_(other.ref_)
  {}

  stride_iterator& operator=(const stride_iterator& other) noexcept {
    ref_ = other.ref_;
    return *this;
  }

  stride_reference operator*() const noexcept { return ref_; }
  void* operator->() = delete;

  stride_iterator& operator++() noexcept {
    ref_.addr_ += ref_.stride_;
    return *this;
  }

  stride_iterator& operator--() noexcept {
    ref_.addr_ -= ref_.stride_;
    return *this;
  }

  stride_iterator operator++(int) noexcept {
#if 0
    stride_iterator clone = *this;
    ++*this;
    return clone;
#endif
    return *this;
  }

  stride_iterator operator--(int) noexcept {
    stride_iterator clone = *this;
    --*this;
    return clone;
  }

  difference_type operator-(const stride_iterator& other) const noexcept {
    return (other.ref_.addr_ - ref_.addr_) / ref_.stride_;
  }

  stride_iterator& operator+=(difference_type n) noexcept {
    ref_.addr_ += (n * ref_.stride_);
    return *this;
  }

  stride_iterator& operator-=(difference_type n) noexcept {
    ref_.addr_ -= (n * ref_.stride_);
    return *this;
  }

  stride_iterator operator+(difference_type n) const noexcept {
    stride_iterator clone(*this);
    clone += n;
    return clone;
  }

  stride_iterator operator-(difference_type n) const noexcept {
    stride_iterator clone(*this);
    clone -= n;
    return clone;
  }

  void* get_addr() const noexcept { return ref_.addr_; }
  size_t get_stride() const noexcept { return ref_.stride_; }

 private:
  stride_reference ref_;
};

stride_iterator stride_reference::operator&() const noexcept {
  return stride_iterator(addr_, stride_);
}

void iter_swap(stride_iterator x, stride_iterator y) noexcept {
  using _namespace(std)::swap;

  assert(x.get_addr() == y.get_addr());
  assert(x.get_stride() == y.get_stride());

  size_t stride = x.get_stride();
  uint8_t* x_i = static_cast<uint8_t*>(x.get_addr());
  uint8_t* y_i = static_cast<uint8_t*>(y.get_addr());
  while (stride-- > 0) swap(*x_i++, *y_i++);
}

stride_reference ref(const stride_reference& x) noexcept { return x; }
stride_reference ref(stride_reference&& x) noexcept { return x; }
const stride_reference cref(const stride_reference& x) noexcept { return x; }
const stride_reference cref(const stride_reference&& x) noexcept { return x; }

bool operator==(const stride_iterator& x, const stride_iterator& y) noexcept {
  return (*x).get() == (*y).get();
}
bool operator!=(const stride_iterator& x, const stride_iterator& y) noexcept {
  return !(x == y);
}
bool operator<(const stride_iterator& x, const stride_iterator& y) noexcept {
  return (*x).get() < (*y).get();
}
bool operator>(const stride_iterator& x, const stride_iterator& y) noexcept {
  return y < x;
}
bool operator<=(const stride_iterator& x, const stride_iterator& y) noexcept {
  return !(y < x);
}
bool operator>=(const stride_iterator& x, const stride_iterator& y) noexcept {
  return !(x < y);
}

} /* namespace std::<unnamed> */

void qsort(void* base, size_t n, size_t stride,
           int (*cmp)(const void*, const void*)) {
  using placeholders::_1;
  using placeholders::_2;

  stride_iterator b = stride_iterator(static_cast<uint8_t*>(base), stride);
  stride_iterator e = b + n;
  sort(b, e,
       [cmp](stride_reference x, stride_reference y) -> bool {
         return cmp(x.get(), y.get()) < 0;
       });
}


} /* namespace std */
