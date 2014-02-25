#ifndef _STDIMPL_ALLOC_BASE_H_
#define _STDIMPL_ALLOC_BASE_H_

#include <cdecl.h>
#include <memory>

_namespace_begin(std)
namespace impl {

template<typename Alloc, bool Propagate =
         allocator_traits<Alloc>::propagate_on_container_swap::value>
struct alloc_swap {
  static inline void do_alloc_swap(Alloc& a, Alloc& b)
      noexcept(noexcept(swap(a, b))) {
    swap(a, b);
  }
};

template<typename Alloc>
struct alloc_swap<Alloc, false> {
  static inline void do_alloc_swap(Alloc& a, Alloc& b) noexcept {}
};

template<typename Alloc, size_t SZ = sizeof(Alloc)> class alloc_base {
 public:
  using allocator_type = Alloc;

  alloc_base() = delete;

  allocator_type get_allocator() const noexcept {
    return alloc_;
  }

 protected:
  alloc_base(const allocator_type& alloc)
  : alloc_(alloc)
  {}

  ~alloc_base() noexcept {}

  allocator_type& get_allocator_() noexcept {
    return alloc_;
  }

  const allocator_type& get_allocator_() const noexcept {
    return alloc_;
  }

  typename allocator_traits<allocator_type>::pointer allocate_(
      typename allocator_traits<allocator_type>::size_type n,
      void* hint = nullptr) {
    return allocator_traits<allocator_type>::allocate(get_allocator_(),
                                                      n, hint);
  }

  void deallocate_(typename allocator_traits<allocator_type>::pointer p,
                   typename allocator_traits<allocator_type>::size_type n) {
    allocator_traits<allocator_type>::deallocate(get_allocator_(), p, n);
  }

  typename allocator_traits<allocator_type>::pointer allocate_dfl(
      typename allocator_traits<allocator_type>::size_type n,
      void* hint = nullptr) {
    auto p = allocate_(n, hint);
    auto i = p;

    try {
      for (auto e = p + n; i != e; ++i)
        allocator_traits<allocator_type>::construct(get_allocator_(), i);
      return p;
    } catch (...) {
      while (i-- != p)
        allocator_traits<allocator_type>::destroy(get_allocator_(), i);
      throw;
    }
  }

  void deallocate_dfl(typename allocator_traits<allocator_type>::pointer p,
                      typename allocator_traits<allocator_type>::size_type n) {
    auto i = p + n;
    while (i-- != p)
      allocator_traits<allocator_type>::destroy(get_allocator_(), i);
    deallocate_(p, n);
  }

  void swap_(alloc_base& o)
      noexcept(noexcept(alloc_swap<allocator_type>::do_alloc_swap(
        declval<allocator_type&>(), declval<allocator_type&>())))
  {
    alloc_swap<allocator_type>::do_alloc_swap(get_allocator_(),
                                              o.get_allocator_());
  }

 private:
  allocator_type alloc_;
};

/*
 * Specialization: use empty base optimization to optimize away
 * empty allocator class.
 */
template<typename Alloc> class alloc_base<Alloc, 0U>
: private Alloc {
 public:
  using allocator_type = Alloc;

  alloc_base() = delete;

  allocator_type get_allocator() const noexcept {
    return static_cast<const allocator_type&>(*this);
  }

 protected:
  alloc_base(const allocator_type& alloc)
  : Alloc(alloc)
  {}

  ~alloc_base() noexcept {}

  allocator_type& get_allocator_() noexcept {
    return static_cast<allocator_type&>(*this);
  }

  const allocator_type& get_allocator_() const noexcept {
    return static_cast<const allocator_type&>(*this);
  }

  typename allocator_traits<allocator_type>::pointer allocate_(
      typename allocator_traits<allocator_type>::size_type n,
      void* hint = nullptr) {
    return allocator_traits<allocator_type>::allocate(get_allocator_(),
                                                      n, hint);
  }

  void deallocate_(typename allocator_traits<allocator_type>::pointer p,
                   typename allocator_traits<allocator_type>::size_type n) {
    allocator_traits<allocator_type>::deallocate(get_allocator_(), p, n);
  }

  typename allocator_traits<allocator_type>::pointer allocate_dfl(
      typename allocator_traits<allocator_type>::size_type n,
      void* hint = nullptr) {
    auto p = allocate_(n, hint);
    auto i = p;

    try {
      for (auto e = p + n; i != e; ++i)
        allocator_traits<allocator_type>::construct(get_allocator_(), i);
      return p;
    } catch (...) {
      while (i-- != p)
        allocator_traits<allocator_type>::destroy(get_allocator_(), i);
      throw;
    }
  }

  void deallocate_dfl(typename allocator_traits<allocator_type>::pointer p,
                      typename allocator_traits<allocator_type>::size_type n) {
    auto i = p + n;
    while (i-- != p)
      allocator_traits<allocator_type>::destroy(get_allocator_(), i);
    deallocate_(p, n);
  }

  void swap_(alloc_base& o)
      noexcept(noexcept(alloc_swap<allocator_type>::do_alloc_swap(
        declval<allocator_type&>(), declval<allocator_type&>())))
  {
    alloc_swap<allocator_type>::do_alloc_swap(get_allocator_(),
                                              o.get_allocator_());
  }
};

} /* namespace std::impl */
_namespace_end(std)

#endif /* _STDIMPL_ALLOC_BASE_H_ */
