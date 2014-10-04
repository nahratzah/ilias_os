#ifndef _STDIMPL_SHARED_PTR_OWNERSHIP_H_
#define _STDIMPL_SHARED_PTR_OWNERSHIP_H_

#include <cdecl.h>
#include <atomic>
#include <memory>
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
  static void acquire(shared_ptr_ownership*, size_t = 1) noexcept;
  static void release(shared_ptr_ownership*, size_t = 1) noexcept;
  void shared_ptr_acquire_from_shared_ptr(size_t = 1) noexcept;
  bool shared_ptr_acquire_from_weak_ptr() noexcept;
  void shared_ptr_release(size_t = 1) noexcept;

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
 private:
  using placement = aligned_storage_t<sizeof(T), alignof(T)>;
  using data_type = tuple<placement, A>;

 public:
  placement_shared_ptr_ownership() = delete;
  placement_shared_ptr_ownership(const placement_shared_ptr_ownership&) =
      delete;
  placement_shared_ptr_ownership& operator=(
      const placement_shared_ptr_ownership&) = delete;

  template<typename... Args>
  placement_shared_ptr_ownership(A, Args&&...);
  ~placement_shared_ptr_ownership() noexcept = default;

  void* get_deleter(const type_info&) noexcept override;
  T* get() noexcept;

 private:
  void release_pointee() noexcept override;
  void destroy_me() noexcept override;

  data_type data_;
};

template<typename T, typename A, typename... Args>
auto create_placement_shared_ptr_ownership(A alloc_arg, Args&&... args) ->
    placement_shared_ptr_ownership<
        T, typename allocator_traits<A>::template rebind_alloc<void>>*;


} /* namespace impl */
_namespace_end(std)

#include <stdimpl/shared_ptr_ownership-inl.h>

#endif /* _STDIMPL_SHARED_PTR_OWNERSHIP_H_ */
