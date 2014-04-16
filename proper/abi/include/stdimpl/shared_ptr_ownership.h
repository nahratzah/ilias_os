#ifndef _STDIMPL_SHARED_PTR_OWNERSHIP_H_
#define _STDIMPL_SHARED_PTR_OWNERSHIP_H_

#include <cdecl.h>
#include <atomic>
#include <typeinfo>

_namespace_begin(std)
namespace impl {


/*
 * Basic shared_ptr ownership.
 *
 * - is reference counted (owned by both shared_ptr and weak_ptr).
 * - contains an additional reference counter (owned by shared_ptr only).
 * - contains an allocator (actual type is not known).
 * - is allocated with said allocator.
 *
 * Invariant: 0 <= shared_refcount_ <= refcount_
 */
class shared_ptr_ownership {
 protected:
  shared_ptr_ownership() noexcept = default;
  virtual ~shared_ptr_ownership() noexcept;

 public:
  shared_ptr_ownership(const shared_ptr_ownership&) = delete;
  shared_ptr_ownership& operator=(const shared_ptr_ownership&) = delete;

  virtual void* get_deleter(const type_info&) noexcept = 0;

  long get_shared_refcount() const noexcept;
  long count_ptrs_to_me() const noexcept;
  static void acquire(shared_ptr_ownership*) noexcept;
  static void release(shared_ptr_ownership*) noexcept;
  void shared_ptr_acquire_from_shared_ptr() noexcept;
  bool shared_ptr_acquire_from_weak_ptr() noexcept;
  void shared_ptr_release() noexcept;

  template<typename T, typename U>
  auto weak_ptr_convert(U* ptr) noexcept ->
      enable_if_t<!is_same<T, U>::value, T*>;
  template<typename T, typename U>
  auto weak_ptr_convert(U* ptr) noexcept ->
      enable_if_t<is_same<T, U>::value, T*>;

 private:
  virtual void release_pointee() noexcept = 0;
  virtual void destroy_me() noexcept = 0;
  static void __attribute__((noreturn)) panic_no_references() noexcept;
  static void destroy_(shared_ptr_ownership*) noexcept;
  void release_pointee_() noexcept;

  mutable atomic<long> refcount_{ 1 };
  mutable atomic<long> shared_refcount_{ 1 };
};


template<typename T, typename A>
class placement_shared_ptr_ownership
: public shared_ptr_ownership
{
 public:
  using offset_type = uint16_t;

 private:
  union placement {
    placement(nullptr_t) noexcept {}
    ~placement() noexcept {}

    T value_;
  };

  using data_type = tuple<placement, A, offset_type>;

 public:
  template<typename... Args>
  placement_shared_ptr_ownership(A, offset_type, Args&&...);
  ~placement_shared_ptr_ownership() noexcept = default;

  void* get_deleter(const type_info&) noexcept override;

  static constexpr size_t n_elems() noexcept {
    size_t sz = sizeof(placement_shared_ptr_ownership);
    size_t align = alignof(placement_shared_ptr_ownership);
    size_t alloc_sz = sizeof(typename allocator_traits<A>::value_type);
    size_t alloc_align = alignof(typename allocator_traits<A>::value_type);

    /* Number of bytes we may have to shift, due to alignment constraints. */
    size_t align_slack = (align > alloc_align ? align - alloc_align : 0);

    return (sz + align_slack + alloc_sz - 1) / alloc_sz;
  }

  T* get() noexcept { return &get<0>(data_).value; }

 private:
  void release_pointee() noexcept override;
  void destroy_me() noexcept override;

  data_type data_;
};


} /* namespace impl */
_namespace_end(std)

#include <stdimpl/shared_ptr_ownership-inl.h>

#endif /* _STDIMPL_SHARED_PTR_OWNERSHIP_H_ */
